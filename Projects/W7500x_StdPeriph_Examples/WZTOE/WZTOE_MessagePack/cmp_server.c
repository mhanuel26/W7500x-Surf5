#include "cmp_server.h"
#include "wizchip_conf.h"
#include <string.h>
#include "socket.h"

typedef struct
{
	uint8_t sn;
	uint8_t* rxbuf;
	uint8_t* txbuf;
	uint8_t *txptr;				
	uint8_t tx_flag;			
	uint16_t port;
	cmp_ext_t fext;			
}tcp_handler;


static uint8_t rxBuf[CFG_BUF_SIZE];
static uint8_t txBuf[CFG_BUF_SIZE];
static uint8_t cmpBuf[CMP_RAM_BUF];
static tcp_handler cfg_handler;
static cmp_ctx_t ether_ctx;
static cmp_ctx_t cmd_ctx;


static void flush_ethernet_writer(cmp_ctx_t *cmp);
static bool ethernet_skip(cmp_ctx_t *cmp, size_t count);
static bool ethernet_reader(cmp_ctx_t *cmp, void *data, size_t sz);
static size_t ethernet_writer(cmp_ctx_t *cmp, const void *data, size_t count);
static bool extract_rx_frame(cmp_ctx_t *ctx);
static void process_cmd(cmp_ctx_t *ctx);
static bool extract_rx_frame(cmp_ctx_t *ctx);

/***********************************************************************/
/***************************    CODE         ***************************/
/***********************************************************************/

void error_and_exit(const char *msg) {
    printf("%s\n\r", msg);
    while(1);
}


/***********************************************************************/
/********************     MCU RAM CMP wrapper functions ****************/
/***********************************************************************/

static void rewind_ram(cmp_ctx_t *ctx){
    ctx->buf = &cmpBuf[0];
}

// static void rewind_ram_count(cmp_ctx_t *ctx, size_t count){
//     ctx->buf -= count;
// }

static bool ram_skip(cmp_ctx_t *ctx, size_t count){
	ctx->buf = count + (uint8_t*)ctx->buf;
	return true;
}

static int get_cmp_size(cmp_ctx_t *ctx){
    return ((uint8_t *)ctx->buf - &cmpBuf[0]);
}

static bool ram_reader(cmp_ctx_t *ctx, void *data, size_t limit) {
    memcpy(data, (void *)ctx->buf, limit);
    ctx->buf = limit + (uint8_t*)ctx->buf;
    return true;
}

static size_t ram_writer(cmp_ctx_t *ctx, const void *data, size_t count) {
    memcpy((void *)ctx->buf, data, count);
    ctx->buf = count +(uint8_t*)ctx->buf;
    return count;
}

/***********************************************************************/
/********************     TCP server functions		    ****************/
/***********************************************************************/

void init_ether_cfg(void){
	tcp_handler *tcp_ptr = &cfg_handler;

	tcp_ptr->rxbuf = &rxBuf[0];
	tcp_ptr->txbuf = &txBuf[0];
	tcp_ptr->sn = SOCK_CFGS;
	tcp_ptr->port = PORT_CFGS;
	tcp_ptr->txptr = tcp_ptr->txbuf;
	tcp_ptr->tx_flag = 0;
	cmp_init(&ether_ctx, &cfg_handler, ethernet_reader, ethernet_skip, ethernet_writer);		// init the CMP ethernet handler
	cmp_init(&cmd_ctx, cmpBuf, ram_reader, ram_skip, ram_writer);
}

static uint16_t available(uint8_t sn){
	return getSn_RX_RSR(sn);
}

static bool ethernet_reader(cmp_ctx_t *cmp, void *data, size_t sz){      // ctx not used here but leave it for compatibility
    uint16_t size = 0;
    uint8_t ret;
    tcp_handler *sHandler = (tcp_handler*)cmp->buf;

	if((size = getSn_RX_RSR(sHandler->sn)) > 0){
		if(size < sz){
			wiz_recv_ignore(sHandler->sn, size);		// in a socket there will be no fragments with this kind of packages
		}
		ret = recv(sHandler->sn, data, sz);
		if(ret <= 0)
			return false;				// some error...
	}
    return true;
}

static bool ethernet_skip(cmp_ctx_t *cmp, size_t count){
    uint16_t size = 0;
	tcp_handler *sHandler = (tcp_handler*)cmp->buf;
    if((size = getSn_RX_RSR(sHandler->sn)) > 0){
		if(count <= size){
			wiz_recv_ignore(sHandler->sn, size);		// in a socket there will be no fragments with this kind of packages
		}
    }
	return true;
}

static void flush_ethernet_writer(cmp_ctx_t *cmp){
	cfg_handler.tx_flag = 1;
	ethernet_writer(cmp, NULL, 0);
}

static size_t ethernet_writer(cmp_ctx_t *cmp, const void *data, size_t count) {
	size_t res;
	int16_t ret;
	tcp_handler *sHandler = (tcp_handler*)cmp->buf;
	if(sHandler->tx_flag){
		uint16_t sentsize = 0;
		uint16_t size = sHandler->txptr - sHandler->txbuf;
		while(size != sentsize)
		{
			ret = send(sHandler->sn, sHandler->txbuf+sentsize, size-sentsize);
			if(ret < 0)
			{
				close(sHandler->sn);
				return 0;
			}
			sentsize += ret; 			// Don't care SOCKERR_BUSY, because it is zero.
		}
		sHandler->tx_flag = 0;						// we will keep in memory until a frame is ready
		sHandler->txptr = sHandler->txbuf;			// init the pointer to start over next time
		res = sentsize;
	}else{
		// copy the data to socket handler memory
		memcpy(sHandler->txptr, data, sizeof(uint8_t)*count);
		sHandler->txptr += count;
		res = count;
	}
	return res;
}

