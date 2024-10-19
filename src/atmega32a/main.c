#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/delay.h>

#include "lib/spi.h"

#ifndef __AVR_ATmega32A__
#define __AVR_ATmega32A__
#include <avr/iom32a.h>
#endif

#define MS_DELAY  100
#define LED       (PINC0)
#define FAST_PWM0 (_BV(WGM00) | _BV(WGM01))
#define FAST_PWM1 (_BV(WGM10) | _BV(WGM11))
#define FAST_PWM2 (_BV(WGM20) | _BV(WGM21))

void test_adc(void)
{
    uint16_t adc_val;
    DDRC |= (_BV(PINC0) | _BV(PINC1) | _BV(PINC2));
    DDRA = 0;
    ADMUX = _BV(REFS0);
    ADCSRA = _BV(ADEN) | _BV(ADSC) | _BV(ADATE) | _BV(ADPS0) | _BV(ADPS1);

    while (1) {
        adc_val = ADC;
        PORTC = _BV(PINC0 + (adc_val % 3));
        _delay_ms(500);
    }
}

ISR(INT0_vect) { PORTC = bit_is_set(PIND, PIND2) ? _BV(PINC0) : 0; }

void test_interrupt(void)
{
    DDRD = 0;
    DDRC |= (_BV(PINC0) | _BV(PINC1) | _BV(PINC2));
    MCUCR = _BV(ISC00);
    GICR = _BV(INT0);
    sei();
}

void test_basic_timer(void)
{
    static uint16_t cnt;
    DDRC |= (_BV(PINC0) | _BV(PINC1) | _BV(PINC2));
    TCCR0 = _BV(CS00) | _BV(CS02);
    while (1) {
        if (TCNT0 > 250) {
            if ((++cnt) == 1000) {
                if (bit_is_set(PORTC, PINC0)) PORTC &= ~_BV(PINC0);
                else PORTC = _BV(PINC0);
                cnt = 0;
            }
        }
    }
}

ISR(TIMER1_OVF_vect)  // Timer1 ISR
{
    PORTC ^= _BV(LED);
    TCNT1 = 1;  // for 1 sec at 16 MHz
}

void test_timer_w_interrupt(void)
{
    DDRC |= _BV(LED);

    TCNT1 = 1;  // for 1 sec at 16 MHz

    TCCR1A = 0x00;
    TCCR1B = _BV(CS11);
    TIMSK = _BV(TOIE1);  // Enable timer1 overflow interrupt(TOIE1)
    sei();               // Enable global interrupts by setting global interrupt enable bit in SREG
    while (1) { _delay_ms(100); }
}

void test_manual_dim_LED(void)
{
    DDRC |= (_BV(PINC0) | _BV(PINC1) | _BV(PINC2));

    static uint8_t brightness = 128;
    static int8_t d = 1;

    PORTC = _BV(PINC2);
    _delay_ms(1000);
    while (1) {
        if (brightness == 0) d = 1;
        if (brightness == 255) { d = -1; }
        brightness += d;
        PORTC = _BV(PINC2);
        for (uint8_t i = 0; i < 255; i++) {
            if (i >= brightness) PORTC = 0;
            _delay_us(20);
        }
    }
}

void test_pwm_on_oc0(void)
{
    uint8_t duty;
    DDRB = _BV(PINB3);
    TCCR0 = FAST_PWM0 | (1 << COM01) | (1 << CS00);

    while (1) {
        for (duty = 0; duty < 255; duty++) {
            OCR0 = duty; /*increase the LED light intensity*/
            _delay_ms(8);
        }
        for (duty = 255; duty > 1; duty--) {
            OCR0 = duty; /*decrease the LED light intensity*/
            _delay_ms(8);
        }
    }
}

void test_servo(void)
{
    DDRD |= (1 << PIND5); /* Make OC1A pin as output */
    TCNT1 = 0;            /* Set timer1 count zero */
    ICR1 = 2499;          /* Set TOP count for timer1 in ICR1 register */

    /* Set Fast PWM, TOP in ICR1, Clear OC1A on compare match, clk/64 */
    TCCR1A = (1 << WGM11) | (1 << COM1A1);
    TCCR1B = (1 << WGM12) | (1 << WGM13) | (1 << CS11);
    while (1) {
        OCR1A = 65; /* Set servo shaft at -90° position */
        _delay_ms(1500);
        OCR1A = 175; /* Set servo shaft at 0° position */
        _delay_ms(1500);
        OCR1A = 300; /* Set servo at +90° position */
        _delay_ms(1500);
    }
}

void setup(void)
{
    DDRA = PORTA = _BV(PINA0);
    DDRC = PORTC = (_BV(PINC0) | _BV(PINC1) | _BV(PINC2));
}

void test_spi_master(void)
{
    setup();

    spi_init_master(SPI_PRESCALER_4);
    int x = 0;
    while (1) {
        ++x;
        x &= 0xf;
        spi_tx_rx(x);
        _delay_ms(500);
    }
}

static void spi_recv(uint8_t data) { PORTC = data & 7; }

void test_spi_slave()
{
    setup();
    spi_init_slave(spi_recv);
    while (1) { _delay_ms(1000); }
}

int main(void)
{
    setup();
    test_spi_master();
    while (1) { _delay_ms(1000); }
}
