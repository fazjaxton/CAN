/*
 * Copyright (c) 2010-2011 by Kevin Smith <faz@fazjaxton.net>
 * MCP2515 CAN library for arduino.
 *
 * This file is free software; you can redistribute it and/or modify
 * it under the terms of either the GNU General Public License version 3
 * as published by the Free Software Foundation.
 */

/**
 * @file CAN.cpp
 * Arduino interface into the CAN driver.
 */
#include "Arduino.h"
#include "CAN.h"
#include <SPI.h>

CanMessage::CanMessage ()
{
    extended = 0;
    id = DEFAULT_CAN_ID;
    len = 0;
    pos = 0;
}

void CanMessage::setByteData (byte val)
{
    if (this->len + sizeof(val) <= CAN_BYTES_MAX) {
        this->data[this->len++] = val;
    }
}

void CanMessage::setIntData (int val)
{
    if (this->len + sizeof(val) <= CAN_BYTES_MAX) {
        /* Big-endian network byte ordering */
        this->data[this->len++] = (uint8_t)(val >> 8);
        this->data[this->len++] = (uint8_t)(val);
    }
}

void CanMessage::setLongData (long val)
{
    if (this->len + sizeof(val) <= CAN_BYTES_MAX) {
        /* Big-endian network byte ordering */
        this->data[this->len++] = (uint8_t)(val >> 24);
        this->data[this->len++] = (uint8_t)(val >> 16);
        this->data[this->len++] = (uint8_t)(val >> 8);
        this->data[this->len++] = (uint8_t)(val);
    }
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
    byte val = 0;

    if (this->pos + sizeof(val) <= this->len) {
        val =  this->data[this->pos++];
    }

    return val;
}

int CanMessage::getIntFromData ()
{
    int val = 0;

    if (this->pos + sizeof(val) <= this->len) {
        val |= (int)(this->data[this->pos++]) << 8;
        val |= (int)(this->data[this->pos++]);
    }

    return val;
}

long CanMessage::getLongFromData ()
{
    long val = 0;

    if (this->pos + sizeof(val) <= this->len) {
        val |= (long)(this->data[this->pos++]) << 24;
        val |= (long)(this->data[this->pos++]) << 16;
        val |= (long)(this->data[this->pos++]) << 8;
        val |= (long)(this->data[this->pos++]);
    }

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

void CanMessage::clear (void)
{
    this->len = 0;
    this->pos = 0;
}

void CanMessage::print (uint8_t format)
{
    uint8_t i;

    Serial.print (this->id, format);

    Serial.print (" [");
    Serial.print (len, DEC);
    Serial.print ("]:");

    for (i = 0; i < this->len; i++) {
        Serial.print (" ");
        Serial.print (this->data[i], format);
    }

    Serial.println ("");
}

/*
 * CANClass
 */
void CANClass::begin(uint32_t bit_time) {
    SPI.begin();
    SPI.setDataMode(SPI_MODE0);
    SPI.setBitOrder(MSBFIRST);
    SPI.setClockDivider(SPI_CLOCK_DIV4);

    mcp2515_init (bit_time);

    /* Set mask to not filter any bits (allow all identifiers) */
    mcp2515_set_rx_mask (0, 0, 0);

    /* Set a filter for both standard and extended message types.  Since the
     * extended bit in the filter registers in not maskable, an acceptance
     * filter has to be explicitly set to accept both types. */
    mcp2515_set_rx_filter (0, 0, 0);
    mcp2515_set_rx_filter (1, 0, 1);
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

