/*
 * Copyright (c) 2023
 * Regis Rousseau
 * Univ Lyon, INSA Lyon, Inria, CITI, EA3720
 * SPDX-License-Identifier: Apache-2.0
 */

#include "app_vbat.h"
#include "app_bme280.h"
#include "app_flash.h"

//  ======== globals ============================================
struct nvs_data {
	uint16_t vbat;
	uint16_t temp;
	uint16_t press;
	uint16_t hum;
};	

int8_t ind_f;		// declaration of isr index

//  ======== app_flash_init =====================================
int8_t app_flash_init(struct nvs_fs *fs)
{
	struct flash_pages_info info;
	int8_t ret;
	ssize_t size;

	// configuration of storage partition in flash memory
	fs->flash_device = NVS_PARTITION_DEVICE;
	if (!device_is_ready(fs->flash_device)) {
		printk("flash device \"%s\" is not ready\n", fs->flash_device->name);
		return 0;
	} else {
		printk("- found device: \"%s\", getting nvs memory\n", fs->flash_device->name);
	}

	fs->offset = NVS_PARTITION_OFFSET;
	ret = flash_get_page_info_by_offs(fs->flash_device, fs->offset, &info);
	if (ret) {
		printk("unable to get page info. error: %d\n", ret);
		return 0;
	}

	fs->sector_size = info.size;
	if (!fs->sector_size || fs->sector_size % info.size) {
		printk("invalid sector size\n");
		return 0;
	}

	fs->sector_count = 3U;
	ret = nvs_mount(fs);
	if (ret) {
		printk("flash init failed. error: %d\n", ret);
		return 0;
	}

	(void)nvs_delete(fs, NVS_SENSOR_ID);
	size =  nvs_calc_free_space(fs);
	printk("flash memory partition size: %d\n", size);
	ind_f = 0;	// initialisation of isr index
	return 0;
}

//  ======== app_flash_init_param ===============================
int8_t app_flash_init_param(struct nvs_fs *fs, uint16_t id, void *data)
{
	int8_t ret;

	ret = nvs_read(fs, id, data, sizeof(data));
	if (ret > 0) {
		printk("ID: %d, address: %s\n", id, data);
	} else {
		printk("no address found, adding %s at id %d\n", data, id);
		(void)nvs_write(fs, id, data, sizeof(data));	
	}
	return 0;
}

//  ======== app_flash_write ====================================
int8_t app_flash_write(struct nvs_fs *fs, void *data)
{
	// writing data in the first page of 2kbytes
	(void)nvs_write(fs, NVS_SENSOR_ID, data, NVS_BUFFER_SIZE);
	return 0;
}

//  ======== app_flash_read =====================================
int8_t app_flash_read(struct nvs_fs *fs)
{
	ssize_t ret;
	struct nvs_data data_rd[NVS_BUFFER_SIZE];

	// reading the first page
	(void)nvs_read(fs, NVS_SENSOR_ID, &data_rd, sizeof(data_rd));

	// printing data
	for (int8_t i = 0; i < NVS_BUFFER_SIZE; i++) {
		printk("vbat: %"PRIu16", temp: %"PRIu16", press: %"PRIu16", hum: %"PRIu16"\n",
		data_rd[i].vbat, data_rd[i].temp, data_rd[i].press, data_rd[i].hum);
	}
	return 0;		
}

//  ======== app_flash_handler ==================================
int8_t app_flash_handler(struct nvs_fs *fs)
{
	int8_t ret;
	struct nvs_data data[NVS_BUFFER_SIZE];
	static struct nvs_fs flash;
	const struct device *bme_dev;
	const struct device *bat_dev;
	const struct device *lora_dev;

	// getting all devices
	bat_dev = DEVICE_DT_GET_ONE(st_stm32_vbat);
	bme_dev = DEVICE_DT_GET_ANY(bosch_bme280);
	lora_dev = DEVICE_DT_GET(DT_ALIAS(lora0));

	// putting 48 structures in fisrt page
	if (ind_f < NVS_MAX_RECORDS) {
		data[ind_f].vbat = app_stm32_get_vbat(bat_dev);
		data[ind_f].temp = app_bme280_get_temp(bme_dev);
		data[ind_f].press = app_bme280_get_press(bme_dev);
		data[ind_f].hum = app_bme280_get_hum(bme_dev);

		// writing data in first page of eeprom
		app_flash_write(&flash, data);
		
		ind_f++;
	} else {
		// sending data onca a day
		app_lorawan_handler(lora_dev, data);

		// erasing first page at @0003 F000 (2kbytes)
		(void)nvs_delete(fs, NVS_SENSOR_ID);
		ind_f = 0;	// reset the isr index
	}
	return 0;
}

