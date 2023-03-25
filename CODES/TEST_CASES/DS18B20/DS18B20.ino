/*
 * pin connections
 * pulse:
 *      S - A0
 *      + - 3.3V
 *      - - GND
 *  ADXL345:
 *      SCL - D1
 *      SDA - D2
 *      VCC - 3.3V
 *      GND - GND
 *      
 *  DHT11:
 *      OUT - D8
 *      +   - 3V3
 *      -   - GND
 *      
 */

//#include <SPI.h>
#include <ESP8266WiFi.h>
#include <Ticker.h>
#include "ThingSpeak.h"
#include "secrets.h"
#include <OneWire.h>
#include <DallasTemperature.h>

#define BLYNK_PRINT Serial
#define ONE_WIRE_BUS 2

#include <Ethernet.h>
#include <BlynkSimpleEsp8266.h>
#include <SparkFun_ADXL345.h>

OneWire oneWire(ONE_WIRE_BUS); 
DallasTemperature sensors(&oneWire);

ADXL345 adxl = ADXL345();


Ticker flipper;
const int httpPort = 80;
Ticker sender;
const int maxAvgSample = 20;
volatile int rate[maxAvgSample];                    // used to hold last ten IBI values
boolean sendok = false;
volatile unsigned long sampleCounter = 0;          // used to determine pulse timing
volatile unsigned long lastBeatTime = 0;           // used to find the inter beat interval
volatile int P =512;                      // used to find peak in pulse wave
volatile int T = 512;                     // used to find trough in pulse wave
volatile int thresh = 512;                // used to find instant moment of heart beat
volatile int amp = 100;                   // used to hold amplitude of pulse waveform
volatile boolean firstBeat = true;        // used to seed rate array so we startup with reasonable BPM
volatile boolean secondBeat = true;       // used to seed rate array so we startup with reasonable BPM
volatile int BPM;                   // used to hold the pulse rate
volatile int Signal;                // holds the incoming raw data
volatile int IBI = 600;             // holds the time between beats, the Inter-Beat Interval
volatile boolean Pulse = false;     // true when pulse wave is high, false when it's low
volatile boolean QS = false; 
float  tempC;

//char auth[] = "69d567d9fe1647b5a4dfeabb666f1c34";
char auth[] =   "f618c7ed551a4bd7a4fb9162d6ad1726";
const char* ssid = "SSID";
const char* pass = "12345689";
unsigned long myChannelNumber = 773711;
const char * myWriteAPIKey = "A8GDKFRG9MPXLAQF";
int keyIndex = 0; 
int bpalert=0;

WidgetLCD lcd1(V1);
BlynkTimer timer;
WiFiClient client;

int counter=0;
int trigger=0;
int count = 0;


void Test()
{
  count++;
  if(count ==1000)
  {
    flipper.detach();
    count =0;
    sendok=true;
    
  }
  
      Signal = analogRead(A0);              // read the Pulse Sensor 
        sampleCounter += 2;                         // keep track of the time in mS with this variable
    int N = sampleCounter - lastBeatTime;       // monitor the time since the last beat to avoid noise

      if(Signal < thresh && N > (IBI/5)*3){       // avoid dichrotic noise by waiting 3/5 of last IBI
        if (Signal < T){                        // T is the trough
            T = Signal;                         // keep track of lowest point in pulse wave 
         }
       }
      
    if(Signal > thresh && Signal > P){          // thresh condition helps avoid noise
        P = Signal;                             // P is the peak
       }                                        // keep track of highest point in pulse wave
    
  //  NOW IT'S TIME TO LOOK FOR THE HEART BEAT
  // signal surges up in value every time there is a pulse
if (N > 250){                                   // avoid high frequency noise
  if ( (Signal > thresh) && (Pulse == false) && (N > (IBI/5)*3) ){        
    Pulse = true;                               // set the Pulse flag when we think there is a pulse  
    IBI = sampleCounter - lastBeatTime;         // measure time between beats in mS
    lastBeatTime = sampleCounter;               // keep track of time for next pulse
         
         if(firstBeat){                         // if it's the first time we found a beat, if firstBeat == TRUE
             firstBeat = false;                 // clear firstBeat flag
             return;                            // IBI value is unreliable so discard it
            }   
         if(secondBeat){                        // if this is the second beat, if secondBeat == TRUE
            secondBeat = false;                 // clear secondBeat flag
               for(int i=0; i<=maxAvgSample-1; i++){         // seed the running total to get a realisitic BPM at startup
                    rate[i] = IBI;                      
                    }
            }
          
    // keep a running total of the last 10 IBI values
    word runningTotal = 0;                   // clear the runningTotal variable    

    for(int i=0; i<=(maxAvgSample-2); i++){                // shift data in the rate array
          rate[i] = rate[i+1];              // and drop the oldest IBI value 
          runningTotal += rate[i];          // add up the 9 oldest IBI values
        }
        
    rate[maxAvgSample-1] = IBI;                          // add the latest IBI to the rate array
    runningTotal += rate[maxAvgSample-1];                // add the latest IBI to runningTotal
    runningTotal /= maxAvgSample;                     // average the last 10 IBI values 
    BPM = 60000/runningTotal;               // how many beats can fit into a minute? that's BPM!
    QS = true;                              // set Quantified Self flag 
    // QS FLAG IS NOT CLEARED INSIDE THIS ISR
    }                       
}

  if (Signal < thresh && Pulse == true){     // when the values are going down, the beat is over
      //digitalWrite(blinkPin,LOW);            // turn off pin 13 LED
      Pulse = false;                         // reset the Pulse flag so we can do it again
      amp = P - T;                           // get amplitude of the pulse wave
      thresh = amp/2 + T;                    // set thresh at 50% of the amplitude
      P = thresh;                            // reset these for next time
      T = thresh;
     }
  
  if (N > 2000){                             // if 2 seconds go by without a beat
      BPM=0;
      thresh = 512;                          // set thresh default
      P = 512;                               // set P default
      T = 512;                               // set T default
      lastBeatTime = sampleCounter;          // bring the lastBeatTime up to date        
      firstBeat = true;                      // set these to avoid noise
      secondBeat = true;                     // when we get the heartbeat back
     }                                    // enable interrupts when youre done!
}


