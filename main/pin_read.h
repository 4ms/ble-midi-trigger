#pragma once
#include <stdio.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"

void start_pin_read(gpio_num_t pin, gpio_int_type_t edge, gpio_isr_t callback);

// class dig_pin_input {
// public:
// 	dig_pin_input(gpio_num_t pin, gpio_int_type_t edge);
// 	void start();
// 	static void IRAM_ATTR gpio_isr_handler(void* arg);

// 	gpio_num_t 	gpio_num_;
// 	xQueueHandle gpio_evt_queue;
// };