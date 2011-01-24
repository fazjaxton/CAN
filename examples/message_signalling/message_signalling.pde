#include <SPI.h>
#include <CAN.h>

/* This program demonstrates signalling using only message IDs.
 * CAN messages may be zero length, and the meaning is conveyed
 * by the message ID itself.  Note that if you do not call one
 * of the receive functions, you must mark the message as
 * received by calling CAN.markReceived */

/* If you don't have two CAN shields, this program can run on
 * just one by setting the mode to CAN_MODE_LOOPBACK instead
 * of CAN_MODE_NORMAL. */

unsigned long last;
unsigned long now;
const int odd_id = 0xA1;
const int even_id = 0xA2;
byte val;
int message;


void setup()
{
  CAN.begin ();
  CAN.setMode (CAN_MODE_NORMAL);
  Serial.begin (115200);
  
  /* Not actually random; just a demonstration */
  randomSeed (0);
  
  last = 0;
}

void loop()
{
  /* Send a message every second */
  now = millis ();
  if (now / 1000 > last) {
    last = now / 1000;

    /* Pick random number and send a message indicating
     * whether it is odd or even */    
    val = random (0, 255);
    if (val % 2 == 0)
      CAN.setMessageID (even_id);
    else
      CAN.setMessageID (odd_id);

    /* Send empty message */
    CAN.sendData ("", 0);
  }

  /* Receive message if available */
  if (CAN.available ()) {
    message = CAN.getMessageID();
    if (message == even_id)
      Serial.println ("Even");
    else
      Serial.println ("Odd");
      
    /* Mark message as received since we only read the ID */
    CAN.markReceived ();
  }
}

