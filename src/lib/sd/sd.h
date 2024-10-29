#pragma once

#include "common.h"
#include "hal.h"

#define SD_LOG_PREFIX "SD Card Driver: "
#define SD_DUMMY_BYTE (0xff)

void sd_init(void);