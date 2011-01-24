#include <SPI.h>
#include <CAN.h>

/* This program demonstrates how to send and receive different
 * data types using different message IDs.  If you are writing
 * a program that sends messages with different lengths or types,
 * you need to check the message ID before receiving to know
 * which receive method to call.  */

/* If you don't have two CAN shields, this program can run on
 * just one by setting the mode to CAN_MODE_LOOPBACK instead
 * of CAN_MODE_NORMAL. */

unsigned long last;
unsigned long now;

const int small_message_id = 0xA1;
const int int_message_id = 0xA2;
const int string_message_id = 0xA3;

byte message_to_send;

int id;
byte b;
int i;
char s[8];


void setup()
{
  CAN.begin ();
  CAN.setMode (CAN_MODE_NORMAL);
  Serial.begin (115200);
  
  last = 0;
  message_to_send = 0;
}

void loop()
{
  /* Send a message every second */
  now = millis ();
  if (now / 1000 > last) {
    last = now / 1000;

    if (message_to_send == 0) {
      CAN.setMessageID (small_message_id);
      CAN.sendByte (3);
    } else if (message_to_send == 1) {
      CAN.setMessageID (int_message_id);
      CAN.sendInt (1234);
    } else {
      CAN.setMessageID (string_message_id);
      
      /* Strings have one extra character to indicate their end.
       * See http://arduino.cc/en/Reference/String */
      CAN.sendData ("Hello", 6);
    }
    message_to_send++;
    if (message_to_send >= 3)
        message_to_send = 0;
  }

  /* Receive message if available */
  if (CAN.available ()) {
    id = CAN.getMessageID();
    
    /* Receive different data types based on message ID */
    if (id == small_message_id) {
      b = CAN.receiveByte ();
      Serial.println (b, DEC);
    } else if (id == int_message_id) {
      i = CAN.receiveInt ();
      Serial.println (i);
    } else if (id == string_message_id) {
      CAN.receiveData (s, 8);
      Serial.println (s);
    }
  }
}

