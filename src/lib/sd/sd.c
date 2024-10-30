#include "sd.h"

#include "util/crc7.h"

union {
    struct {
        uint32_t voltage_range : 28;
        uint8_t uhs_ii_card_status : 1;
        uint8_t ccs : 1;  /* Card Capacity Status */
        uint8_t busy : 1; /* Card power up status bit */
    } __attribute__((packed));
    uint32_t raw;  // 32-bit overlay for direct access
} sd_ocr;

union {
    struct {
        uint32_t manufacture_data;
        uint8_t cmd_support : 4;
        uint8_t reserved : 2;
        uint8_t sd_specx : 4;
        uint8_t sd_spec4 : 1;
        uint8_t ex_security : 4;
        uint8_t sd_spec3 : 1;
        uint8_t bus_width : 4;
        uint8_t sd_security : 3;
        uint8_t data_stat_after_erase : 1;
        uint8_t sd_spec : 4;
        uint8_t scr_struct : 4;

    } __attribute__((packed));
    uint64_t raw;
} sd_scr;

static uint8_t sd_send_cmd(uint8_t cmd, uint32_t args)
{
    uint8_t resp, cmd_buf[5];

    cmd_buf[0] = (0x40 | (cmd & 0x3f));
    cmd_buf[1] = (uint8_t)(args >> 24);
    cmd_buf[2] = (uint8_t)(args >> 16);
    cmd_buf[3] = (uint8_t)(args >> 8);
    cmd_buf[4] = (uint8_t)(args);

    SET_BIT(SPI_DDR, SPI_SS);     // As an ouput
    UNSET_BIT(SPI_PORT, SPI_SS);  // CS/SS Low/Enable

    spi_tx_rx(cmd_buf[0]);
    spi_tx_rx(cmd_buf[1]);
    spi_tx_rx(cmd_buf[2]);
    spi_tx_rx(cmd_buf[3]);
    spi_tx_rx(cmd_buf[4]);
    spi_tx_rx((crc7_cal(cmd_buf, 5) << 1) | 1);

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

static inline uint32_t sd_read_ocr(void) { return sd_send_cmd(58, 0) < 0x2 ? sd_recv_r7() : 0; }

void sd_init(void)
{
    crc7_init();

    _delay_ms(10);
    spi_init_master(SPI_PRESCALER_128);

    SET_BIT(SPI_DDR, SPI_SS);   // As an ouput
    SET_BIT(SPI_PORT, SPI_SS);  // set high/disable

    // 1. At least 74 dummy clocks
    for (uint8_t i = 0; i < 10; ++i) spi_tx_rx(SD_DUMMY_BYTE);

    // 2. Software reset (CMD0)
    uint32_t resp = sd_send_cmd(0, 0);
    resp == 0x1 ? printf(SD_LOG_PREFIX "Software reset OK!\n")
                : printf(SD_LOG_PREFIX "Software reset failed: 0x%lx\n", resp);

    // 3. Initialization (CMD8)
    resp = sd_send_cmd(8, 0x01aa);
    if (resp == 0x1 && (sd_recv_r7() == 0x01aa)) {
        printf(SD_LOG_PREFIX "Initialization OK!\n");
    } else printf(SD_LOG_PREFIX "Initialization failed: 0x%lx\n", resp);

    // 4. Supported Voltage range
    resp = sd_read_ocr();
    if (resp && ((resp & 0x00380000) == 0x00380000)) {
        printf(SD_LOG_PREFIX "Supported voltage range OK!\n");
    } else printf(SD_LOG_PREFIX "Supported voltage range failed: 0x%lx\n", resp);

    do {
        resp = sd_send_cmd(55, 0);
        if (resp == 0x1 || resp == 0) {
            printf(SD_LOG_PREFIX "CMD55 OK!\n");
            resp = sd_send_cmd(41, 0x40000000);
            if (resp == 0x0) printf(SD_LOG_PREFIX "CMD41 OK!\n");
            else printf(SD_LOG_PREFIX "CMD41 Fail: 0x%lx\n", resp);
        } else {
            printf(SD_LOG_PREFIX "CMD55 Fail: 0x%lx\n", resp);
            return;  // FAIL
        }
    } while (resp);

    printf("Init succeed!\nRetrieve Register values OCR/SCR/CID/CSD\n");

    sd_ocr.raw = sd_read_ocr();
    if (sd_ocr.raw) printf(SD_LOG_PREFIX "OCR:\tOK!\tValue: 0x%lx\n", sd_ocr.raw);
    else printf(SD_LOG_PREFIX "OCR: failed 0x%lx\n", sd_ocr.raw);

    if (sd_ocr.busy) printf(SD_LOG_PREFIX "OCR:\tPower Up completed!\n");
    if (sd_ocr.ccs) printf(SD_LOG_PREFIX "OCR:\tHight capacity card detected (SDHC/SDXC)!\n");

    // Retrieve register values SCR / CID / CSD
}
