/*
 * Copyright (c) 2023
 * Regis Rousseau
 * Univ Lyon, INSA Lyon, Inria, CITI, EA3720
 * SPDX-License-Identifier: Apache-2.0
 */

#include "app_bme280.h"

//  ======== app_bme280_init ============================================
int8_t app_bme280_init(const struct device *dev)
{
    // getting bme280 sensor i2c device (SDA: PA10, SCL:PA8)
    dev = DEVICE_DT_GET_ANY(bosch_bme280);
    if (dev == NULL) {
        printk("error: no bme280 device found\n");
		return 0;
	}

    if (!device_is_ready(dev)) {
		printk("error: bme280 is not ready\n");
		return 0;
	} else {
        printk("- found device \"%s\", getting bme280 data\n", dev->name);
    }
    return 0;
}

//  ======== app_bme280_get_temp ========================================
int16_t app_bme280_get_temp(const struct device *dev)
{
    struct sensor_value temp_int32;
    int16_t temp_int16;
    int8_t ret;

    // getting bme280 sensor i2c device
    dev = DEVICE_DT_GET_ANY(bosch_bme280);

    // fetching data
	ret = sensor_sample_fetch(dev);
    if (ret < 0 && ret != -EBADMSG) { 
	    printk("bme280 device sample is not up to date. error: %d\n", ret);
	    return 0;
    }

    // getting channel function
	ret = sensor_channel_get(dev, SENSOR_CHAN_AMBIENT_TEMP, &temp_int32);
    if (ret < 0) {
        printk("can't read sensor channels. error: %d\n", ret);
	    return 0;
    }

    // temperature received from channel get
    // resolution 12 bits: 0 to 4095 (uint16)
	temp_int16 = (int16_t)(temp_int32.val1*100 + temp_int32.val2 / 10000);
    printk("bme280 temp: %d\n", temp_int16);
    return temp_int16;
}

//  ======== app_bme280_get_press =======================================
uint16_t app_bme280_get_press(const struct device *dev)
{
    struct sensor_value press_int32;
    uint16_t press_uint16;
    int8_t ret;

    // getting bme280 sensor i2c device
    dev = DEVICE_DT_GET_ANY(bosch_bme280);

    // fetching data
	ret = sensor_sample_fetch(dev);
    if (ret < 0 && ret != -EBADMSG) { 
	    printk("bme280 device sample is not up to date. error: %d\n", ret);
	    return 0;
    }

    // getting channel function
	ret = sensor_channel_get(dev, SENSOR_CHAN_PRESS, &press_int32);
    if (ret < 0) {
        printk("can't read sensor channels. error: %d\n", ret);
	    return 0;
    }

    // pressure received from channel get
    // resolution 12 bits: 0 to 4095 (uint16)
	press_uint16 = ((uint16_t)(press_int32.val1*1000 + press_int32.val2 / 10000)) - 5000;
    printk("bme280 pressure: %d\n", press_uint16);
    return press_uint16;
}

//  ======== app_bme280_get_hum =========================================
uint16_t app_bme280_get_hum(const struct device *dev)
{
    struct sensor_value hum_int32;
    uint16_t hum_uint16;
    int8_t ret;

    // getting bme280 sensor i2c device
    dev = DEVICE_DT_GET_ANY(bosch_bme280);

    // fetching data
	ret = sensor_sample_fetch(dev);
    if (ret < 0 && ret != -EBADMSG) { 
	    printk("bme280 device sample is not up to date. error: %d\n", ret);
	    return 0;
    }

    // getting channel function
	ret = sensor_channel_get(dev, SENSOR_CHAN_HUMIDITY, &hum_int32);
    if (ret < 0) {
        printk("can't read sensor channels. error: %d\n", ret);
	    return 0;
    }

    // humidity received from channel get
    // resolution 12 bits: 0 to 4095 (uint16)
	hum_uint16 = (uint16_t)(hum_int32.val1*100 + hum_int32.val2 / 10000);
    printk("bme280 humidity: %d\n", hum_uint16);
    return hum_uint16;
}