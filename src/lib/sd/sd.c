#include "sd.h"

void sd_init(void) { spi_init_master(SPI_PRESCALER_4); }