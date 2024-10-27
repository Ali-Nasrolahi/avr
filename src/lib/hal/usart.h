#pragma once

#include "common.h"

void usart_init(uint32_t baudrate);
void usart_send(uint8_t data);
uint8_t usart_recv(void);

void usart_enable_stdio(uint32_t baudrate);