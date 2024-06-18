/*
 * Copyright (c) 2023
 * Regis Rousseau
 * Univ Lyon, INSA Lyon, Inria, CITI, EA3720
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef APP_LORAWAN__H
#define APP_LORAWAN__H

#include <zephyr/device.h>
#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/lorawan/lorawan.h>
#include <zephyr/posix/time.h>

//  ======== defines ===============================================
#define LED_TX                  DT_ALIAS(led0)
#define LED_RX                  DT_ALIAS(led1)
#define DELAY 			        K_MSEC(5000)

// OTAA
#define LORAWAN_DEV_EUI			{ 0x70, 0xB3, 0xD5, 0x7E, 0xD0, 0x06, 0x21, 0xA5 }
#define LORAWAN_JOIN_EUI		{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }
#define LORAWAN_APP_KEY			{ 0xC7, 0x32, 0x0F, 0x37, 0xFF, 0x62, 0xE0, 0xA8, 0x4E, 0x94, 0xC1, 0x9C, 0x27, 0x2B, 0xFA, 0x4C }

//  ======== prototypes ============================================
int8_t app_lorawan_init(const struct device *dev);
int8_t app_lorawan_handler(const struct device *dev, uint16_t *data_tx);
time_t app_lorawan_get_time(const struct device *dev);

#endif /* APP_LORAWAN__H */