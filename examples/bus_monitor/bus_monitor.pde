#include <SPI.h>
#include <CAN.h>

/* This program is a CAN-bus monitor.  It listens on a
 * CAN bus and prints out the IDs and data fields of
 * any CAN messages it hears.  */

CanMessage message;
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
    message = CAN.getMessage ();
    Serial.print (message.id, HEX);
    Serial.print (":");

    for (i=0; i<message.len; i++) {
      Serial.print (' ');
      Serial.print (message.data[i], HEX);
    }
    Serial.println ("");
  }
}

