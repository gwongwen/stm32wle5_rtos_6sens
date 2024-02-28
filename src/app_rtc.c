/*
 * Copyright (c) 2023
 * Regis Rousseau
 * Univ Lyon, INSA Lyon, Inria, CITI, EA3720
 * SPDX-License-Identifier: Apache-2.0
 */

#include "app_rtc.h"
#include "drv_rtc.h"

//  ======== app_stm32_rtc_init =============================================
int8_t app_stm32_rtc_init(const struct device *dev)
{
    // getting internal vbat ADC device at GPIO PB14
    dev = DEVICE_DT_GET(TIMER);

    if (dev == NULL) {
        printk("error: no stm32 rtc device found\n");
		return 0;
	}

    if (!device_is_ready(dev)) {
		printk("error: stm32 rtc is not ready\n");
		return 0;
	} else {
        printk("- found device \"%s\", getting timestamp\n", dev->name);
    }
    return 0;
}

//  ======== app_stm32_get_rtc ==============================================
int8_t app_stm32_get_rtc(const struct device *dev)
{
    struct rtc_raw_ts *timestamp;
    struct rtc_ts raw_time;

    stm32_rtc_initialize(dev);
    stm32_rtc_getTimestamp(timestamp);
    
//    raw_time = rtc_ts_fromRaw(timestamp);
//   printk("d: %d, t: %d, ms: %d\n", raw_time.d, raw_time.t, raw_time.ms);

    return 0;
}



