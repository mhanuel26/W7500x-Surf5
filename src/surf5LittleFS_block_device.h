/*
 * Copyright (c) 2022, Erich Styger
 * Copyright (c) 2024, Manuel Abbatemarco
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */


#ifndef __LITTLEFSBLOCKDEVICE_H_
#define __LITTLEFSBLOCKDEVICE_H_

#include <stdint.h>
#include "lfs.h"

int Surf5LittleFS_block_device_read(const struct lfs_config *c, lfs_block_t block, lfs_off_t off, void *buffer, lfs_size_t size);

int Surf5LittleFS_block_device_prog(const struct lfs_config *c, lfs_block_t block, lfs_off_t off, const void *buffer, lfs_size_t size);

int Surf5LittleFS_block_device_erase(const struct lfs_config *c, lfs_block_t block);

int Surf5LittleFS_block_device_sync(const struct lfs_config *c);

int Surf5LittleFS_block_device_deinit(void);

int Surf5LittleFS_block_device_init(void);

#endif /* __LITTLEFSBLOCKDEVICE_H_ */
