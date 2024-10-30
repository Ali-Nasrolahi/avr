#include "crc7.h"

static uint8_t crc_table[256];

static void gen_crc_table()
{
    uint8_t CRCPoly = 0x89;  // the value of our CRC-7 polynomial

    // generate a table value for all 256 possible byte values
    for (uint16_t i = 0; i < 256; ++i) {
        crc_table[i] = (i & 0x80) ? i ^ CRCPoly : i;
        for (uint8_t j = 1; j < 8; ++j) {
            crc_table[i] <<= 1;
            if (crc_table[i] & 0x80) crc_table[i] ^= CRCPoly;
        }
    }
}

static uint8_t _crc7(uint8_t crc, uint8_t data) { return crc_table[(crc << 1) ^ data]; }

void crc7_init(void) { gen_crc_table(); }

uint8_t crc7_cal(uint8_t *data, size_t size)
{
    uint8_t CRC = 0;

    for (uint8_t i = 0; i < size; ++i) CRC = _crc7(CRC, data[i]);

    return CRC;
}