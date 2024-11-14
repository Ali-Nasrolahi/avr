#include <stdlib.h>

#include "avrlib/hal.h"
#include "avrlib/mctl/stepper.h"
#include "avrlib/utility.h"

int main(void)
{
    DDRB = 0xff;
    PORTB = 0;

    stp_config_t conf;

    conf.m_conf = stp_init_motor(1.8F);
    conf.drv_conf = stp_init_drv(&PORTB, PINB0, PINB1, PINB2, PINB3, PINB4);

    while (1) {
        stp_forward_degree(&conf, 90);
        _delay_ms(1000);
        stp_backward_degree(&conf, 45);
        _delay_ms(1000);
    }
}