#define PIN_LED 16
#define PIN_BUTTON 0
#define LED_ON() digitalWrite(PIN_LED, LOW)
#define LED_OFF() digitalWrite(PIN_LED, HIGH)
#define LED_TOGGLE() digitalWrite(PIN_LED, digitalRead(PIN_LED) ^ 0x01)
#define BLYNK_TEMPLATE_ID "TMPLULSEfJ_m"
#define BLYNK_DEVICE_NAME "AGV"
#define BLYNK_AUTH_TOKEN "dorvkYcxlOdLwqXg-M_y4iKeZJEJuPOo"
#define BLYNK_PRINT Serial
#include <Wire.h>
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <Ticker.h>
#include <time.h>
#include <BlynkSimpleEsp8266.h>
//==================================================================================


Ticker ticker;

bool longPress()
{
  static int lastPress = 0;
  if (millis() - lastPress > 3000 && digitalRead(PIN_BUTTON) == 0) {
    return true;
  } else if (digitalRead(PIN_BUTTON) == 1) {
    lastPress = millis();
  }
  return false;
}

void tick()
{
  //toggle state
  int state = digitalRead(PIN_LED);  // get the current state of GPIO1 pin
  digitalWrite(PIN_LED, !state);     // set pin to the opposite state
}

bool in_smartconfig = false;
void enter_smartconfig()
{
  if (in_smartconfig == false) {
    in_smartconfig = true;
    ticker.attach(0.1, tick);
    WiFi.beginSmartConfig();
  }
}
void exit_smart()
{
  ticker.detach();
  LED_OFF();
  in_smartconfig = false;
}

//========================================================================================


char auth[] = BLYNK_AUTH_TOKEN;
BlynkTimer timer;
int spd=0;
int dir=1;
int power=0;
char ssid[100];
char pass[100];

BLYNK_WRITE(V0){
  int speed = param.asInt();
  Serial.println(speed);
  Wire.beginTransmission(8);
  Wire.write("speed");
  Wire.write(speed==0? 0:map(speed,1,10,75,255));
  Wire.endTransmission();
 //Wire.requestFrom(8, 1); /* request & read data of size 13 from slave */
}
BLYNK_WRITE(V2){
  int t15 = param.asInt();
  Serial.println(t15);
  Wire.beginTransmission(8);
  Wire.write("power");
  Wire.write(t15);
  Wire.endTransmission();
 //Wire.requestFrom(8, 1); /* request & read data of size 13 from slave */
}
BLYNK_WRITE(V1){
  int direct = param.asInt();
  Serial.println(direct);
  Wire.beginTransmission(8);
  Wire.write("direct");
  Wire.write(direct);
  Wire.endTransmission();
  
 
 //Wire.requestFrom(8, 1); /* request & read data of size 13 from slave */
 
}
// This function is called every time the device is connected to the Blynk.Cloud
BLYNK_CONNECTED()
{
  // Change Web Link Button message to "Congratulations!"
  Blynk.setProperty(V3, "offImageUrl", "https://static-image.nyc3.cdn.digitaloceanspaces.com/general/fte/congratulations.png");
  Blynk.setProperty(V3, "onImageUrl",  "https://static-image.nyc3.cdn.digitaloceanspaces.com/general/fte/congratulations_pressed.png");
  Blynk.setProperty(V3, "url", "https://docs.blynk.io/en/getting-started/what-do-i-need-to-blynk/how-quickstart-device-was-made");
}

// This function sends Arduino's uptime every second to Virtual Pin 2.
void myTimerEvent()
{
  // You can send any value at any time.
  // Please don't send more that 10 values per second.
  Wire.requestFrom(8,3);
  Blynk.virtualWrite(V3,spd);
  Blynk.virtualWrite(V1,dir);
}

//=================================================================================================



//===============================================================

void setup() {
  
  Wire.begin(D1,D2);

  Serial.begin(115200);
  Serial.setDebugOutput(true);

  pinMode(PIN_LED, OUTPUT);
  pinMode(PIN_BUTTON, INPUT);
  

  Serial.println(WiFi.SSID());
  Serial.println(WiFi.psk());
  
  Serial.printf("Connection status: %d\n", WiFi.status());
  if(WiFi.SSID()!=""){
    WiFi.begin();
    
    ticker.attach(0.1, tick);
    int cntimer=millis();
    while (WiFi.status() != WL_CONNECTED && (millis()-cntimer)<10000)
    { 
      delay(500);
    }
    if (WiFi.isConnected()){
      
      strcpy(ssid, WiFi.SSID().c_str());
      strcpy(pass, WiFi.psk().c_str());
      ticker.detach();
      LED_OFF();
      Serial.printf("\nConnection status: %d\n", WiFi.status());
      Blynk.begin(auth,ssid,pass);    
    }else {
      ticker.attach(1, tick);
      WiFi.disconnect();
    }    
  }else {
    ticker.attach(1, tick);
    
  }
  timer.setInterval(1000L, myTimerEvent);
  Serial.println("Setup done");
}

void loop() {
  if (longPress()) {
    enter_smartconfig();
    Serial.println("Enter smartconfig");
  }
  if (WiFi.status() == WL_CONNECTED && in_smartconfig && WiFi.smartConfigDone()) {
    exit_smart();    
    WiFi.hostname("esp");
    WiFi.setAutoReconnect(true);     
    Serial.println("Connected, Exit smartconfig");
    Serial.printf("Connection status: %d\n", WiFi.status());
    Blynk.begin(auth,ssid,pass);
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    //Serial.println(Blynk.connected());
    LED_ON();
    timer.run();
    if(Blynk.connected()) {
      Blynk.run();
    }else {
    Blynk.connect();
    }
    while(Wire.available()>0){
      power = Wire.read();
      dir = Wire.read();
      spd = Wire.read();
      Serial.println(dir);
      Serial.println(spd);      
    }
    
    
  }else {
    
    if(!ticker.active()){
      ticker.attach(1, tick);
    }        
  }
} 
