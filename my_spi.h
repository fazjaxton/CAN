/*
 * Copyright (c) 2010-2011 by Kevin Smith <faz@fazjaxton.net>
 *
 * This file is free software; you can redistribute it and/or modify
 * it under the terms of either the GNU General Public License version 3
 * as published by the Free Software Foundation.
 */

#ifndef __SPI_H__
#define __SPI_H__

/* Set to '1' if building CAN library for Arduino
 * '0' for a standalone C/C++ program */
#define ARDUINO     1

#include <stdint.h>

#if ARDUINO
#include <SPI.h>
#endif


#if ARDUINO
const int slaveSelectPin = 10;

static inline void assert_ss (void)
{
    digitalWrite (slaveSelectPin, LOW);
}

static inline void deassert_ss (void)
{
    digitalWrite (slaveSelectPin, HIGH);
}

static uint8_t spi_transfer (uint8_t byte)
{
    return SPI.transfer (byte);
}

#else

void init_spi (void);
void assert_ss (void);
void deassert_ss (void);
uint8_t spi_transfer (uint8_t byte);

#endif

static inline void spi_send (uint8_t byte)
{
    spi_transfer (byte);
}

static inline uint8_t spi_receive (void)
{
    return spi_transfer (0);
}

#endif
