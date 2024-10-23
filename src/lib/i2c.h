#pragma once

#include "common.h"

#define I2C_DDR  (DDRC)
#define I2C_PORT (PORTC)

#ifdef __AVR_ATmega328P__
#define I2C_SCL (PINC5)
#define I2C_SDA (PINC4)
#elif defined(__AVR_ATmega32A__)
#define I2C_SCL (PINC0)
#define I2C_SDA (PINC1)
#endif

void i2c_init_master();
void i2c_init_slave(uint8_t addr, void (*recv_f)(uint8_t), uint8_t (*req_f)(void));

void i2c_send(uint8_t addr, uint8_t data);
/**
 * @brief Receives a single byte from slave.
 *
 * @param addr Slave address
 * @return uint8_t received data
 */
uint8_t i2c_recv(uint8_t addr);

/**
 * @brief Receives an arbitrary n bytes from slave.
 *
 * @param addr Slave address
 * @param buf Data buffer
 * @param buf_size Buffer size
 */
void i2c_recvn(uint8_t addr, void* buf, size_t buf_size);