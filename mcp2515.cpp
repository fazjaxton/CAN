#include "mcp2515.h"
#include "mcp2515_regs.h"
#include "my_spi.h"

#define SYNC_JUMP_WIDTH 1

/* Sync (1) + These lengths = 16;  16 * Tq = Nominal Bit Time */
#define PROP_SEG_LEN    5
#define PHASE_SEG_1_LEN 5
#define PHASE_SEG_2_LEN 5

/* SPI Commands */
enum {
    MCP2515_CMD_RESET       = 0xC0,
    MCP2515_CMD_READ        = 0x03,
    MCP2515_CMD_WRITE       = 0x02,
    MCP2515_CMD_RTS         = 0x80,
    MCP2515_CMD_READ_STATUS = 0xA0,
    MCP2515_CMD_BIT_MODIFY  = 0x05,
};

void mcp2515_read_regs (uint8_t addr, uint8_t* buf, uint8_t n)
{
    int i;

    assert_ss();
    spi_send(MCP2515_CMD_READ);
    spi_send(addr);
    for (i=0; i<n; i++)
        buf[i] = spi_receive();
    deassert_ss();
}

void mcp2515_write_regs (uint8_t addr, const uint8_t* buf, uint8_t n)
{
    int i;

    assert_ss();
    spi_send(MCP2515_CMD_WRITE);
    spi_send(addr);
    for (i=0; i<n; i++)
        spi_send(buf[i]);
    deassert_ss();
}

static void mcp2515_write_reg (uint8_t addr, uint8_t buf)
{
    assert_ss();
    spi_send(MCP2515_CMD_WRITE);
    spi_send(addr);
    spi_send(buf);
    deassert_ss();
}

static void mcp2515_bit_modify (uint8_t addr, uint8_t mask, uint8_t bits)
{
    assert_ss();
    spi_send(MCP2515_CMD_BIT_MODIFY);
    spi_send(addr);
    spi_send(mask);
    spi_send(bits);
    deassert_ss();
}

/*
 * Initialize the MCP2515
 */
void mcp2515_init (uint8_t speed)
{
    mcp2515_write_reg (CNF1,
            (speed << BRP) |
            ((SYNC_JUMP_WIDTH - 1) << SJW) );

    mcp2515_write_reg (CNF2,
            ((PROP_SEG_LEN - 1) << PRSEG) |
            ((PHASE_SEG_1_LEN - 1) << PHSEG1) |
            (0 << SAM) |  /* Sample once */
            (1 << BTLMODE) );  /* Phase 2 set by CNF3 */

    mcp2515_write_reg (CNF3,
            ((PHASE_SEG_2_LEN - 1) << PHSEG2) |
            (0 << WAKFIL) );

    mcp2515_write_reg (REG(RX, 0, CTRL),
            (0x0 << RXM) |
            (0 << BUKT) );
}

/*
 * Set the operating mode of the MCP2515
 */
void mcp2515_set_mode (uint8_t mode)
{
    mcp2515_bit_modify (CANCTRL, REQOP_MASK, mode << REQOP);
}

/*
 * Reads a message from the receive buffer and marks it as read.
 */
uint8_t mcp2515_get_msg (uint8_t rx_buf, uint32_t *id,
                    uint8_t *data, uint8_t *len)
{
    uint8_t buf[5];
    uint8_t extended;

    mcp2515_read_regs (REG(RX, rx_buf, SIDH), buf, 5);
    *len = buf[4] & 0x0f;
    if (*len > 8)
        *len = 8;
    mcp2515_read_regs (REG(RX, rx_buf, D0), data, *len);

    extended = buf[1] & (1 << IDE);
    if (extended) {
        *id = ((uint32_t)(buf[0]) << 21) |
              ((uint32_t)(buf[1] & 0xE0) << 13) |
              ((uint32_t)(buf[1] & 0x03) << 16) |
              ((uint32_t)(buf[2]) << 8) |
              ((uint32_t)(buf[3]) << 0);
    } else {
        *id = ((uint32_t)buf[0] << 3) |
              ((uint32_t)buf[1] >> 5);
    }

    mcp2515_bit_modify (CANINTF, 1 << (RX0IF + rx_buf), 0);

    return extended;
}

/*
 * Loads a message into the transmit buffer
 */
void mcp2515_set_msg (uint8_t tx_buf, uint32_t id, const uint8_t *data,
                    uint8_t len, uint8_t extended)
{
    uint8_t buf[5];

    if (extended) {
        buf[0] = (uint8_t)(id >> 21);
        buf[1] = (uint8_t)(((id >> 13) & 0xE0) |
                (1 << EXIDE) |
                ((id >> 16) & 0x03));
        buf[2] = (uint8_t)(id >> 8);
        buf[3] = (uint8_t)(id);
    } else {
        buf[0] = (uint8_t)(id >> 3);
        buf[1] = (uint8_t)(id << 5);
    }

    if (len > 8)
        len = 8;

    buf[4] = len << DLC0;

    mcp2515_write_regs (REG(TX, tx_buf, SIDH), buf, 5);
    mcp2515_write_regs (REG(TX, tx_buf, D0), data, len);
}

/*
 * Requests transmission of the loaded message
 */
void mcp2515_request_tx (uint8_t tx_buf)
{
    mcp2515_bit_modify (REG(TX, tx_buf, CTRL), 1 << TXREQ, 1 << TXREQ);
}

/*
 * Returns non-zero if a message has been received
 */
uint8_t mcp2515_msg_received (void)
{
    uint8_t byte;

    mcp2515_read_regs (CANINTF, &byte, 1);

    return (byte & ((1 << RX1IF) | (1 << RX0IF)));
}

/* 
 * Returns non-zero if the message has been sent
 */
uint8_t mcp2515_msg_sent (void)
{
	uint8_t byte;

	mcp2515_read_regs (REG(TX, 0, CTRL), &byte, 1);

	return (!(byte & (1 << TXREQ)));
}

void mcp2515_set_rx_mask (uint8_t mask_num, uint32_t mask, uint8_t extended)
{
    uint8_t reg;
    uint8_t buf[4];

    if (mask_num == 0)
        reg = RXM0SIDH;
    else
        reg = RXM1SIDH;

    if (extended) {
        buf[0] = (uint8_t)(mask >> 21);
        buf[1] = (uint8_t)(((mask >> 13) & 0xE0)
                         | ((mask >> 16) & 0x03));
        buf[2] = (uint8_t)(mask >> 8);
        buf[3] = (uint8_t)(mask >> 0);
    } else {
        buf[0] = (uint8_t)(mask >> 3);
        buf[1] = (uint8_t)(mask << 5);
        buf[2] = 0;
        buf[3] = 0;
    }

    mcp2515_write_regs (reg, buf, 4);
}

void mcp2515_set_rx_filter (uint8_t filter_num, uint32_t filter, uint8_t extended)
{
    uint8_t reg;
    uint8_t buf[4];

    /* Calculate correct register */
    reg = filter_num * 4;
    if (reg >= 12)
        reg += 4;

    if (extended) {
        buf[0] = (uint8_t)(filter >> 21);
        buf[1] = (uint8_t)(((filter >> 13) & 0xE0)
                         | ((filter >> 16) & 0x03)
                         | (1 << EXIDE));
        buf[2] = (uint8_t)(filter >> 8);
        buf[3] = (uint8_t)(filter >> 0);
    } else {
        buf[0] = (uint8_t)(filter >> 3);
        buf[1] = (uint8_t)(filter << 5);
        buf[2] = 0;
        buf[3] = 0;
    }

    mcp2515_write_regs (reg, buf, 4);
}
