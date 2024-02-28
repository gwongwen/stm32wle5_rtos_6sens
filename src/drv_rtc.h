/*
 * Copyright (c) 2023
 * Regis Rousseau
 * Univ Lyon, INSA Lyon, Inria, CITI, EA3720
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef DRV_RTC_H
#define DRV_RTC_H

//  ======== includes ============================================
#include <zephyr/device.h>
#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include <stm32_ll_pwr.h>
#include <stm32_ll_rcc.h>
#include <stm32_ll_rtc.h>

//  ======== defines =============================================
#define TIMER 				DT_INST(0, st_stm32_rtc)
#define RTC_PREDIV_ASYNC	0x7F	// prescaler values for LSE @ 32768 Hz
#define RTC_PREDIV_SYNC		0x00FF	// prescaler values for LSE @ 32768 Hz

// ========= typedef =============================================
typedef struct rtc_time {
	uint8_t hours;
	uint8_t minutes;
	uint8_t seconds;
} rtc_time_t;

typedef struct rtc_date {
	uint8_t day;
	uint8_t weekday;
	uint8_t month;
	uint8_t year;
} rtc_date_t;

typedef struct rtc_raw_ts {
	uint32_t d;
	uint32_t t;
	uint16_t ss;
} rtc_raw_ts_t;

typedef struct rtc_ts {
	rtc_date_t d;
	rtc_time_t t;
	uint32_t ms;
} rtc_ts_t;

typedef struct rtc_api_s {
	void (* getTimestamp) (rtc_raw_ts_t * ts);
	void (* setTime) (rtc_ts_t st);
} rtc_api_t;

//  ======== prototypes ==========================================
uint8_t stm32_rtc_readValue(unsigned int v, const unsigned int mask, const unsigned int bytePos);
rtc_ts_t rtc_ts_fromRaw(rtc_raw_ts_t * ts);
void stm32_rtc_setTime(rtc_ts_t ts);
int stm32_rtc_initialize(struct device *dev);
void stm32_rtc_getTimestamp(rtc_raw_ts_t * ts);

#endif /* DRV_RTC_H */