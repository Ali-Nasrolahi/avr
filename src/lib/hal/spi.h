#pragma once
#include "common.h"

#define SPI_DDR  (DDRB)
#define SPI_PORT (PORTB)

#ifdef __AVR_ATmega328P__
#define SPI_SS   (PINB2)
#define SPI_MOSI (PINB3)
#define SPI_MISO (PINB4)
#define SPI_SCK  (PINB5)
#elif defined(__AVR_ATmega32A__)
#define SPI_SS   (PINB4)
#define SPI_MOSI (PINB5)
#define SPI_MISO (PINB6)
#define SPI_SCK  (PINB7)
#endif

#define SPI_PRESCALER_4   (0)
#define SPI_PRESCALER_16  (_BV(SPR0))
#define SPI_PRESCALER_64  (_BV(SPR1))
#define SPI_PRESCALER_128 (_BV(SPR0) | _BV(SPR1))

// Enable SS (ACTIVE LOW)
#define SPI_ENABLE_SS                \
    do {                             \
        SET_BIT(SPI_DDR, SPI_SS);    \
        UNSET_BIT(SPI_PORT, SPI_SS); \
    } while (0)

// Disable SS (ACTIVE LOW)
#define SPI_DISABLE_SS             \
    do {                           \
        SET_BIT(SPI_DDR, SPI_SS);  \
        SET_BIT(SPI_PORT, SPI_SS); \
    } while (0)

void spi_init_master(uint8_t clock_prescaler);
void spi_init_slave(void (*receiver)(uint8_t));
uint8_t spi_tx_rx(uint8_t);
uint8_t spi_rx_block(void);

inline uint8_t spi_rx(void) { return spi_tx_rx(0x00); }
inline void spi_tx(uint8_t data) { (void)spi_tx_rx(data); }
