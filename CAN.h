/*
 * Copyright (c) 2010-2011 by Kevin Smith <faz@fazjaxton.net>
 * MCP2515 CAN library for arduino.
 *
 * This file is free software; you can redistribute it and/or modify
 * it under the terms of either the GNU General Public License version 3
 * as published by the Free Software Foundation.
 */

#ifndef CAN_h
#define CAN_h

#include <inttypes.h>
#include <WProgram.h>

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

class CanMessage {
    public:
        uint8_t extended;
        uint32_t id;
        uint8_t len;
        uint8_t data[8];

        CanMessage();

        /* Functions to copy different data types into the message */
        void setByteData (byte b);
        void setIntData (int i);
        void setLongData (long l);

        /* Conveneince functions for copying strings and byte arrays */
        void setData (const uint8_t *data, uint8_t len);
        void setData (const char *data, uint8_t len);

        /* Send a message */
        void send();

        /* Functions to retrieve different data types from the message */
        byte getByteFromData ();
        int getIntFromData ();
        long getLongFromData ();

        /* Conveneince functions for copying strings and byte arrays */
        void getData (uint8_t *data);
        void getData (char *data);
};

class CANClass {
	public:
		/* Called before any other CAN functions are used */
		static void begin();
		/* Called when all CAN functions are complete */
		static void end();

		/* Set operational mode; pass in one of the modes enumerated above */
		static void setMode(uint8_t);

		/* Check whether received CAN data is available */
        static uint8_t available ();

        /* Recieve a CAN message */
        static CanMessage getMessage ();
};

extern CANClass CAN;

#endif
