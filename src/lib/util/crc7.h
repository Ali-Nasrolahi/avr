#pragma once

#include "common.h"

void crc7_init(void);
uint8_t crc7_cal(uint8_t *data, size_t size);