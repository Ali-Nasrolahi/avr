#include "usart.h"
#ifdef __AVR_ATmega32A__

#define UART_UBRR(baud) ((F_CPU / (16UL * baud)) - 1)

static int usart_putchar(char, FILE *);
static int usart_getchar(FILE *);

static FILE stdio_usart_stdin = FDEV_SETUP_STREAM(NULL, usart_getchar, _FDEV_SETUP_READ);
static FILE stdio_usart_stdout = FDEV_SETUP_STREAM(usart_putchar, NULL, _FDEV_SETUP_WRITE);

static int usart_putchar(char c, FILE *stream)
{
    if (c == '\n') usart_putchar('\r', stream);
    usart_send(c);
    return 0;
}
static int usart_getchar(FILE *stream)
{
    (void)stream;
    return usart_recv();
}

/* Public API */
void usart_init(uint32_t baud)
{
    UBRRH = UART_UBRR(baud) >> 8;
    UBRRL = UART_UBRR(baud);
    UCSRB = _BV(RXEN) | _BV(TXEN);
    UCSRC = _BV(URSEL) | _BV(UCSZ0) | _BV(UCSZ1);
}

void usart_send(uint8_t data)
{
    loop_until_bit_is_set(UCSRA, UDRE);
    UDR = data;
}

uint8_t usart_recv(void)
{
    loop_until_bit_is_set(UCSRA, RXC);
    return UDR;
}

void usart_enable_stdio(uint32_t baudrate)
{
    usart_init(baudrate);
    stdin = &stdio_usart_stdin;
    stdout = &stdio_usart_stdout;
}

/* Public API */
#endif