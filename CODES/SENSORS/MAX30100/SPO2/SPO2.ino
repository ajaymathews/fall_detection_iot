#include <Arduino.h>
#include <math.h>
#include <Wire.h>
#include "MAX30100.h"
MAX30100*pulseOxymeter;

int mode = 0;

void setup() {
  Wire.begin();
  Serial.begin(9600);
  Serial.println("Up for SaO2");
  Serial.println("Pulse oxymeter test!");

  pulseOxymeter = new MAX30100( DEFAULT_OPERATING_MODE, DEFAULT_SAMPLING_RATE, DEFAULT_LED_PULSE_WIDTH, DEFAULT_IR_LED_CURRENT, true, false);
  pinMode(2, OUTPUT);
}

void loop() {
  pulseoxymeter_t result = pulseOxymeter->update();
  
  if ((analogRead(0)>130) && (analogRead(0)<160))
  {
    Serial.print( "SaO2: " );
    mode = 1;
  }
 else if ((analogRead(0)>130) && (analogRead(0)<160)) {

    Serial.print( "BPM: " );
    mode = 2;

  if (result.pulseDetected == true)
  {
    if (mode == 1) {

      Serial.print( "SaO2: ");
      Serial.print( result.SaO2 );
      Serial.print( "%" );

    } else if (mode == 2) {

      Serial.print( "BPM: ");
      Serial.print(result.heartBPM);

    }
  }
  delay(10);
  digitalWrite( 2, !digitalRead(2) );

}
}
