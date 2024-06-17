/*
 * Copyright (c) 2023
 * Regis Rousseau
 * Univ Lyon, INSA Lyon, Inria, CITI, EA3720
 * SPDX-License-Identifier: Apache-2.0
 */
 
#ifndef APP_ROM_H_
#define APP_ROM_H_

//  ======== includes =============================================
#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/flash.h>

//  ======== defines ==============================================
#define ROM_OFFSET              0x00
#define ROM_PAGE_SIZE           512
#define ROM_BUFFER_SIZE         14   // 1 structure: ID+timestamp+value = 14 bytes
#define ROM_MAX_RECORDS         36   // number of structures to store per page
#define LORAWAN_DEV_EUI		    { 0x70, 0xB3, 0xD5, 0x7E, 0xD0, 0x06, 0x21, 0xA5 } 
#define THRESHOLD               2048    // for now, VDD/2  
                                        
//  ======== prototypes ===========================================
int8_t app_rom_init(const struct device *dev);
int8_t app_rom_write(const struct device *dev, void *data);
int8_t app_rom_read(const struct device *dev);
int8_t app_rom_handler(const struct device *dev);


#endif /* APP_ROM_H_ */