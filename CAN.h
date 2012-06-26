/*
 * Copyright (c) 2010-2011 by Kevin Smith <faz@fazjaxton.net>
 * MCP2515 CAN library for arduino.
 *
 * This file is free software; you can redistribute it and/or modify
 * it under the terms of either the GNU General Public License version 3
 * as published by the Free Software Foundation.
 */

/**
 * @file CAN.h
 * Include file for Arduino-based CAN projects.
 */
#ifndef CAN_h
#define CAN_h

#include <inttypes.h>
#include <WProgram.h>
#include "mcp2515.h"

#define DEFAULT_CAN_ID	0x0555

/* Operation Modes */
enum CAN_MODE {
	CAN_MODE_NORMAL, 		/* Transmit and receive as normal */
	CAN_MODE_SLEEP,			/* Low power mode */
	CAN_MODE_LOOPBACK,		/* Test mode; anything "sent" appears in the
							 *  receive buffer without external signaling */
	CAN_MODE_LISTEN_ONLY,   /* Receive only; do not transmit */
	CAN_MODE_CONFIG,		/* Default; Allows writing to config registers */

	CAN_MODE_COUNT
};

/* Supported speeds in bits per second */
enum CAN_SPEED {
    CAN_SPEED_500000            = MCP2515_SPEED_500000,
    CAN_SPEED_250000            = MCP2515_SPEED_250000,
    CAN_SPEED_125000            = MCP2515_SPEED_125000,
    CAN_SPEED_100000            = MCP2515_SPEED_100000,
    CAN_SPEED_62500             = MCP2515_SPEED_62500,
    CAN_SPEED_50000             = MCP2515_SPEED_50000,
    CAN_SPEED_31250             = MCP2515_SPEED_31250,
    CAN_SPEED_25000             = MCP2515_SPEED_25000,
    CAN_SPEED_20000             = MCP2515_SPEED_20000,
    CAN_SPEED_15625             = MCP2515_SPEED_15625,
};

/**
 * A class representing a single CAN message.  The message can be built
 * using the send<Type>Data functions, or the bytes of the message can
 * be set directly by accessing the public data[] array.  This class is
 * also used to retrieve a message that has been received.  The data
 * can be read using the get<type>Data functions, or can be read directly
 * by accessing the public data[] array.
 */
class CanMessage {
    public:
        /** A flag indicating whether this is an extended CAN message */
        uint8_t extended;
        /** The ID of the CAN message.  The ID is 29 bytes long if the
          * extended flag is set, or 11 bytes long if not set. */
        uint32_t id;
        /** The number of bytes in the data field (0-8) */
        uint8_t len;
        /** Array containing the bytes of the CAN message.  This array
          * may be accessed directly to set or read the CAN message.
          * This field can also be set by the setTypeData functions and
          * read by the getTypeData functions. */
        uint8_t data[8];

        CanMessage();

        /**
         * Simple interface to set up a CAN message for sending a byte data
         * type.  When received, this message should be unpacked with the
         * getByteData function.  This interface only allows one byte to be
         * packed into a message.  To pack more data, access the data array
         * directly.
         * @param b - The byte to pack into the message.
         */
        void setByteData (byte b);

        /**
         * Simple interface to set up a CAN message for sending an int data
         * type.  When received, this message should be unpacked with the
         * getIntData function.  This interface only allows one int to be
         * packed into a message.  To pack more data, access the data array
         * directly.
         * @param i - The int to pack into the message.
         */
        void setIntData (int i);

        /**
         * Simple interface to set up a CAN message for sending a long data
         * type.  When received, this message should be unpacked with the
         * getLongData function.  This interface only allows one long to be
         * packed into a message.  To pack more data, access the data array
         * directly.
         * @param l - The long to pack into the message.
         */
        void setLongData (long l);

        /**
         * A convenience function for copying multiple bytes of data into
         * the message.
         * @param data - The data to be copied into the message
         * @param len  - The size of the data
         */
        void setData (const uint8_t *data, uint8_t len);
        void setData (const char *data, uint8_t len);

        /**
         * Send the CAN message.  Once a message has been created, this
         * function sends it.
         */
        void send();

        /**
         * Simple interface to retrieve a byte from a CAN message.  This
         * should only be used on messages that were created using the
         * setByteData function on another node.
         * @return The byte contained in the message.
         */
        byte getByteFromData ();

        /**
         * Simple interface to retrieve an int from a CAN message.  This
         * should only be used on messages that were created using the
         * setIntData function on another node.
         * @return The int contained in the message.
         */
        int getIntFromData ();

        /**
         * Simple interface to retrieve a long from a CAN message.  This
         * should only be used on messages that were created using the
         * setLongData function on another node.
         * @return The long contained in the message.
         */
        long getLongFromData ();

        /**
         * A convenience function for copying multiple bytes out of a
         * CAN message.
         * @param data - The location to copy the data to.
         */
        void getData (uint8_t *data);
        void getData (char *data);
};

class CANClass {
	public:
		/**
         * Call before using any other CAN functions.
         * @param speed - Desired bus speed.  Should be one of the
         *                CAN_SPEED enumerated values.
         */
		static void begin(uint8_t speed);

		/** Call when all CAN functions are complete */
		static void end();

		/**
         * Set operational mode.
         * @param mode - One of the CAN_MODE enumerated values */
		static void setMode(uint8_t mode);

        /** Check whether a message may be sent */
        static uint8_t ready ();

		/**
         * Check whether received CAN data is available.
         * @return True if a message is available to be retrieved.
         */
        static boolean available ();

        /**
         * Retrieve a CAN message.
         * @return A CanMessage containing the retrieved message
         */
        static CanMessage getMessage ();
};

extern CANClass CAN;

#endif
