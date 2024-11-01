#include "sd.h"

#include "sd-type.h"
#include "util/crc7.h"

static inline void sd_delay(void)
{
    SPI_DISABLE_SS;
    for (uint8_t i = 0; i < 10; ++i) spi_tx_rx(SD_DUMMY_BYTE);
}

static inline uint8_t sd_wait_for_valid_resp(void)
{
    uint8_t resp;
    SPI_ENABLE_SS;
    while ((resp = spi_tx_rx(SD_DUMMY_BYTE)) == 0xff || resp == 0xfe /* data block */);
    return resp;
}

static uint8_t sd_send_cmd(uint8_t cmd, uint32_t args)
{
    uint8_t resp, cmd_buf[5];

    cmd_buf[0] = (0x40 | (cmd & 0x3f));
    cmd_buf[1] = (uint8_t)(args >> 24);
    cmd_buf[2] = (uint8_t)(args >> 16);
    cmd_buf[3] = (uint8_t)(args >> 8);
    cmd_buf[4] = (uint8_t)(args);

    SPI_ENABLE_SS;

    spi_tx_rx(cmd_buf[0]);
    spi_tx_rx(cmd_buf[1]);
    spi_tx_rx(cmd_buf[2]);
    spi_tx_rx(cmd_buf[3]);
    spi_tx_rx(cmd_buf[4]);
    spi_tx_rx((crc7_cal(cmd_buf, 5) << 1) | 1);

    while ((resp = spi_tx_rx(SD_DUMMY_BYTE)) == SD_DUMMY_BYTE);

    SPI_DISABLE_SS;

    return resp;
}

static uint8_t sd_send_acmd(uint8_t acmd, uint32_t args)
{
    uint32_t resp = SD_ACMD_FAIL;
    do {
        resp = sd_send_cmd(55, 0);
        if (resp < 2) {
            printf(SD_LOG_PREFIX "CMD55 OK!\n");
            resp = sd_send_cmd(acmd, args);
            if (resp == 0x0) printf(SD_LOG_PREFIX "ACDM%d OK!\n", acmd);
            else printf(SD_LOG_PREFIX "ACMD%d Fail: 0x%lx\n", acmd, resp);
        } else {
            printf(SD_LOG_PREFIX "CMD55 Fail: 0x%lx\n", resp);
            return SD_ACMD_FAIL;
        }
    } while (resp);

    return 0;
}

static uint32_t sd_recv_32(void)
{
    uint32_t resp = (uint32_t)sd_wait_for_valid_resp() << 24;

    resp |= (((uint32_t)spi_tx_rx(SD_DUMMY_BYTE)) << 16);
    resp |= (spi_tx_rx(SD_DUMMY_BYTE) << 8);
    resp |= (spi_tx_rx(SD_DUMMY_BYTE));

    SPI_DISABLE_SS;

    return resp;
}

static uint64_t sd_recv_64(void)
{
    uint64_t resp = (uint64_t)sd_wait_for_valid_resp() << 56;

    resp |= ((uint64_t)spi_tx_rx(SD_DUMMY_BYTE) << 48);
    resp |= ((uint64_t)spi_tx_rx(SD_DUMMY_BYTE) << 40);
    resp |= ((uint64_t)spi_tx_rx(SD_DUMMY_BYTE) << 32);
    resp |= ((uint64_t)spi_tx_rx(SD_DUMMY_BYTE) << 24);
    resp |= ((uint64_t)spi_tx_rx(SD_DUMMY_BYTE) << 16);
    resp |= ((uint64_t)spi_tx_rx(SD_DUMMY_BYTE) << 8);
    resp |= ((uint64_t)spi_tx_rx(SD_DUMMY_BYTE));

    SPI_DISABLE_SS;

    return resp;
}

static inline uint32_t sd_read_ocr(void) { return sd_send_cmd(58, 0) < 2 ? sd_recv_32() : 0; }

static inline uint64_t sd_read_scr(void) { return sd_send_acmd(51, 0) < 2 ? sd_recv_64() : 0; }

static int8_t _sd_init(void)
{
    uint32_t resp;

    _delay_ms(10);
    crc7_init();
    spi_init_master(SPI_PRESCALER_128);

    // 1. At least 74 dummy clocks
    sd_delay();

    // 2. Software reset (CMD0)
    if ((resp = sd_send_cmd(0, 0)) == 1) printf(SD_LOG_PREFIX "Software reset OK!\n");
    else return printf(SD_LOG_PREFIX "Software reset failed: 0x%lx\n", resp), -1;

    // 3. Initialization (CMD8)
    resp = sd_send_cmd(8, 0x01aa);
    if (resp == 0x1 && (sd_recv_32() == 0x01aa)) printf(SD_LOG_PREFIX "Initialization OK!\n");
    else return printf(SD_LOG_PREFIX "Initialization failed: 0x%lx\n", resp), -1;

    // 4. Supported Voltage range
    resp = sd_read_ocr();
    printf("ocr %lx\n", resp);
    if (resp && ((resp & 0x00380000) == 0x00380000))
        printf(SD_LOG_PREFIX "Supported voltage range OK!\n");
    else return printf(SD_LOG_PREFIX "Supported voltage range failed: 0x%lx\n", resp), -1;

    if (sd_send_acmd(41, 0x40000000) != SD_ACMD_FAIL)
        printf(SD_LOG_PREFIX "SD Card is ready to operate!\n");
    else return printf(SD_LOG_PREFIX "SD Card cannot become ready!\n"), -1;

    return 0;
}

void sd_init(void)
{
    if (_sd_init()) return;

    printf("Init succeed!\nRetrieve Register values OCR/SCR/CID/CSD\n");

    if ((sd_ocr.raw = sd_read_ocr())) printf(SD_LOG_PREFIX "OCR:\tOK!\tValue: 0x%lx\n", sd_ocr.raw);
    else printf(SD_LOG_PREFIX "OCR: failed 0x%lx\n", sd_ocr.raw);

    if (sd_ocr.busy) printf(SD_LOG_PREFIX "OCR:\tPower Up completed!\n");
    if (sd_ocr.ccs) printf(SD_LOG_PREFIX "OCR:\tHight capacity card detected (SDHC/SDXC)!\n");

    if ((sd_scr.raw = sd_read_scr())) {
        printf(SD_LOG_PREFIX "SCR: OK! High 4 bytes: 0x%lx Low 4 bytes: 0x%lx\n",
               (uint32_t)(sd_scr.raw >> 32), (uint32_t)(sd_scr.raw));
    } else printf(SD_LOG_PREFIX "SCR: failed 0x%lx\n", (uint32_t)(sd_scr.raw));

    // Retrieve register values CID / CSD
}
