#include <stdlib.h>

#include "avrlib/hal.h"
#include "sd/sd.h"

void spi_recv(uint8_t data) { PORTD = (data & 0xf) << 2; }

void test_spi_slave(void)
{
    DDRD = _BV(PIND5) | _BV(PIND4) | _BV(PIND3) | _BV(PIND2);
    spi_init_slave(spi_recv);
    while (1) { _delay_ms(1000); }
}

void test_spi_master(void)
{
    spi_init_master(SPI_PRESCALER_64);
    int x = 0;
    while (1) {
        ++x;
        x &= 7;
        spi_tx_rx(x);
        _delay_ms(500);
    }
}

void test_i2c_master(void)
{
    i2c_init_master();

    int x = 0;
    while (1) {
        ++x;
        x &= 7;
        i2c_send(0x10, x);
        PORTD = (i2c_recv(0x10) & 0xf) << 2;
        _delay_ms(500);
    }
}

static void i2c_receiver(uint8_t data) { PORTD = (data & 0xf) << 2; }
static uint8_t i2c_request(void)
{
    static uint8_t data = 0;
    ++data;
    data &= 7;
    return data;
}

void test_i2c_slave(void)
{
    i2c_init_slave(0x10, i2c_receiver, i2c_request);
    while (1) { _delay_ms(1000); }
}

int main(void)
{
    static char buf_[350];
    usart_enable_stdio(9600);
    printf("stdio is enabled!\n");
    sd_init();

    if (sd_read_sector(0, 512 - 250, buf_)) printf("\n\nREAD Ok\n\n");
    else printf("READ FAILED\n");
    for (size_t i = 0; i < 250; i++) printf("0x%x ", buf_[i]);

    while (1) { _delay_ms(1000); }
}
