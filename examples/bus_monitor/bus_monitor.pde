#include <SPI.h>
#include <CAN.h>

/* This program is a CAN-bus monitor.  It listens on a
 * CAN bus and prints out the IDs and data fields of
 * any CAN messages it hears.  */

byte len;
byte data[8];
short id;
byte i;

void setup()
{
  CAN.begin();
  CAN.setMode (CAN_MODE_LISTEN_ONLY);
  Serial.begin(115200);
}

void loop()
{
  if (CAN.available()) {
    id = CAN.getMessageID ();
    Serial.print (id, HEX);
    Serial.print (":");

    len = CAN.receiveData (data, 8);
    for (i=0; i<len; i++) {
      Serial.print (' ');
      Serial.print (data[i], HEX);
    }
    Serial.println ("");
  }
}

