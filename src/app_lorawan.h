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

// ABP : Activation by Personlization
#define LORAWAN_DEV_ADDR        { 0x26, 0x0B, 0x21, 0xC4 }
#define LORAWAN_NWK_SKEY        { 0x68, 0x72, 0x40, 0x44, 0x12, 0x0C, 0x11, 0x65, 0x2F, 0x50, 0x19, 0x9B, 0x50, 0x48, 0xCB, 0xC1 }
#define LORAWAN_APP_SKEY        { 0x32, 0x9D, 0xE3, 0xB7, 0xA8, 0x34, 0x95, 0x7E, 0x7C, 0x48, 0xB6, 0x38, 0x60, 0x85, 0x58, 0xEC }
#define LORAWAN_APP_EUI         { 0x70, 0xB3, 0xD5, 0x7E, 0xD0, 0x06, 0x30, 0xAB }

//  ======== prototypes ============================================
int8_t app_lorawan_init(const struct device *dev);
int8_t app_lorawan_handler(const struct device *dev, uint16_t *data_tx);
time_t app_lorawan_get_time(const struct device *dev);

#endif /* APP_LORAWAN__H */