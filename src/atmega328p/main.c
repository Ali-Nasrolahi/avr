#include <stdlib.h>

#include "avrlib/hal.h"
#include "avrlib/utility.h"

#if 0 /* Arduino gist */

int buttonState = 0;  // variable for reading the pushbutton status

void setup()
{
    pinMode(LED_BUILTIN, OUTPUT);
    pinMode(2, OUTPUT);
    pinMode(11, OUTPUT);
    pinMode(14, INPUT);
    pinMode(15, INPUT);
    digitalWrite(LED_BUILTIN, 0);
}
void loop()
{
    buttonState = digitalRead(14);
    if (buttonState == HIGH) {
        delay(100);
        buttonState = digitalRead(14);
        if (buttonState == HIGH) {
            bool state = !digitalRead(LED_BUILTIN);
            digitalWrite(LED_BUILTIN, state);
            digitalWrite(2, state);
        }
    }
    analogWrite(11, analogRead(15) / 4);
}
#endif

void setup(void)
{
    DDRD |= _BV(PIND2 /* EN/DIS */);
    DDRB |= _BV(PINB1 /* PWM 1 */) | _BV(PINB5 /* LED */) | _BV(PINB3 /* PWM 2 */);
    DDRC &= ~(_BV(PINC0) /* enable/disable */ | _BV(PINC1) /* Pot Analog Inputs */ |
              _BV(PINC2) /* Direction */);

    UNSET_BIT(PORTD, PIND2);
    UNSET_BIT(PORTB, PINB1);
    UNSET_BIT(PORTB, PINB3);
    UNSET_BIT(PORTB, PINB5);

    adc_init(ADC_REF_AVCC, 1, ADC_PRESCALER_128);

    pwm_init(&TCCR1A, &TCCR1B);  // OCR1A, PINB1
    pwm_init(&TCCR2A, &TCCR2B);  // OCR2A, PINB3
}

int main(void)
{
    bool enabled = false;
    bool direction = false;

    setup();
    usart_enable_stdio(9600), printf("stdio is enabled!\n");

    while (1) {
        if (bit_is_set(PINC, PINC0)) {
            _delay_ms(250);
            if (bit_is_set(PINC, PINC0)) {
                if (enabled) UNSET_BIT(PORTD, PIND2), UNSET_BIT(PORTB, PINB5), enabled = false;
                else SET_BIT(PORTD, PIND2), SET_BIT(PORTB, PINB5), enabled = true;
            }
        }

        if (bit_is_set(PINC, PINC2)) {
            _delay_ms(250);
            if (bit_is_set(PINC, PINC2)) {
                if (direction) {
                    pwm_deinit(&TCCR1A);
                    pwm_init(&TCCR2A, &TCCR2B);
                    direction = false;
                } else {
                    pwm_deinit(&TCCR2A);
                    pwm_init(&TCCR1A, &TCCR1B);
                    direction = true;
                }
            }
        }
        if (direction) OCR1A = adc_read();
        else OCR2A = (adc_read() >> 2);
    }
}
