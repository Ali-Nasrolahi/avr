#include "sd.h"

static uint8_t sd_send_cmd(uint8_t cmd, uint32_t args, uint8_t crc7)
{
    uint8_t resp;

    SET_BIT(SPI_DDR, SPI_SS);     // As an ouput
    UNSET_BIT(SPI_PORT, SPI_SS);  // CS/SS Low/Enable

    spi_tx_rx(0x40 | (cmd & 0x3f));
    spi_tx_rx((uint8_t)(args >> 24));
    spi_tx_rx((uint8_t)(args >> 16));
    spi_tx_rx((uint8_t)(args >> 8));
    spi_tx_rx((uint8_t)(args));
    spi_tx_rx((crc7 << 1) | 1);

    while ((resp = spi_tx_rx(SD_DUMMY_BYTE)) == SD_DUMMY_BYTE);

    SET_BIT(SPI_PORT, SPI_SS);  // CS/SS High/disable

    return resp;
}

static uint32_t sd_recv_r7(void)
{
    uint32_t r7resp = 0;
    SET_BIT(SPI_DDR, SPI_SS);     // As an ouput
    UNSET_BIT(SPI_PORT, SPI_SS);  // CS/SS Low/Enable

    r7resp |= (((uint32_t)spi_tx_rx(SD_DUMMY_BYTE)) << 24);
    r7resp |= (((uint32_t)spi_tx_rx(SD_DUMMY_BYTE)) << 16);
    r7resp |= (spi_tx_rx(SD_DUMMY_BYTE) << 8);
    r7resp |= (spi_tx_rx(SD_DUMMY_BYTE));

    SET_BIT(SPI_PORT, SPI_SS);  // CS/SS High/disable

    return r7resp;
}

void sd_init(void)
{
    _delay_ms(10);
    spi_init_master(SPI_PRESCALER_4);

    SET_BIT(SPI_DDR, SPI_SS);   // As an ouput
    SET_BIT(SPI_PORT, SPI_SS);  // set high/disable

    // 1. At least 74 dummy clocks
    for (uint8_t i = 0; i < 10; ++i) spi_tx_rx(SD_DUMMY_BYTE);

    // 2. Software reset (CMD0)
    uint8_t resp = sd_send_cmd(0, 0, 0x4a);
    resp == 0x1 ? printf(SD_LOG_PREFIX "Software reset OK!\n")
                : printf(SD_LOG_PREFIX "Software reset failed: 0x%x\n", resp);

    // 3. Initialization (CMD8)
    resp = sd_send_cmd(8, 0x01aa, 0x43);
    if (resp == 0x1 && (sd_recv_r7() == 0x01aa)) {
        printf(SD_LOG_PREFIX "Initialization OK!\n");
    } else printf(SD_LOG_PREFIX "Initialization failed: 0x%x\n", resp);
}