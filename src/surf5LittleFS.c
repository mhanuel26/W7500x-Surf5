#include "surf5LittleFs.h"
#include "surf5LittleFS_block_device.h"
#include "lfs.h"

// variables used by the filesystem
lfs_t lfs;
lfs_file_t file;


// configuration of the filesystem is provided by this struct
const struct lfs_config cfg = {
    // block device operations
    .read  = Surf5LittleFS_block_device_read,
    .prog  = Surf5LittleFS_block_device_prog,
    .erase = Surf5LittleFS_block_device_erase,
    .sync  = Surf5LittleFS_block_device_sync,

    // block device configuration
    .read_size = Surf5LittleFS_CONFIG_FILESYSTEM_READ_BUFFER_SIZE,
    .prog_size = Surf5LittleFS_CONFIG_FILESYSTEM_PROG_BUFFER_SIZE,
    .block_size = Surf5LittleFS_CONFIG_BLOCK_SIZE,
    .block_count = Surf5LittleFS_CONFIG_BLOCK_COUNT,
    .cache_size = Surf5LittleFS_CONFIG_FILESYSTEM_CACHE_SIZE,
    .lookahead_size = Surf5LittleFS_CONFIG_FILESYSTEM_LOOKAHEAD_SIZE,
    .block_cycles = 500,
};


