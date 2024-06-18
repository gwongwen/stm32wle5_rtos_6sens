/*
 * Copyright (c) 2023
 * Regis Rousseau
 * Univ Lyon, INSA Lyon, Inria, CITI, EA3720
 * SPDX-License-Identifier: Apache-2.0
 */

#include "app_rom.h"

//  ======== globals ============================================
uint8_t dev_eui[] = LORAWAN_DEV_EUI;
int32_t times = 1678984720;;

struct rom_data {
	uint8_t *id;
	int32_t timestamp;
	uint16_t val;
};

int8_t ind_r;		// index used by Interrupt Service Routine

//  ======== app_rom_init =========================================
 int8_t app_rom_init(const struct device *dev)
 {
    int8_t ret;
	ssize_t size;

	// getting eeprom device
	dev = DEVICE_DT_GET(DT_ALIAS(eeprom0));

    if (dev == NULL) {
		printk("no eeprom device found. error: %d\n", dev);
		return 0;
	}
    if (!device_is_ready(dev)) {
		printk("eeprom is not ready\n");
		return 0;
	} else {
        printk("- found device \"%s\", writing/reading data\n", dev->name);
    }

	// erasing 1 page at @0x00
	ret  = flash_erase(dev, ROM_OFFSET, 1024*ROM_PAGE_SIZE);
	if (ret) {
		printk("error erasing flash. error: %d\n", ret);
	} else {
		printk("erased all pages\n");
	}

	// initialisation of isr index
	ind_r = 0;	// initialisation of isr index
	return 0;
}

//  ======== app_rom_write ========================================
int8_t app_rom_write(const struct device *dev, void *data)
{
	int8_t ret;
	dev = DEVICE_DT_GET(DT_ALIAS(eeprom0));

	// writing data in the first page of 2kbytes
	ret = flash_write(dev, ROM_OFFSET, data, sizeof(data));
	if (ret) {
		printk("error writing data. error: %d\n", ret);
	} else {
		printk("wrote %zu bytes to address 0x0003f000\n", sizeof(data));
	}
	return 0;
}

//  ======== app_rom_read =========================================
int8_t app_rom_read(const struct device *dev)
{
	int8_t ret;
	struct rom_data data[ROM_BUFFER_SIZE];
	dev = DEVICE_DT_GET(DT_ALIAS(eeprom0));

	// reading the first page
	ret = flash_read(dev, ROM_OFFSET, data, sizeof(data));
	if (ret) {
		printk("error reading data.. error: %d\n", ret);
	} else {
		printk("read %zu bytes from address 0x00\n", sizeof(data));
	}
	// printing data
	for (int8_t i = 0; i < ROM_MAX_RECORDS; i++) {
		printk("val: %"PRIu16"\n", data[i].val);
	}
	return 0;
}

//  ======== app_rom_handler ======================================
int8_t app_rom_handler(const struct device *dev)
{
	int8_t ret;
	const struct device *lora_dev;
	struct rom_data data[ROM_BUFFER_SIZE];
	uint16_t adc_val;

	// getting lora sx1276 device and eeprom device
	lora_dev = DEVICE_DT_GET(DT_ALIAS(lora0));
	dev = DEVICE_DT_GET(DT_ALIAS(eeprom0));

	// getting value of the geophone from the adc
	adc_val = app_adc_get_val();

	// definition of a trigger threshold
	if (adc_val > THRESHOLD){

		// putting 36 structures in all page (1024 pages in this eeprom)
		if (ind_r < ROM_MAX_RECORDS) {
			data[ind_r].id = dev_eui;
			data[ind_r].timestamp = times;
			data[ind_r].val = adc_val;
			ind_r++;
		} else {
			// writing data in pages of eeprom
			//	app_rom_write(dev, data);
			(void)app_flash_write(struct nvs_fs *fs, void *data);

			// sending data
			app_lorawan_handler(lora_dev, data);
			ind_r = 0;
		}
	}
	return 0;
}