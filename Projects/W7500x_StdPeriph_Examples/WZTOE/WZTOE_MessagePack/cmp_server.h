#include "cmp.h"

#ifndef CMP_SERVER_H__
#define CMP_SERVER_H__

#define SET_CONFIG		(1U)


#define DEBUG_ETHERNET  1
#define DATA_BUF_SIZE 2048

#ifndef CFG_BUF_SIZE
	#define CFG_BUF_SIZE			512
#endif

#define CMP_RAM_BUF		512

#define SOCK_CFGS       1
#define PORT_CFGS		36430

void init_ether_cfg(void);
int32_t process_cfg_socket(void);

#endif