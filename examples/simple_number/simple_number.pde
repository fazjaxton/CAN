#include <SPI.h>
#include <CAN.h>

/* This program demonstrates simple use of the CAN library
 * by sending a single number that increments each time. */

/* If you don't have two CAN shields, this program can run on
 * just one by setting the mode to CAN_MODE_LOOPBACK instead
 * of CAN_MODE_NORMAL. */

unsigned long last;
unsigned long now;
int i;
int val;

void setup()
{
  CAN.begin();
  CAN.setMode (CAN_MODE_NORMAL);
  Serial.begin(115200);
  
  last = 0;
  i = 0;
}

void loop()
{
  /* Send a new value every second */
  now = millis();
  if (now / 1000 > last) {
    last = now / 1000;
    CAN.sendInt(i);
    i++;
  }
  
  /* Receive data if available */
  if (CAN.available()) {
    val = CAN.receiveInt();
    Serial.println(val);
  }
}

