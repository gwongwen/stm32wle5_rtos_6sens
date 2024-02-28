/*
 * Copyright (c) 2023
 * Regis Rousseau
 * Univ Lyon, INSA Lyon, Inria, CITI, EA3720
 * SPDX-License-Identifier: Apache-2.0
 */

 #include "app_lorawan.h"
 #include "app_flash.h"

//  ======== globals ============================================
// downlink callback
static void dl_callback(uint8_t port, bool data_pending, int16_t rssi, int8_t snr, uint8_t len, const uint8_t *data)
{
	printk("downlink data received: ");
    for(int8_t i=0; i < len; i++) {
		printk("%02X ", data[i]);
	}
    printk("\n");
	printk("port: %d, pending: %d, RSSI: %ddB, SNR: %dBm\n", port, data_pending, rssi, snr);
}

// adr change callback
static void lorwan_datarate_changed(enum lorawan_datarate dr)
{
	uint8_t unused, max_size;

	lorawan_get_payload_sizes(&unused, &max_size);
	printk("new datarate: DR_%d, max payload: %d\n", dr, max_size);
}

static const struct gpio_dt_spec led_tx = GPIO_DT_SPEC_GET(LED_TX, gpios);
static const struct gpio_dt_spec led_rx = GPIO_DT_SPEC_GET(LED_RX, gpios);

//  ======== app_loarwan_init ===================================
int8_t app_lorawan_init(const struct device *dev)
{
    struct lorawan_join_config join_cfg;

    int8_t ret;
    uint8_t dev_addr[] = LORAWAN_DEV_ADDR;
    uint8_t nwk_skey[] = LORAWAN_NWK_SKEY;
    uint8_t app_skey[] = LORAWAN_APP_SKEY;
    uint8_t app_eui[]  = LORAWAN_APP_EUI;

    // setup tx and rx led at 
    ret = gpio_pin_configure_dt(&led_tx, GPIO_OUTPUT_ACTIVE);
	if (ret < 0) {
		return 0;
	}
    ret = gpio_pin_configure_dt(&led_rx, GPIO_OUTPUT_ACTIVE);
	if (ret < 0) {
		return 0;
	}

    printk("starting lorawan node\n");
    // getting lora sx1276 device
	dev = DEVICE_DT_GET(DT_ALIAS(lora0));
	if (!device_is_ready(dev)) {
		printk("%s: device not ready\n", dev->name);
		return 0;
	}

	printk("starting lorawan stack\n");
    // starting device
	ret = lorawan_start();
	if (ret < 0) {
		printk("lorawan_start failed. error: %d\n", ret);
		return 0;
	}

     // enable adr
    lorawan_enable_adr(true);

	// enable callbacks
	struct lorawan_downlink_cb downlink_cb = {
		.port = LW_RECV_PORT_ANY,
		.cb = dl_callback
	};
	lorawan_register_downlink_callback(&downlink_cb);
	lorawan_register_dr_changed_callback(lorwan_datarate_changed);  

// configuration of lorawan parameters 
    join_cfg.mode = LORAWAN_ACT_ABP;
    join_cfg.dev_eui = dev_addr;
    join_cfg.abp.dev_addr = dev_addr;
    join_cfg.abp.app_skey = app_skey;
    join_cfg.abp.nwk_skey = nwk_skey;
    join_cfg.abp.app_eui  = app_eui;

	do {
        // setup of lorawan parameters 
		ret = lorawan_join(&join_cfg);

        // flashing of the LED when a packet is received
        ret = gpio_pin_toggle_dt(&led_rx);
			if (ret < 0) {
				return 0;
			}

		if (ret < 0) {
			printk("join network failed. error: %d\n", ret);
			k_sleep(DELAY);
		} else {
			printk("join successful\n");
		}
	} while (ret != 0)	;
    return 0;
}

//  ======== app_loarwan_get_time ===============================
time_t app_lorawan_get_time(const struct device *dev)
{
    int8_t ret;
    uint32_t gps_time;
	time_t unix_time;
	struct tm timeinfo;
	char buf[32];

    // getting lora sx1276 device
    dev = DEVICE_DT_GET(DT_ALIAS(lora0));
	if (!device_is_ready(dev)) {
		printk("%s: device not ready\n", dev->name);
		return 0;
	}
	
    // getting timstamp from lorawan
    lorawan_clock_sync_run();
    ret = lorawan_clock_sync_get(&gps_time);
	if (ret != 0) { 
			printk("lorawan_clock_sync_get failed. error: %d\n", ret);
		} else {
			/* 
			 * The difference in time between UNIX (epoch Jan 1st 1970) and
			 * GPS (epoch Jan 6th 1980) is 315964800 seconds. This is a bit
			 * of a fudge as it doesn't take into account leap seconds and 
			 * hence is out by roughly 18 seconds. 
			 *
			 */
			unix_time = gps_time - 315964800;
			localtime_r(&unix_time, &timeinfo);
			strftime(buf, sizeof(buf), "%A %B %d %Y %I:%M:%S %p %Z", &timeinfo);
			printk("GPS Time: %lu, UTC Time: %s", gps_time, buf);
		}
    return unix_time;
}

//  ======== app_loarwan_handler ================================
int8_t app_lorawan_handler(const struct device *dev, uint16_t *data_tx)
{   
    int8_t ret;

    dev = DEVICE_DT_GET(DT_ALIAS(lora0));

    printk("sending data...\n");
    // transmission of packets on lorawan protocole
	for (int8_t itr = 0; itr < 10 ; itr++) {
		ret = lorawan_send(2, data_tx, sizeof(data_tx), LORAWAN_MSG_CONFIRMED);

		if (ret == -EAGAIN) {
			printk("lorawan_send failed: %d. continuing...\n", ret);
			k_sleep(DELAY);
			continue;
		} else if (ret < 0) {
			printk("lorawan_send failed: %d. continuing...\n", ret);
			k_sleep(DELAY);
			return 0;;
		}

        // flashing of the LED when a packet is transmitted
		ret = gpio_pin_toggle_dt(&led_tx);
			if (ret < 0) {
				return 0;
			}
		printk("data sent !\n");
		k_sleep(DELAY);
	}
	return 0;
}


