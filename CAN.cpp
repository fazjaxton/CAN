/*
 * Copyright (c) 2010 by Kevin Smith <faz@fazjaxton.net>
 * MCP2515 CAN library for arduino.
 *
 * This file is free software; you can redistribute it and/or modify
 * it under the terms of either the GNU General Public License version 2
 * or the GNU Lesser General Public License version 2.1, both as
 * published by the Free Software Foundation.
 */

#include "CAN.h"
#include "mcp2515_regs.h"

enum {
	MCP2515_CMD_RESET	= 0xC0,
	MCP2515_CMD_READ	= 0x03,
	MCP2515_CMD_WRITE	= 0x02,
	MCP2515_CMD_RTS		= 0x80,
	MCP2515_CMD_READ_STATUS	= 0xA0,
	MCP2515_CMD_BIT_MODIFY	= 0x05,
};

#define REG(d,n,r)	((d + (n << 4)) | r)
#define TX		0x30
#define RX		0x60

#define SYNC_JUMP_WIDTH	1
#define BR_PRESCALER	0

/* Sync (1) + These lengths = 16;  16 * Tq = Nominal Bit Time */
#define PROP_SEG_LEN	5
#define PHASE_SEG_1_LEN	5
#define PHASE_SEG_2_LEN	5

#define CAN_DATA_LEN    8

const int slaveSelectPin = 10;

static void mcp2515_read_regs (uint8_t addr, uint8_t* buf, uint8_t n)
{
	int i;

	digitalWrite(slaveSelectPin, LOW);
	SPI.transfer (MCP2515_CMD_READ);
	SPI.transfer (addr);
	for (i=0; i<n; i++)
		buf[i] = SPI.transfer(0);
	digitalWrite(slaveSelectPin, HIGH);
}

static void mcp2515_write_regs (uint8_t addr, const uint8_t* buf, uint8_t n)
{
	int i;

	digitalWrite(slaveSelectPin, LOW);
	SPI.transfer (MCP2515_CMD_WRITE);
	SPI.transfer (addr);
	for (i=0; i<n; i++)
		SPI.transfer(buf[i]);
	digitalWrite(slaveSelectPin, HIGH);
}

static void mcp2515_write_reg (uint8_t addr, uint8_t val)
{
	digitalWrite(slaveSelectPin, LOW);
	SPI.transfer (MCP2515_CMD_WRITE);
	SPI.transfer (addr);
	SPI.transfer(val);
	digitalWrite(slaveSelectPin, HIGH);
}

static void mcp2515_bit_modify (uint8_t addr, uint8_t mask, uint8_t bits)
{
	digitalWrite(slaveSelectPin, LOW);
	SPI.transfer(MCP2515_CMD_BIT_MODIFY);
	SPI.transfer(addr);
	SPI.transfer(mask);
	SPI.transfer(bits);
	digitalWrite(slaveSelectPin, HIGH);
}

void mcp2515_set_mode (uint8_t mode)
{
	if (mode >= CAN_MODE_COUNT)
		return;

	mcp2515_bit_modify (CANCTRL, REQOP_MASK, mode << REQOP);
}

void mcp2515_set_id (uint16_t id)
{
	uint8_t buf[2];

	buf[0] = (uint8_t)(id >> 3);
	buf[1] = (uint8_t)(id << 5);

	mcp2515_write_regs (REG(TX, 0, SIDH), buf, 2);
}

uint16_t mcp2515_get_id (void)
{
	uint16_t id = 0;
	uint8_t buf[2];

	mcp2515_read_regs (REG(RX, 0, SIDH), buf, 2);

	id |= (uint16_t)buf[0] << 3;
	id |= (uint16_t)buf[1] >> 5;

	return id;
}

void mcp2515_set_msg (const uint8_t *data, uint8_t len)
{
	if (len > CAN_DATA_LEN)
		len = CAN_DATA_LEN;

	mcp2515_write_reg (REG(TX, 0, DLC), len);
	mcp2515_write_regs (REG(TX, 0, D0), data, len);
}

void mcp2515_mark_received (uint8_t rx_buf)
{
	mcp2515_bit_modify (CANINTF, 1 << (RX0IF + rx_buf), 0);
}

