extern "C" {
	void app_main(void);
}
#include <esp_log.h>

#include "adc_read.h"
#include "pin_read.h"
#include "ble_midi_server.h"

xQueueHandle trig_queue;

// void trigger_debug_monitor_task(void* arg)
// {
//     gpio_num_t io_num;
//     for(;;) {
//         if(xQueueReceive(trig_queue, &io_num, portMAX_DELAY)) {
//             printf("GPIO[%d] intr, val: %d\n", io_num, gpio_get_level(io_num));
//         }
//     }
// }
static void IRAM_ATTR trig_isr_handler(void* arg)
{
	uint32_t gpio_num = (uint32_t) arg;
	// ets_printf("trig_isr_handler added to trig_queue gpio_num=%d\n", (uint32_t)gpio_num);
	xQueueSendFromISR(trig_queue, &gpio_num, NULL);
}

void app_main(void) {

	trig_queue = xQueueCreate(1000, sizeof(uint32_t));
	printf("trig_queue addr = 0x%08x\n", (uint32_t)(&trig_queue));

	start_ble_midi_server(&trig_queue);
	start_pin_read(GPIO_NUM_4, GPIO_INTR_POSEDGE, (gpio_isr_t)trig_isr_handler);
	// xTaskCreate(trigger_debug_monitor_task, "trigger_debug_monitor_task", 2048, NULL, 10, NULL);

} // app_main
