#include <avr/interrupt.h>
#include <avr/io.h>
#include <stddef.h>
#include <util/delay.h>

#include "lib/spi.h"

void spi_recv(uint8_t data)
{
    data = (data & 0xf) << 2;
    PORTD = data;
}

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

int main(void)
{
    DDRD = _BV(PIND5) | _BV(PIND4) | _BV(PIND3) | _BV(PIND2);
    test_spi_slave();
    // test_spi_master();

    return 0;
}
