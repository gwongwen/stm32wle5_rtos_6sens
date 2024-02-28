/*
 * Copyright (c) 2023
 * Regis Rousseau
 * Univ Lyon, INSA Lyon, Inria, CITI, EA3720
 * SPDX-License-Identifier: Apache-2.0
 */

#include "drv_rtc.h"

//  ======== globals ============================================
const rtc_api_t rtc_stm32_API = {
	.getTimestamp = stm32_rtc_getTimestamp,
	.setTime = stm32_rtc_setTime,
};

//  ======== routines ============================================
uint8_t stm32_rtc_readValue(unsigned int v, const unsigned int mask, const unsigned int bytePos)
{
	v = v & mask;
	v >>= (bytePos * 8);
	v = __LL_RTC_CONVERT_BCD2BIN(v);
	return v;
}

rtc_ts_t rtc_ts_fromRaw(rtc_raw_ts_t * ts)
{
	rtc_ts_t r;

	r.d.day = stm32_rtc_readValue(ts->d,0x00FF0000,2);
	r.d.month = stm32_rtc_readValue(ts->d,0x0000FF00,1);;
	r.d.year = stm32_rtc_readValue(ts->d,0x000000FF,0);;

	r.t.hours = stm32_rtc_readValue(ts->t, 0x00FF0000,2);
	r.t.minutes = stm32_rtc_readValue(ts->t,0x0000FF00,1);
	r.t.seconds = stm32_rtc_readValue(ts->t,0x000000FF,0);

	r.ms = (1000 * (RTC_PREDIV_SYNC - ts->ss)) / (RTC_PREDIV_SYNC + 1);
	return r;
}

void stm32_rtc_setTime(rtc_ts_t ts)
{
	LL_RTC_DateTypeDef rtc_date = {
		.Year = ts.d.year,
		.Month = ts.d.month,
		.Day = ts.d.day,
		.WeekDay = ts.d.weekday
	};

	LL_RTC_TimeTypeDef rtc_time = {
		.TimeFormat = LL_RTC_TIME_FORMAT_AM_OR_24,
		.Hours = ts.t.hours,
		.Minutes = ts.t.minutes,
		.Seconds = ts.t.seconds
	};

	if (LL_RTC_DATE_Init(RTC, LL_RTC_FORMAT_BIN, &rtc_date) != SUCCESS) {
		printk("set date failed\n");
	}

	if (LL_RTC_TIME_Init(RTC, LL_RTC_FORMAT_BIN, &rtc_time) != SUCCESS) {
		printk("set time failed\n");
	}

	printk("set RTC clock to %02u-%02u-%02u %02u:%02u:%02u:%04u\n",
		(unsigned int) ts.d.day,
		(unsigned int) ts.d.month,
		(unsigned int) ts.d.year,
		(unsigned int) ts.t.hours,
		(unsigned int) ts.t.minutes,
		(unsigned int) ts.t.seconds,
		(unsigned int) ts.ms
	);
}

int stm32_rtc_initialize(struct device *dev)
{
	//(void) dev;
    dev = DEVICE_DT_GET(TIMER);

	static LL_RTC_InitTypeDef const initData = {
		.HourFormat = LL_RTC_HOURFORMAT_24HOUR,
		// prescaler values for LSE @ 32768 Hz
		.AsynchPrescaler = RTC_PREDIV_ASYNC,
		.SynchPrescaler = RTC_PREDIV_SYNC
	};

	static rtc_ts_t const startTime = {
		.d = {
			.day = 1,
			.weekday = LL_RTC_WEEKDAY_MONDAY,
			.month = LL_RTC_MONTH_JANUARY,
			.year = 0,
		},
		.t = {
			.hours = 9,
			.minutes = 45,
			.seconds = 13
		},
		.ms = 0
	};

	LL_PWR_EnableBkUpAccess();
	LL_RCC_ForceBackupDomainReset();
	LL_RCC_ReleaseBackupDomainReset();

	LL_RCC_LSE_Enable();

	/* Wait untill LSE is ready */
	while (LL_RCC_LSE_IsReady() != 1) {};

	LL_RCC_SetRTCClockSource(LL_RCC_RTC_CLKSOURCE_LSE);

	LL_RCC_EnableRTC();

	if (LL_RTC_DeInit(RTC) != SUCCESS) {
		return -EIO;
	}

	if (LL_RTC_Init(RTC, (LL_RTC_InitTypeDef *)&initData) != SUCCESS) {
		return -EIO;
	}

	LL_RTC_EnableShadowRegBypass(RTC);

	stm32_rtc_setTime(startTime);

	printk("RTC initialised\n");
	return 0;
}

void stm32_rtc_getTimestamp(rtc_raw_ts_t * ts)
{
	// get sub section
	ts->ss = (uint16_t) LL_RTC_TIME_GetSubSecond(RTC);
	// 0x00HHMMSS in bcd format
	ts->t = LL_RTC_TIME_Get(RTC);
	// 0xWWDDMMYY
	ts->d = LL_RTC_DATE_Get(RTC);

	printk("ss: %d, t:%d, d: %d\n", ts->ss, ts->t, ts->d);
}
