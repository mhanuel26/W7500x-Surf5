/*
 * Copyright (c) 2022, Erich Styger
 * Copyright (c) 2024, Manuel Abbatemarco
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "surf5LittleFS_block_device.h"
#include "surf5LittleFs.h"
#include "surf5Flash.h"
#include "lfs.h"


int Surf5LittleFS_block_device_read(const struct lfs_config *c, lfs_block_t block, lfs_off_t off, void *buffer, lfs_size_t size) {
  uint8_t res;

  res = Surf5Flash_Read((void*)((block+Surf5LittleFS_CONFIG_BLOCK_OFFSET) * c->block_size + off), buffer, size);
  if (res != ERR_OK) {
    return LFS_ERR_IO;
  }
  return LFS_ERR_OK;
}

int Surf5LittleFS_block_device_prog(const struct lfs_config *c, lfs_block_t block, lfs_off_t off, const void *buffer, lfs_size_t size) {
  uint8_t res;

  res = Surf5Flash_Program((void*)((block+Surf5LittleFS_CONFIG_BLOCK_OFFSET) * c->block_size + off), buffer, size);
  if (res != ERR_OK) {
    return LFS_ERR_IO;
  }
  return LFS_ERR_OK;
}

int Surf5LittleFS_block_device_erase(const struct lfs_config *c, lfs_block_t block) {
  uint8_t res;

  res = Surf5Flash_Erase((void*)((block+Surf5LittleFS_CONFIG_BLOCK_OFFSET) * c->block_size), c->block_size);
  if (res != ERR_OK) {
    return LFS_ERR_IO;
  }
  return LFS_ERR_OK;
}

int Surf5LittleFS_block_device_sync(const struct lfs_config *c) {
  return LFS_ERR_OK;
}

int Surf5LittleFS_block_device_deinit(void) {
  return LFS_ERR_OK;
}

int Surf5LittleFS_block_device_init(void) {
  return LFS_ERR_OK;
}