int32_t process_cfg_socket(void)
{
   int32_t ret;
   cmp_ctx_t *cmp = &ether_ctx;
   tcp_handler *sHandler = (tcp_handler*)cmp->buf;


#ifdef DEBUG_ETHERNET
   uint8_t destip[4];
   uint16_t destport;
#endif
   switch(getSn_SR(sHandler->sn))
   {
      case SOCK_ESTABLISHED :
         if(getSn_IR(sHandler->sn) & Sn_IR_CON)
         {
#ifdef DEBUG_ETHERNET
			getSn_DIPR(sHandler->sn, destip);
			destport = getSn_DPORT(sHandler->sn);
			printf("%d:Connected - %d.%d.%d.%d : %d\r\n",sHandler->sn, destip[0], destip[1], destip[2], destip[3], destport);
#endif
			setSn_IR(sHandler->sn,Sn_IR_CON);
         }
         if(available(sHandler->sn) > 0){
        	 if (extract_rx_frame(cmp))
        		 process_cmd(cmp);
         }
         break;
      case SOCK_CLOSE_WAIT :
#ifdef DEBUG_ETHERNET
         //printf("%d:CloseWait\r\n",sHandler->sn);
#endif
         if((ret = disconnect(sHandler->sn)) != SOCK_OK)
        	 return ret;
#ifdef DEBUG_ETHERNET
         printf("%d:Socket Closed\r\n", sHandler->sn);
#endif
         break;
      case SOCK_INIT :
#ifdef DEBUG_ETHERNET
    	 printf("%d:Listen, TCP command server, port [%d]\r\n", sHandler->sn, sHandler->port);
#endif
         if( (ret = listen(sHandler->sn)) != SOCK_OK)
        	 return ret;
         break;
      case SOCK_CLOSED:
#ifdef DEBUG_ETHERNET
         printf("%d:TCP command server start\r\n",sHandler->sn);
#endif
         if((ret = socket(sHandler->sn, Sn_MR_TCP, sHandler->port, 0x00)) != sHandler->sn)
        	 return ret;
#ifdef DEBUG_ETHERNET
         printf("%d:Socket opened.\r\n", sHandler->sn);
#endif
         break;
      default:
         break;
   }
   //

   return 1;
}

static bool extract_rx_frame(cmp_ctx_t *ctx){
	uint8_t *data = ((tcp_handler*)ctx->buf)->rxbuf;			// Our TCP buffer
	cmp_ext_t *fext = &((tcp_handler*)ctx->buf)->fext;		// The ext to save type and size
    if(data == NULL){		// the memory should be initialized by upper layer
        return false;
    }
    cmp_object_t obj;
    if (!cmp_read_object(ctx, &obj)) {
        error_and_exit(cmp_strerror(ctx));
    }
    switch (obj.type) {
        case CMP_TYPE_EXT8:
        case CMP_TYPE_EXT16:
        case CMP_TYPE_EXT32:
        case CMP_TYPE_FIXEXT1:
        case CMP_TYPE_FIXEXT2:
        case CMP_TYPE_FIXEXT4:
        case CMP_TYPE_FIXEXT8:
        case CMP_TYPE_FIXEXT16:
            fext->type = obj.as.ext.type;
            fext->size = obj.as.ext.size;
            if(!ethernet_reader(ctx, data, fext->size))
                return false;
#if DEBUG_ETHERNET
            printf("Type=%02x\n\r", fext->type);
            printf("Size=%02x\n\r", (uint16_t)fext->size);
            printf("\n\rData = ");
            for(uint32_t k=0; k<fext->size; k++){
                if(k%16==0)
                    printf("\n\r");
                printf("%02x ", data[k]);
            }
            printf("\n\r");
#endif
            rewind_ram(&cmd_ctx);
            memcpy(cmpBuf, data, fext->size);
        break;
        default:
        	ethernet_reader(ctx, data, CFG_BUF_SIZE+1);			// this will discard all data received
        	return false;
        break;
    }
    return true;
}


static void process_cmd(cmp_ctx_t *ctx){
	cmp_ext_t *fext = &((tcp_handler*)ctx->buf)->fext;
	cmp_object_t obj;
	uint16_t items;
	uint32_t prop_sz;
	char property[100];
    uint32_t str_size = 0;

	switch(fext->type){
		case SET_CONFIG:

		    if (!cmp_read_object(&cmd_ctx, &obj)) {
		        error_and_exit(cmp_strerror(&cmd_ctx));
		    }
		    switch(obj.type){
		    	case CMP_TYPE_FIXMAP:
		    	case CMP_TYPE_MAP16:
		    	case CMP_TYPE_MAP32:
		    		items = obj.as.map_size;
		    		printf("Array Size = %d \n\r ", items);

		    	break;
                case CMP_TYPE_FIXSTR:
                    items = obj.as.str_size;
                    prop_sz = sizeof(property);
                    memset(property, 0, sizeof(property));       
                    rewind_ram(&cmd_ctx);            
                    cmp_read_str(&cmd_ctx, property, &prop_sz);
                    printf("String Size  = %d \r\n", items);
                    printf("String Data:  = %s \r\n", property);
                break;
		    }
		break;
		default:

		break;
	}
}