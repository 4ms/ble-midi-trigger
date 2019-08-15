#pragma once
#include <stdio.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"

uint32_t adc_read(uint8_t chan);
void adc_setup(void);
