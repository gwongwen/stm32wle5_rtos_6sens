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
int8_t ind;		// declaration of isr index

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
	printk("sector size: %d\n", info.size);

	// mounting data storage partition in flash memory
	fs->sector_count = 4U;
	ret = nvs_mount(fs);
	if (ret) {
		printk("flash init failed. error: %d\n", ret);
		return 0;
	}

	// cleaning data storage partition
	(void)nvs_delete(fs, NVS_SENSOR_ID);
	size =  nvs_calc_free_space(fs);
	printk("flash memory partition size: %d\n", size);

	// initialisation of isr index
	ind = 0;	
	return 0;
}

//  ======== app_flash_write ====================================
int8_t app_flash_store(struct nvs_fs *fs, struct vtph *data)
{	
	// writing data in the first page of 2kbytes
	(void)nvs_write(fs, NVS_SENSOR_ID, &data, sizeof(data));

	// printing data to be stored in memory
	for (ind = 0; ind < NVS_MAX_RECORDS; ind++) {
		printk("wrt -> vbat: %"PRIu16", temp: %d, press: %"PRIu16", hum: %"PRIu16"\n", data[ind].vbat, data[ind].temp, data[ind].press, data[ind].hum);
	}

	// reading the first page
	(void)nvs_read(fs, NVS_SENSOR_ID, &data, sizeof(data));

	// printing data stored in memory
	for (ind = 0; ind < NVS_MAX_RECORDS; ind++) {
		printk("rd -> vbat: %"PRIu16", temp: %"PRIu16", press: %"PRIu16", hum: %"PRIu16"\n", data[ind].vbat, data[ind].temp, data[ind].press, data[ind].hum);
	}
	return 0;
}

//  ======== app_flash_handler ==================================
int8_t app_flash_handler(struct nvs_fs *fs)
{
	uint16_t vbat, temp, press, hum;
	const struct device *bme_dev;
	const struct device *bat_dev;
	struct vtph data[NVS_MAX_RECORDS];

	// getting all sensor devices
	bat_dev = DEVICE_DT_GET_ONE(st_stm32_vbat);
	bme_dev = DEVICE_DT_GET_ANY(bosch_bme280);

	// putting n structures in fisrt page for this test
	while (ind < NVS_MAX_RECORDS) {
		data[ind].vbat = app_stm32_get_vbat(bat_dev);
		data[ind].temp = app_bme280_get_temp(bme_dev);
		data[ind].press = app_bme280_get_press(bme_dev);
		data[ind].hum = app_bme280_get_hum(bme_dev);
		ind++;
	}
	// writing and reading stored data
	app_flash_store(fs, &data);

	// cleaning data storage partition
	(void)nvs_delete(fs, NVS_SENSOR_ID);
	ind = 0;
	return 0;
}