uint8_t mcp2515_get_msg (uint8_t rx_buf, uint8_t *data, uint8_t len)
{
	uint8_t msg_len;

	mcp2515_read_regs (REG(RX, rx_buf, DLC), &msg_len, 1);
	msg_len &= 0x0f;

    if (msg_len > CAN_DATA_LEN)
        msg_len = CAN_DATA_LEN;
	if (msg_len > len)
		msg_len = len;

	mcp2515_read_regs (REG(RX, rx_buf, D0), data, msg_len);
	mcp2515_mark_received (rx_buf);

	return msg_len;
}

void mcp2515_request_tx (void)
{
	mcp2515_bit_modify (REG(TX, 0, CTRL), 1 << TXREQ, 1 << TXREQ);
}

uint8_t mcp2515_msg_received (void)
{
	uint8_t byte;

	mcp2515_read_regs (CANINTF, &byte, 1);

	return (byte & (1 << RX0IF));
}

uint8_t mcp2515_msg_sent (void)
{
	uint8_t byte;

	mcp2515_read_regs (REG(TX, 0, CTRL), &byte, 1);

	return (!(byte & (1 << TXREQ)));
}

void CANClass::begin() {
	SPI.begin();
	SPI.setDataMode(SPI_MODE0);
	SPI.setBitOrder(MSBFIRST);
	SPI.setClockDivider(SPI_CLOCK_DIV4);

	mcp2515_write_reg (CNF1,
			(BR_PRESCALER << BRP) |
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
			(0x3 << RXM) |
			(0 << BUKT) );

	mcp2515_set_id (DEFAULT_CAN_ID);
}

void CANClass::end() {
	SPI.end();
}

void CANClass::setMode (uint8_t mode)
{
	mcp2515_set_mode (mode);
}

void CANClass::setMessageID (uint16_t id)
{
	mcp2515_set_id (id);
}

void CANClass::sendByte (uint8_t val)
{
	mcp2515_set_msg (&val, 1);
	mcp2515_request_tx ();
}

void CANClass::sendInt (uint16_t val)
{
	uint8_t buf[2];

	/* Big-endian network byte ordering */
	buf[0] = val >> 8;
	buf[1] = val & 0xff;

	mcp2515_set_msg (buf, 2);
	mcp2515_request_tx ();
}

void CANClass::sendLong (uint32_t val)
{
	uint8_t buf[4];

	/* Big-endian network byte ordering */
	buf[0] = (val >> 24) & 0xff;
	buf[1] = (val >> 16) & 0xff;
	buf[2] = (val >>  8) & 0xff;
	buf[3] = (val >>  0) & 0xff;

	mcp2515_set_msg (buf, 4);
	mcp2515_request_tx ();
}

void CANClass::sendData (const uint8_t *data, uint8_t len)
{
	mcp2515_set_msg (data, len);
	mcp2515_request_tx ();
}

/* Compiler doesn't like casting "uint8_t *" to "char *" automatically, so
 * this method does it explicitly. */
void CANClass::sendData (const char *data, uint8_t len)
{
	sendData ((const uint8_t *)data, len);
}

uint8_t CANClass::available (void)
{
	return mcp2515_msg_received();
}

uint16_t CANClass::getMessageID(void)
{
	return mcp2515_get_id();
}

uint8_t CANClass::receiveByte(void)
{
	uint8_t buf;

	mcp2515_get_msg (0, &buf, 1);

	return buf;
}

uint16_t CANClass::receiveInt (void)
{
	uint8_t buf[2];
	uint16_t val = 0;

	mcp2515_get_msg (0, buf, 2);

	val |= (uint16_t)buf[0] << 8;
	val |= (uint16_t)buf[1] << 0;

	return val;
}

uint32_t CANClass::receiveLong (void)
{
	uint8_t buf[4];
	uint32_t val = 0;

	mcp2515_get_msg (0, buf, 4);

	val |= (uint32_t)buf[0] << 24;
	val |= (uint32_t)buf[1] << 16;
	val |= (uint32_t)buf[2] <<  8;
	val |= (uint32_t)buf[3] <<  0;

	return val;
}

uint8_t CANClass::receiveData (uint8_t *data, uint8_t len)
{
	return mcp2515_get_msg (0, data, len);
}

/* Same as above; explicitly cast from "char *" to "uint8_t *". */
uint8_t CANClass::receiveData (char *data, uint8_t len)
{
	return receiveData ((uint8_t *)data, len);
}

void CANClass::markReceived (void)
{
	mcp2515_mark_received (0);
}

CANClass CAN;

