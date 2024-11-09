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
    DDRB |= _BV(PINB5 /* LED */) | _BV(PINB3 /* PWM OUTPUT */);
    DDRC &= ~(_BV(PINC0) /* button */ | _BV(PINC1) /* Pot Analog Inputs */);

    UNSET_BIT(PORTD, PIND2);
    UNSET_BIT(PORTB, PINB5);
    UNSET_BIT(PORTB, PINB3);

    adc_init(ADC_REF_AVCC, 1, ADC_PRESCALER_128);

    pwm_init(&TCCR2A, &TCCR2B);
}

int main(void)
{
    bool enabled = false;

    setup();
    usart_enable_stdio(9600), printf("stdio is enabled!\n");

    while (1) {
        if (bit_is_set(PINC, PINC0)) {
            _delay_ms(100);
            if (bit_is_set(PINC, PINC0)) {
                if (enabled) UNSET_BIT(PORTD, PIND2), UNSET_BIT(PORTB, PINB5), enabled = false;
                else SET_BIT(PORTD, PIND2), SET_BIT(PORTB, PINB5), enabled = true;
            }
        }
        OCR2A = (adc_read() >> 2);
    }
}
