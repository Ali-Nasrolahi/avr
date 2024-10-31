#include "i2c.h"

static void (*slave_recv_f)(uint8_t);
static uint8_t (*slave_req_f)(void);

ISR(TWI_vect)
{
    switch (TW_STATUS) {
        case TW_SR_DATA_ACK:
            slave_recv_f ? slave_recv_f(TWDR) : (void)TWDR;
            break;
        case TW_ST_SLA_ACK:
        case TW_ST_DATA_ACK:
            TWDR = slave_req_f ? slave_req_f() : 0;
            break;
        case TW_BUS_ERROR:
        default:
            break;
    }

    TWCR = (1 << TWIE) | (1 << TWINT) | (1 << TWEA /* acknowledge */) | (1 << TWEN);
}

static void i2c_begin()
{
    TWCR = _BV(TWINT) | _BV(TWSTA) | _BV(TWEN);
    loop_until_bit_is_set(TWCR, TWINT);
}

static void i2c_end() { TWCR = _BV(TWINT) | _BV(TWSTO) | _BV(TWEN); }

static void i2c_tx(uint8_t data)
{
    TWDR = data;
    TWCR = _BV(TWINT) | _BV(TWEN);
    loop_until_bit_is_set(TWCR, TWINT);
}

static uint8_t i2c_rx_ack()
{
    TWCR = _BV(TWINT) | _BV(TWEN) | _BV(TWEA);
    loop_until_bit_is_set(TWCR, TWINT);
    return TWDR;
}

static uint8_t i2c_rx_nack()
{
    TWCR = _BV(TWINT) | _BV(TWEN);
    loop_until_bit_is_set(TWCR, TWINT);
    return TWDR;
}

inline static uint8_t i2c_set_rbit(uint8_t data) { return (data << 1) | 1 /* read enable */; }
inline static uint8_t i2c_set_wbit(uint8_t data) { return (data << 1) | 0 /* write enable */; }

void i2c_init_master(void)
{
    TWSR = 0;
    TWBR = 0xff;

    TWCR = _BV(TWEN);
}

void i2c_init_slave(uint8_t addr, void (*recv_f)(uint8_t), uint8_t (*req_f)(void))
{
    TWAR = addr << 1;  // Do not respond to general call address ($00)
    TWCR = _BV(TWEN) | _BV(TWEA);

    if (recv_f && req_f) {
        slave_recv_f = recv_f;
        slave_req_f = req_f;
        TWCR |= _BV(TWIE);
        sei();
    }
}

void i2c_send(uint8_t addr, uint8_t data)
{
    i2c_begin();
    i2c_tx(i2c_set_wbit(addr));  // SLA+W <==> Master Transmitter Mode
    i2c_tx(data);
    i2c_end();
}

uint8_t i2c_recv(uint8_t addr)
{
    i2c_begin();
    i2c_tx(i2c_set_rbit(addr));    // SLA+R <==> Master Receiver Mode
    uint8_t data = i2c_rx_nack();  // Receive a single byte, then close the connection
    i2c_end();
    return data;
}

void i2c_recvn(uint8_t addr, void* buf, size_t buf_size)
{
    (void)buf;
    (void)addr;
    (void)buf_size;
    (void)i2c_rx_ack;
}