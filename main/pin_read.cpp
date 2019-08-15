#include "pin_read.h"

#define ESP_INTR_FLAG_DEFAULT 0

void start_pin_read(gpio_num_t pin, gpio_int_type_t edge, gpio_isr_t callback)
{
	gpio_config_t io_conf;

	io_conf.intr_type = edge;
	io_conf.mode = GPIO_MODE_INPUT;
	io_conf.pin_bit_mask = (1ULL<<pin);
	io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
	io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
	gpio_config(&io_conf);

	gpio_install_isr_service(ESP_INTR_FLAG_DEFAULT);
	gpio_isr_handler_add(pin, callback, (void*) pin);
}

//C++ style below (problems with static methods):
// void IRAM_ATTR dig_pin_input::gpio_isr_handler(void* arg)
// {
// 	uint32_t gpio_num = (uint32_t) arg;
// 	xQueueSendFromISR(gpio_evt_queue, &gpio_num, NULL);
// }

// dig_pin_input::dig_pin_input(gpio_num_t pin, gpio_int_type_t edge)
// {
// 	gpio_num_ = pin;

//     gpio_config_t io_conf;

//     io_conf.intr_type = edge;
//     io_conf.mode = GPIO_MODE_INPUT;
//     io_conf.pin_bit_mask = (1ULL<<pin);
//     io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
//     io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
//     gpio_config(&io_conf);
// }

// void dig_pin_input::start()
// {
//     gpio_evt_queue = xQueueCreate(10, sizeof(uint32_t));
//     gpio_install_isr_service(ESP_INTR_FLAG_DEFAULT);
//     gpio_isr_handler_add(gpio_num_, gpio_isr_handler, (void*) gpio_num_);
// }

