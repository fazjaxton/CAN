#include <SPI.h>
#include <CAN.h>

/* This program is a CAN-bus monitor.  It listens on a
 * CAN bus and prints out the IDs and data fields of
 * any CAN messages it hears.  */

CanMessage message;
byte i;

void setup()
{
  CAN.begin(CAN_SPEED_500000);
  CAN.setMode (CAN_MODE_LISTEN_ONLY);
  Serial.begin(115200);
}

void loop()
{
  if (CAN.available()) {
    message = CAN.getMessage ();
    message.print (HEX);
  }
}

