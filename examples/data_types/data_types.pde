#include <SPI.h>
#include <CAN.h>

/* This program demonstrates how to send and receive different
 * data types using different message IDs.  If you are writing
 * a program that sends messages with different lengths or types,
 * you can check the message ID to know which "get" method to
 * call.  */

/* If you don't have two CAN shields, this program can run on
 * just one by setting the mode to CAN_MODE_LOOPBACK instead
 * of CAN_MODE_NORMAL. */

unsigned long last;
unsigned long now;

const int small_message_id = 0xA1;
const int int_message_id = 0xA2;
const int string_message_id = 0xA3;

byte message_to_send;

byte b;
int i;
char s[8];
CanMessage sendMessage;
CanMessage receiveMessage;

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
  if (now > last + 1000) {
    last = now;

    if (message_to_send == 0) {
      sendMessage.id = small_message_id;
      sendMessage.setByteData (3);
    } else if (message_to_send == 1) {
      sendMessage.id = int_message_id;
      sendMessage.setIntData (1234);      
    } else {
      sendMessage.id = string_message_id;
      
      /* Strings have one extra character to indicate their end.
       * See http://arduino.cc/en/Reference/String */
      sendMessage.setData ("Hello", 6);
    }
    
    sendMessage.send();
    
    message_to_send++;
    if (message_to_send >= 3)
        message_to_send = 0;
  }

  /* Receive message if available */
  if (CAN.available ()) {
    receiveMessage = CAN.getMessage ();
    
    /* Receive different data types based on message ID */
    if (receiveMessage.id == small_message_id) {
      b = receiveMessage.getByteFromData ();
      Serial.println (b, DEC);
    } else if (receiveMessage.id == int_message_id) {
      i = receiveMessage.getIntFromData ();
      Serial.println (i);
    } else if (receiveMessage.id == string_message_id) {
      receiveMessage.getData (s);
      Serial.println (s);
    }
  }
}

