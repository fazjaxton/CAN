#ifndef __MCP2515_H__
#define __MCP2515_H__

#include <stdint.h>

/* Operation Modes */
#define MCP2515_MODE_NORMAL         0x00
#define MCP2515_MODE_SLEEP          0x01
#define MCP2515_MODE_LOOPBACK       0x02
#define MCP2515_MODE_LISTEN_ONLY    0x03
#define MCP2515_MODE_CONFIG         0x04

enum {
    MCP2515_SPEED_500000 = 0,
    MCP2515_SPEED_250000 = 1,
    MCP2515_SPEED_125000 = 3,
    MCP2515_SPEED_100000 = 4,
    MCP2515_SPEED_62500 = 7,
    MCP2515_SPEED_50000 = 9,
    MCP2515_SPEED_31250 = 15,
    MCP2515_SPEED_25000 = 19,
    MCP2515_SPEED_20000 = 24,
    MCP2515_SPEED_15625 = 31
};

void mcp2515_init (uint8_t speed);

void mcp2515_read_regs (uint8_t addr, uint8_t* buf, uint8_t n);
void mcp2515_write_regs (uint8_t addr, const uint8_t* buf, uint8_t n);

void mcp2515_set_mode (uint8_t mode);
uint8_t mcp2515_get_msg (uint8_t rx_buf, uint32_t *id,
                                        uint8_t *data, uint8_t *len);
void mcp2515_set_msg (uint8_t tx_buf, uint32_t id, const uint8_t *data,
                                        uint8_t len, uint8_t extended);

static inline void mcp2515_set_msg_std (uint8_t tx_buf, uint32_t id,
                                        const uint8_t *data, uint8_t len)
{
    mcp2515_set_msg (tx_buf, id, data, len, 0);
}

static inline void mcp2515_set_msg_ext (uint8_t tx_buf, uint32_t id,
                                        const uint8_t *data, uint8_t len)
{
    mcp2515_set_msg (tx_buf, id, data, len, 1);
}

void mcp2515_request_tx (uint8_t tx_buf);
uint8_t mcp2515_msg_received (void);
uint8_t mcp2515_msg_sent (void);

void mcp2515_set_rx_mask (uint8_t mask_num, uint32_t mask, uint8_t extended);
void mcp2515_set_rx_filter (uint8_t filter_num, uint32_t filter, uint8_t extended);

#endif


