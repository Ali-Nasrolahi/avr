#pragma once

#include "common.h"
#include "hal.h"

#define SD_LOG_PREFIX "SD Card Driver: "
#define SD_DUMMY_BYTE (0xff)
#define SD_ACMD_FAIL  (0x39)

void sd_init(void);

bool sd_read_sector(uint32_t lba, void* buf);
bool sd_write_sector(uint32_t lba, void* buf);