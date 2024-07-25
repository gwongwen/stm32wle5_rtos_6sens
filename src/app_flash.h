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
#include <zephyr/fs/nvs.h>
#include <zephyr/drivers/flash.h>
#include <zephyr/storage/flash_map.h>

//  ======== globals ===============================================
struct vtph {
	uint16_t vbat;
	uint16_t temp;
	uint16_t press;
	uint16_t hum;
};

//  ======== defines ===============================================
#define NVS_PARTITION			storage_partition
#define NVS_PARTITION_DEVICE	FIXED_PARTITION_DEVICE(NVS_PARTITION)
#define NVS_PARTITION_OFFSET	FIXED_PARTITION_OFFSET(NVS_PARTITION)
#define NVS_MAX_RECORDS         96       // 1 structure: 4 samples (vbat, temp, press, hum) = 8 bytes
                                         // 4 samples every 15 min
#define NVS_DEVNONCE_ID			0										 
#define NVS_SENSOR_ID           1                                       

//  ======== prototypes ============================================
int8_t app_flash_init(struct nvs_fs *fs);
int8_t app_flash_store(struct nvs_fs *fs, struct vtph *data);
int8_t app_flash_handler(struct nvs_fs *fs);
int8_t app_flash_init_param(struct nvs_fs *fs, uint16_t id, void *data);

#endif /* APP_FLASH_H */