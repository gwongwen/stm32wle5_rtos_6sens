/*
 * Copyright (c) 2023
 * Regis Rousseau
 * Univ Lyon, INSA Lyon, Inria, CITI, EA3720
 * SPDX-License-Identifier: Apache-2.0
 */

#include "app_adc.h"

//  ======== globals ============================================
uint16_t sp_buf;		

static const struct adc_dt_spec adc_channels[] = {
	DT_FOREACH_PROP_ELEM(DT_PATH(zephyr_user), io_channels, DT_SPEC_AND_COMMA)
};

struct adc_sequence adc_ch13_seq = {
	.buffer 		= &sp_buf,
	.buffer_size	= sizeof(sp_buf),
};

//  ======== app_adc_handler ====================================
uint16_t app_adc_get_val(void)
{
	int8_t err;

	// getting STM32 ADC device at GPIO PB13
	for (size_t i = 0U; i < ARRAY_SIZE(adc_channels); i++) {
		if (!device_is_ready(adc_channels[i].dev)) {
			printk("ADC controller device %s not ready\n", adc_channels[i].dev->name);
		return 0;
		}
		// getting channel setup
		err = adc_channel_setup_dt(&adc_channels[i]);
		if (err < 0) {
			printk("could not setup channel 13. error: %d\n", err);
			return 0;
		}
	}

	// initialization of channel and getting data
	// resolution 12 bits: 0 to 4095 (uint16)
	for (size_t i = 0U; i < ARRAY_SIZE(adc_channels); i++) {

		printk("- %s, channel %d: ",
			       adc_channels[i].dev->name,
			       adc_channels[i].channel_id);

		(void)adc_sequence_init_dt(&adc_channels[i], &adc_ch13_seq);
		err = adc_read(adc_channels[i].dev, &adc_ch13_seq);
		printk("adc: %d\n", sp_buf);
	}
	return sp_buf;
}