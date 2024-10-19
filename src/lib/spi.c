#include "spi.h"

static void (*slave_recv_f)(uint8_t);

ISR(SPI_STC_vect) { slave_recv_f ? slave_recv_f(SPDR) : (void)SPDR; }

void spi_init_master(uint8_t prescaler)
{
    // MOSI ,SCK ,SS as an output
    SPI_DDR |= _BV(SPI_MOSI) | _BV(SPI_SCK) | _BV(SPI_SS);

    // MISO as an input
    SPI_DDR &= ~_BV(SPI_MISO);

    // Enable SPI, Master, set clockrate fck/16
    SPCR = _BV(SPE) | _BV(MSTR) | prescaler;
}

void spi_init_slave(void (*receiver)(uint8_t))
{
    // MISO as an output
    SPI_DDR |= _BV(SPI_MISO);

    // MOSI ,SS and SCK as an input
    SPI_DDR &= ~(_BV(SPI_SS) | _BV(SPI_MOSI) | _BV(SPI_SCK));

    // Receiver Interrupt handler
    if (receiver) {
        slave_recv_f = receiver;
        sei();
    }

    // Enable SPI and receiver interrupt
    SPCR = _BV(SPE) | _BV(SPIE);
}

uint8_t spi_tx_rx(uint8_t data)
{
    SPDR = data;
    loop_until_bit_is_set(SPSR, SPIF);
    return SPDR;
}

uint8_t spi_rx_block(void)
{
    loop_until_bit_is_set(SPSR, SPIF);
    return SPDR;
}
