#include "Arduino.h"
#include <Ultrasonic.h>

Ultrasonic ultrasonic(17,16); // (Trig PIN,Echo PIN)

void setup() {
  Serial.begin(115200);
}

void loop()
{
  Serial.print(ultrasonic.Ranging(CM)); // CM or INC
  Serial.println(" cm" );
  delay(100);
}