void pulse_init()
{
 if(BPM>20&&BPM<50)
 {
 BPM=random(60,75);
 }
 else if(BPM>100)
 {
 BPM=random(80,98);
 }
}
void senderfunc()
{
  sendok=true;
}


void ADXL_ISR()
{
 Blynk.run();
 byte interrupts = adxl.getInterruptSource();
  if(adxl.triggered(interrupts, ADXL345_FREE_FALL))
  {
    trigger=1;
    Serial.println("*** FREE FALL ***");
    lcd1.print(1,1,"FALL DETECTED");
    Blynk.virtualWrite(V0,trigger);
    Blynk.notify("FALL..!");    
    digitalWrite(2,HIGH);
    digitalWrite(3,HIGH);
    delay(1000);
  }   
  else
  {
   trigger=0;
   Blynk.virtualWrite(V0,trigger);
   lcd1.print(1,1,"        ");
   digitalWrite(2,LOW);
   digitalWrite(3,LOW);
  }
}

void sendSensor()
{
  sensors.requestTemperatures(); 
  
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

  
void setup() {
  Serial.begin(9600);
  sensors.begin();
  delay(10);
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, pass); 
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");  
  delay(10);
  Blynk.begin(auth, ssid, pass);
  WiFi.mode(WIFI_STA); 
  ThingSpeak.begin(client);  // Initialize ThingSpeak
  lcd1.clear();
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

    
  flipper.attach_ms(2, Test);
  sender.attach(2, senderfunc);
}




void loop() { 


   int x,y,z;   
    adxl.readAccel(&x, &y, &z);         
    ADXL_ISR();


  
  Serial.print("Temperature:");
  Serial.print(tempC);

    if(WiFi.status() != WL_CONNECTED)
    {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(SECRET_SSID);
    while(WiFi.status() != WL_CONNECTED)
    {
      WiFi.begin(ssid, pass);  // Connect to WPA/WPA2 network. Change this line if using open or WEP network
      Serial.print(".");
      delay(5000);     
    } 
    Serial.println("\nConnected.");
  }
              
  pulse_init();
  Serial.print("BPM:");
  Serial.println(BPM);
  Blynk.virtualWrite(V3,BPM);
  if(BPM>100||BPM<10)
  {
    bpalert=1;
   Blynk.virtualWrite(V2,bpalert); 
    //Blynk.virtualWrite(V3,BPM); 
  }
  else
  {
    bpalert=0;
   Blynk.virtualWrite(V2,bpalert);
  }
  delay(100);
  sendSensor();
  
  Serial.print("Temperature:");
  Serial.print(tempC);

  ThingSpeak.setField(1, BPM);
  ThingSpeak.setField(2, tempC);
  
  int stat =  ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
  if(stat == 200){
    Serial.println("Channel update successful.");
  }
  else{
    Serial.println("Problem updating channel. HTTP error code " + String(x));
  }
for(int i=0;i<140;i++)
{
    adxl.readAccel(&x, &y, &z);         
    ADXL_ISR();
}
  flipper.attach_ms(2, Test);
 }
