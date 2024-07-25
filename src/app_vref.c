/*
 * Copyright (c) 2023
 * Regis Rousseau
 * Univ Lyon, INSA Lyon, Inria, CITI, EA3720
 * SPDX-License-Identifier: Apache-2.0
 */

#include "app_vref.h"

//  ======== app_stm32_vbat_init ============================================
int8_t app_stm32_vref_init(const struct device *dev)
{
    // getting internal vbat ADC device at GPIO IN5 - PB1
    dev = DEVICE_DT_GET_ONE(st_stm32_vref);

    if (dev == NULL) {
        printk("error: no stm32 vref device found\n");
		return 0;
	}

    if (!device_is_ready(dev)) {
		printk("error: stm32 vref is not ready\n");
		return 0;
	} else {
        printk("- found device \"%s\", getting vref data\n", dev->name);
    }
    return 0;
}

//  ======== app_stm32_get_vbat =============================================
uint16_t app_stm32_get_vref(const struct device *dev)
{
    struct sensor_value vref_int32;
    uint16_t vref_uint16;
    int8_t ret = 0;

    // fetching data
    ret = sensor_sample_fetch(dev);
    if (ret < 0 && ret != -EBADMSG) {        
	    printk("error: stm32 vref sample is not up to date\n");
	    return 0;
    }

    // getting channel function
	ret = sensor_channel_get(dev, SENSOR_CHAN_VOLTAGE, &vref_int32);
    if (ret < 0) {
        printk("error: can't read sensor channels\n");
	    return 0;
    }

    // reference level received and converted from channel get
    // resolution 12bits: 0 to 4095 (uint16)
    vref_uint16 = (uint16_t)(sensor_value_to_milli(&vref_int32));
    printk("stm32 vref: %"PRIu16"\n", vref_uint16);
    return vref_uint16;
}
