/* Blynk credentials */
char auth[] = " f618c7ed551a4bd7a4fb9162d6ad1726";

/* WiFi credentials */
char ssid[] = "SSID";
char pass[] = "123456789";

#define BLYNK_PRINT Serial
#include <SPI.h>
#include <Ethernet.h>
#include <BlynkSimpleEsp8266.h>

#include <OneWire.h>
#include <DallasTemperature.h>

// Data wire is plugged into digital pin 2 on the Arduino
#define ONE_WIRE_BUS 2

// Setup a oneWire instance to communicate with any OneWire device
OneWire oneWire(ONE_WIRE_BUS);  

// Pass oneWire reference to DallasTemperature library
DallasTemperature sensors(&oneWire);
BlynkTimer timer;
int deviceCount = 0;
float tempC;
int count=0;
int motor_time=53;


void sendSensor()
{

  // Send command to all the sensors for temperature conversion
  sensors.requestTemperatures(); 
  
  // Display temperature from each sensor
  for (int i = 0;  i < 1;  i++)
  {

    Serial.print("Sensor");
    Serial.print(" : ");
    tempC = sensors.getTempCByIndex(i);
    Serial.print(tempC);
    Serial.print((char)176);//shows degrees character
    Serial.print("C");
   Blynk.virtualWrite(V6, tempC);
    }

  Serial.println("");
  delay(1000);
}

void setup(void)
{
  sensors.begin();  // Start up the library
  Serial.begin(9600);
  Blynk.begin(auth, ssid, pass);
  timer.setInterval(1000L, sendSensor); 
}

void loop(void)
{ 

  Blynk.run();
  timer.run();
  
}
