//SCL-D1
//SDA-D2

#include <SparkFun_ADXL345.h>         // SparkFun ADXL345 Library
//ADXL345 adxl = ADXL345(10);   
ADXL345 adxl = ADXL345();         

void setup(){
  
  Serial.begin(9600);                 // Start the serial terminal
  Serial.println("System Start");
  Serial.println();
  pinMode(2,OUTPUT);
  pinMode(3,OUTPUT);
  adxl.powerOn();                     // Power on the ADXL345

  adxl.setRangeSetting(16);           // Give the range settings
                                      // Accepted values are 2g, 4g, 8g or 16g
                                      // Higher Values = Wider Measurement Range
                                      // Lower Values = Greater Sensitivity

  adxl.setSpiBit(0);                  // Configure the device to be in 4 wire SPI mode when set to '0' or 3 wire SPI mode when set to 1
                                      // Default: Set to 1
                                      // SPI pins on the ATMega328: 11, 12 and 13 as reference in SPI Library 
   
  adxl.setActivityXYZ(1, 0, 0);       // Set to activate movement detection in the axes "adxl.setActivityXYZ(X, Y, Z);" (1 == ON, 0 == OFF)
  adxl.setActivityThreshold(75);      // 62.5mg per increment   // Set activity   // Inactivity thresholds (0-255)
 
  adxl.setInactivityXYZ(1, 0, 0);     // Set to detect inactivity in all the axes "adxl.setInactivityXYZ(X, Y, Z);" (1 == ON, 0 == OFF)
  adxl.setInactivityThreshold(75);    // 62.5mg per increment   // Set inactivity // Inactivity thresholds (0-255)
  adxl.setTimeInactivity(10);         // How many seconds of no activity is inactive?

  adxl.setTapDetectionOnXYZ(0, 0, 1); // Detect taps in the directions turned ON "adxl.setTapDetectionOnX(X, Y, Z);" (1 == ON, 0 == OFF)

  adxl.setTapThreshold(50);           
  adxl.setTapDuration(15);            
  adxl.setDoubleTapLatency(80);       
  adxl.setDoubleTapWindow(200);       
 
  adxl.setFreeFallThreshold(9);    
  adxl.setFreeFallDuration(10);      
   
  adxl.InactivityINT(0);
  adxl.ActivityINT(0);
  adxl.FreeFallINT(1);
  adxl.doubleTapINT(0);
  adxl.singleTapINT(0);
}

void loop()
{
  int x,y,z;   
  adxl.readAccel(&x, &y, &z); 
  Serial.println("insidelooop");        
  ADXL_ISR();
}

void ADXL_ISR()
{
 byte interrupts = adxl.getInterruptSource();
  if(adxl.triggered(interrupts, ADXL345_FREE_FALL))
  {
    Serial.println("*** FREE FALL ***");
    digitalWrite(2,HIGH);
    digitalWrite(3,HIGH);
    delay(1000);
  }   
  else
  {
   digitalWrite(2,LOW);
   digitalWrite(3,LOW);
  }
}
