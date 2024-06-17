#include <zephyr/device.h>
#include <zephyr/kernel.h>
#include "app_lorawan.h"
#include "app_flash.h"

//  ======== globals ============================================
uint16_t cnt;           // declaration of counter index

//  ======== interrupt sub-routine ===============================
void adc_work_handler(struct k_work *work_geo)
{
	const struct device *dev;
	app_rom_handler(dev);
}
K_WORK_DEFINE(geo_work, adc_work_handler);

void adc_timer_handler(struct k_timer *timer)
{
	k_work_submit(&geo_work);
}
K_TIMER_DEFINE(adc_timer, adc_timer_handler, NULL);

//  ======== main ===============================================
int8_t main(void)
{
	const struct device *dev;
	static struct nvs_fs fs;
	uint16_t vbat;
	cnt = 0;
	uint32_t max_cnt = 0;
	int8_t ret;
	
	// initialization of all devices
	app_bme280_init(dev);
	app_stm32_vbat_init(dev);
	app_rom_init(dev);
	app_flash_init(dev);

	// initialization of LoRaWAN
	app_lorawan_init(dev);

	printk("Measurement\nBoard: %s\n", CONFIG_BOARD);
	
	// beginning of isr timer
	k_timer_start(&adc_timer, K_SECONDS(5), K_SECONDS(5));	// for test

	while (1) {
		// counter value for waiting 10 min -> 600000ms
		if (cnt >= 600000) {
			app_flash_handler(&fs);
		}
	}	
	return 0;
}