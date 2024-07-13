#ifndef __LITTLEFS_LIBRARY__
#define __LITTLEFS_LIBRARY__


#define ERR_OK          0U
#define ERR_RANGE       1U
#define ERR_VALUE       2U

#define Surf5LittleFS_CONFIG_BLOCK_SIZE    (4096)
/*!< w7500X has 32 blocks of 4k byte */
#define Surf5LittleFS_CONFIG_BLOCK_COUNT    (32) 
/*!< The memory organization is based on a main Flash memory block containing 512 sectors of 256byte or 32 blocks of 4 K byte. */
#define Surf5LittleFS_CONFIG_FILESYSTEM_READ_BUFFER_SIZE    (256)
#define Surf5LittleFS_CONFIG_FILESYSTEM_PROG_BUFFER_SIZE    (256)
#define Surf5LittleFS_CONFIG_FILESYSTEM_LOOKAHEAD_SIZE      (256)
#define Surf5LittleFS_CONFIG_FILESYSTEM_CACHE_SIZE          (256)
#define Surf5LittleFS_CONFIG_BLOCK_OFFSET                   (0)



#endif