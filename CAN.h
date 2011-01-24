/*
 * Copyright (c) 2011 by Kevin Smith <faz@fazjaxton.net>
 * CAN Library for Arduino
 *
 * This file is free software; you can redistribute it and/or modify
 * it under the terms of either the GNU General Public License version 2
 * or the GNU Lesser General Public License version 2.1, both as
 * published by the Free Software Foundation.
 */

#ifndef CAN_h
#define CAN_h

#include <SPI.h>
#include <inttypes.h>
#define DEFAULT_CAN_ID	0x0555

/* Operation Modes */
enum {
	CAN_MODE_NORMAL, 		/* Transmit and receive as normal */
	CAN_MODE_SLEEP,			/* Low power mode */
	CAN_MODE_LOOPBACK,		/* Test mode; anything "sent" appears in the
							 *  receive buffer without external signaling */
	CAN_MODE_LISTEN_ONLY,   /* Receive only; do not transmit */
	CAN_MODE_CONFIG,		/* Default; Allows writing to config registers */

	CAN_MODE_COUNT
};

class CANClass {
	public:
		/* Called before any other CAN functions are used */
		static void begin();
		/* Called when all CAN functions are complete */
		static void end();

		/* Set operational mode; pass in one of the modes enumerated above */
		static void setMode(uint8_t);

		/* Set the message ID for outgoing messages */
		static void setMessageID(uint16_t);

		/* Send a single byte of data in a CAN message */
		static void sendByte(uint8_t);
		/* Send an integer value in a CAN message */
		static void sendInt(uint16_t);
		/* Send a long value in a CAN message */
		static void sendLong(uint32_t);
		/* Send a custom data buffer in a CAN message */
		static void sendData(const uint8_t *, uint8_t);
		static void sendData(const char *, uint8_t);

		/* Check whether received CAN data is available */
		static uint8_t available();
		/* Read the ID of the received message */
		static uint16_t getMessageID(void);

		/* Receive a single byte of data */
		static uint8_t receiveByte(void);
		/* Receive an integer value */
		static uint16_t receiveInt(void);
		/* Receive a long value */
		static uint32_t receiveLong(void);
		/* Receive a custom data buffer */
		static uint8_t receiveData(uint8_t *, uint8_t);
		static uint8_t receiveData(char *, uint8_t);

		/* Mark a message as received.  This is only necessary if you want
		 * to ignore a message or only read its ID.  All of the receive
		 * functions do this automatically. */
		static void markReceived (void);
};

extern CANClass CAN;

#endif
