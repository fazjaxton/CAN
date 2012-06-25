/*
 * Copyright (c) 2010-2011 by Kevin Smith <faz@fazjaxton.net>
 * MCP2515 CAN library for arduino.
 *
 * This file is free software; you can redistribute it and/or modify
 * it under the terms of either the GNU General Public License version 3
 * as published by the Free Software Foundation.
 */

#include "CAN.h"
#include <SPI.h>

CanMessage::CanMessage ()
{
    extended = 0;
    id = DEFAULT_CAN_ID;
    len = 0;
}

void CanMessage::setByteData (byte b)
{
    len = 1;

    data[0] = b;
}

void CanMessage::setIntData (int i)
{
    len = 2;

	/* Big-endian network byte ordering */
	data[0] = i >> 8;
	data[1] = i & 0xff;
}

void CanMessage::setLongData (long l)
{
    len = 4;

	/* Big-endian network byte ordering */
	data[0] = (l >> 24) & 0xff;
	data[1] = (l >> 16) & 0xff;
	data[2] = (l >>  8) & 0xff;
	data[3] = (l >>  0) & 0xff;
}

void CanMessage::setData (const uint8_t *data, uint8_t len)
{
    byte i;

    this->len = len;

    for (i=0; i<len; i++) {
        this->data[i] = data[i];
    }
}

void CanMessage::setData (const char *data, uint8_t len)
{
    setData ((const uint8_t *)data, len);
}

void CanMessage::send ()
{
	mcp2515_set_msg (0, id, data, len, extended);
	mcp2515_request_tx (0);
}

byte CanMessage::getByteFromData()
{
	return data[0];
}

int CanMessage::getIntFromData ()
{
    int val;

	val |= (uint16_t)data[0] << 8;
	val |= (uint16_t)data[1] << 0;

	return val;
}

long CanMessage::getLongFromData ()
{
    long val;

	val |= (uint32_t)data[0] << 24;
	val |= (uint32_t)data[1] << 16;
	val |= (uint32_t)data[2] <<  8;
	val |= (uint32_t)data[3] <<  0;

	return val;
}

void CanMessage::getData (uint8_t *data)
{
    byte i;

    for (i=0; i<len; i++) {
        data[i] = this->data[i];
    }
}

void CanMessage::getData (char *data)
{
    getData ((uint8_t *)data);
}

/*
 * CANClass
 */
void CANClass::begin(uint8_t speed) {
	SPI.begin();
	SPI.setDataMode(SPI_MODE0);
	SPI.setBitOrder(MSBFIRST);
	SPI.setClockDivider(SPI_CLOCK_DIV4);

	mcp2515_init (speed);
}

void CANClass::end() {
    SPI.end ();
}

void CANClass::setMode (uint8_t mode)
{
	mcp2515_set_mode (mode);
}

uint8_t CANClass::ready ()
{
	return mcp2515_msg_sent ();
}

boolean CANClass::available ()
{
	return (boolean)mcp2515_msg_received();
}

CanMessage CANClass::getMessage ()
{
    CanMessage m;

	m.extended = mcp2515_get_msg (0, &m.id, m.data, &m.len);

    return m;
}

CANClass CAN;

