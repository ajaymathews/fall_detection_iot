#include "MAX30100.h"
 
MAX30100* pulseOxymeter;
 
void setup() {
  Wire.begin(4,5);
  Serial.begin(9600);
  Serial.println("Pulse oxymeter test!");
 
  pulseOxymeter = new MAX30100();
  pinMode(2, OUTPUT);
 
}
 
void loop() {
  //You have to call update with frequency at least 37Hz. But the closer you call it to 100Hz the better, the filter will work.
  pulseoxymeter_t result = pulseOxymeter->update();
  
  if( result.pulseDetected == true )
  { 
    Serial.print( "BPM: " );
    Serial.print( result.heartBPM );

    Serial.print( "SaO2: " );
    Serial.print( result.SaO2 );
    Serial.println( "%" );
  }
}
