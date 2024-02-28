/*
 * Copyright (c) 2023
 * Regis Rousseau
 * Univ Lyon, INSA Lyon, Inria, CITI, EA3720
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef APP_RTC_H
#define APP_RTC_H

//  ======== includes ============================================
#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/counter.h>
#include <zephyr/posix/time.h>

//  ======== defines =============================================
#define TIMER DT_INST(0, st_stm32_rtc)

//  ======== prototypes ===========================================
int8_t app_stm32_rtc_init(const struct device *dev);
int8_t app_stm32_get_rtc(const struct device *dev);
#endif /* APP_SENSOR_H */

