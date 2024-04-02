/*
 * Copyright (c) 2023
 * Regis Rousseau
 * Univ Lyon, INSA Lyon, Inria, CITI, EA3720
 * SPDX-License-Identifier: Apache-2.0
 */

#include "app_vbat.h"

//  ======== app_stm32_vbat_init ============================================
int8_t app_stm32_vbat_init(const struct device *dev)
{
    // getting internal vbat ADC device at GPIO PB14
    dev = DEVICE_DT_GET_ONE(st_stm32_vbat);

    if (dev == NULL) {
        printk("error: no stm32 vbat device found\n");
		return 0;
	}

    if (!device_is_ready(dev)) {
		printk("error: stm32 vbat is not ready\n");
		return 0;
	} else {
        printk("- found device \"%s\", getting vbat data\n", dev->name);
    }
    return 0;
}

//  ======== app_stm32_get_vbat =============================================
int16_t app_stm32_get_vbat(const struct device *dev)
{
    struct sensor_value bat_int32;
    int16_t bat_int16;
    int8_t ret = 0;

    // getting vbat ADC device
    dev = DEVICE_DT_GET_ONE(st_stm32_vbat);

    // fetching data
    ret = sensor_sample_fetch(dev);
    if (ret < 0 && ret != -EBADMSG) {        
	    printk("error: stm32 vbat sample is not up to date\n");
	    return 0;
    }

    // getting channel function
	ret = sensor_channel_get(dev, SENSOR_CHAN_VOLTAGE, &bat_int32);
    if (ret < 0) {
        printk("error: can't read sensor channels\n");
	    return 0;
    }

    // battery level received and converted from channel get
    // resolution 12bits: 0 to 4095 (uint16)
    bat_int16 = (int16_t)(bat_int32.val1*100 + bat_int32.val2/10000);
    printk("stm32 vbat: %d\n", bat_int16);
    return bat_int16;
}
