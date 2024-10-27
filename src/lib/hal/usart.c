#include "usart.h"

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
#ifdef __AVR_ATmega328P__
    UBRR0H = UART_UBRR(baud) >> 8;
    UBRR0L = UART_UBRR(baud);
    UCSR0B = _BV(RXEN0) | _BV(TXEN0);
    UCSR0C = _BV(UCSZ00) | _BV(UCSZ01);
#else
    UBRRH = UART_UBRR(baud) >> 8;
    UBRRL = UART_UBRR(baud);
    UCSRB = _BV(RXEN) | _BV(TXEN);
    UCSRC = _BV(URSEL) | _BV(UCSZ0) | _BV(UCSZ1);
#endif
}

void usart_send(uint8_t data)
{
#ifdef __AVR_ATmega328P__
    loop_until_bit_is_set(UCSR0A, UDRE0);
    UDR0 = data;
#else
    loop_until_bit_is_set(UCSRA, UDRE);
    UDR = data;
#endif
}

uint8_t usart_recv(void)
{
#ifdef __AVR_ATmega328P__
    loop_until_bit_is_set(UCSR0A, RXC0);
    return UDR0;
#else
    loop_until_bit_is_set(UCSRA, RXC);
    return UDR;
#endif
}

void usart_enable_stdio(uint32_t baudrate)
{
    usart_init(baudrate);
    stdin = &stdio_usart_stdin;
    stdout = &stdio_usart_stdout;
}