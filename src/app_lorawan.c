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
	static struct nvs_fs fs;
	uint16_t dev_nonce = 0u;

    int8_t ret;
	int8_t itr = 1;
	ssize_t err;
	uint8_t dev_eui[] 	= LORAWAN_DEV_EUI;
	uint8_t join_eui[]	= LORAWAN_JOIN_EUI;
	uint8_t app_key[]	= LORAWAN_APP_KEY;

    // setup tx led at GPIO PC0
    ret = gpio_pin_configure_dt(&led_tx, GPIO_OUTPUT_ACTIVE);
	if (ret < 0) {
		return 0;
	}
	// setup rx led at GPIO PC1
    ret = gpio_pin_configure_dt(&led_rx, GPIO_OUTPUT_ACTIVE);
	if (ret < 0) {
		return 0;
	}

	// initialization and reading/writing the devnonce parameter
	app_flash_init(&fs);
	app_flash_init_param(&fs, NVS_DEVNONCE_ID, &dev_nonce);

	 printk("starting lorawan node\n");
    // getting lora sx1276 device
	dev = DEVICE_DT_GET(DT_ALIAS(lora0));
	if (!device_is_ready(dev)) {
		printk("%s: device not ready\n", dev->name);
		return 0;
	}

	printk("starting lorawan stack\n");
    // starting device
	ret = lorawan_set_region(LORAWAN_REGION_EU868);
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
    join_cfg.mode = LORAWAN_ACT_OTAA;
	join_cfg.dev_eui = dev_eui;
	join_cfg.otaa.join_eui = join_eui;
	join_cfg.otaa.app_key = app_key;
	join_cfg.otaa.nwk_key = app_key;
	join_cfg.otaa.dev_nonce = dev_nonce;

	do {
		printk("joining network using OTAA, dev nonce %d, attempt %d\n", join_cfg.otaa.dev_nonce, itr++);
		ret = lorawan_join(&join_cfg);

		// flashing of the LED when a packet is received
		ret = gpio_pin_toggle_dt(&led_rx);
			if (ret < 0) {
				return 0;
			}

		if (ret < 0) {
			if ((ret =-ETIMEDOUT)) {
				printk("timed-out waiting for response.\n");
			} else {
				printk("join network failed. error: %d\n", ret);
			}
		} else {
			printk("OTAA join successful\n");
		}

		dev_nonce++;
		join_cfg.otaa.dev_nonce = dev_nonce;

		// save value away in Non-Volatile Storage.
		err = nvs_write(&fs, NVS_DEVNONCE_ID, &dev_nonce, sizeof(dev_nonce));
		if (err < 0) {
			printk("NVS: failed to write id %d. error: %d\n", NVS_DEVNONCE_ID, err);
		}

		if (ret < 0) {
			// if failed, wait before re-trying.
			k_sleep(DELAY);
		}
	} while (ret != 0);
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

    // transmission of packets on lorawan protocole
	printk("sending data...\n");
	ret = lorawan_send(2, data_tx, sizeof(data_tx), LORAWAN_MSG_CONFIRMED);
	if (ret == -EAGAIN) {
			printk("lorawan_send failed: %d. continuing...\n", ret);
			k_sleep(DELAY);
			return 0;
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
	//k_sleep(DELAY);
	return 0;
}


