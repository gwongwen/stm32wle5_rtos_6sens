/*
 * Copyright (c) 2023
 * Regis Rousseau
 * Univ Lyon, INSA Lyon, Inria, CITI, EA3720
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef APP_FLASH_H
#define APP_FLASH_H

//  ======== includes ==============================================
#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/flash.h>
#include <zephyr/storage/flash_map.h>

//  ======== defines ===============================================
#define FLASH_PARTITION			storage_partition
#define FLASH_PARTITION_DEVICE	FIXED_PARTITION_DEVICE(FLASH_PARTITION)
#define FLASH_PARTITION_OFFSET	FIXED_PARTITION_OFFSET(FLASH_PARTITION)
#define FLASH_PAGE_SIZE         2048    // 2U 
#define FLASH_BUFFER_SIZE       8       // 1 structure: 4 samples (vbat, temp, press, hum) = 8 bytes
#define FLASH_STRUCT_SIZE       384     // number of strutures to store: 48 * 4 samples = 384 bytes
                                        // 4 samples per 30 mins

//  ======== prototypes ============================================
int8_t app_flash_init(const struct device *dev);
int8_t app_flash_write(const struct device *dev, uint16_t data);
int8_t app_flash_read(const struct device *dev);
int8_t app_flash_handler(const struct device *dev);

#endif /* APP_FLASH_H */