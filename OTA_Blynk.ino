#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <Ticker.h>
#include <time.h>
#include <Wire.h>
#include <BlynkSimpleEsp8266.h>

//___________Smart Config_______________

#define PIN_LED 16
#define PIN_BUTTON 0
#define LED_ON() digitalWrite(PIN_LED, HIGH)
#define LED_OFF() digitalWrite(PIN_LED, LOW)
#define LED_TOGGLE() digitalWrite(PIN_LED, digitalRead(PIN_LED) ^ 0x01)

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
  LED_ON();
  in_smartconfig = false;
}


void getWifiInfo(){
  String str =WiFi.psk();    
  char pass[str.length()+1];
  strcpy(pass, str.c_str());
  str =WiFi.SSID();
  char ssid[str.length()+1];
  strcpy(ssid, str.c_str());
  Serial.print("ssid: ");
  Serial.println(ssid);
  Serial.print("pass: ");
  Serial.println(pass);
}

//___________Blynk____________
#define BLYNK_TEMPLATE_ID "TMPLULSEfJ_m"
#define BLYNK_DEVICE_NAME "AGV"
#define BLYNK_AUTH_TOKEN "dorvkYcxlOdLwqXg-M_y4iKeZJEJuPOo"

#define BLYNK_PRINT Serial

char auth[] = BLYNK_AUTH_TOKEN;
// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "NGAN QUYNH 18-21";
char pass[] = "nganquynh012018";

BlynkTimer timer;

BLYNK_WRITE(V0){
  int spd = param.asInt();
  Serial.println(spd);
  Wire.beginTransmission(8); /* begin with device address 8 */
  Wire.write(23);  /* sends hello slave string */
  Wire.write(spd);
  Wire.endTransmission();    /* stop transmitting */
 
 //Wire.requestFrom(8, 1); /* request & read data of size 13 from slave */
 
}
BLYNK_WRITE(V1){
  int dir = param.asInt();
  Serial.println(dir);
  Wire.beginTransmission(8); /* begin with device address 8 */
  Wire.write(24);
  Wire.write(dir);  /* sends hello slave string */
  Wire.endTransmission();    /* stop transmitting */
 
 //Wire.requestFrom(8, 1); /* request & read data of size 13 from slave */
 
}
BLYNK_CONNECTED()
{
  // Change Web Link Button message to "Congratulations!"
  Blynk.setProperty(V3, "offImageUrl", "https://static-image.nyc3.cdn.digitaloceanspaces.com/general/fte/congratulations.png");
  Blynk.setProperty(V3, "onImageUrl",  "https://static-image.nyc3.cdn.digitaloceanspaces.com/general/fte/congratulations_pressed.png");
  Blynk.setProperty(V3, "url", "https://docs.blynk.io/en/getting-started/what-do-i-need-to-blynk/how-quickstart-device-was-made");
}
void myTimerEvent()
{
  // You can send any value at any time.
  // Please don't send more that 10 values per second.
  Blynk.virtualWrite(V2, millis() / 1000);
}

void setup() {
  Serial.begin(9600);
  Serial.setDebugOutput(true);
  //For Smart config
  pinMode(PIN_LED, OUTPUT);
  pinMode(PIN_BUTTON, INPUT);
  ticker.attach(1, tick);
  Serial.println("Setup done");
  //For Blynk
  Wire.begin(D1, D2);
  // Blynk.begin(auth, ssid, pass);
  // You can also specify server:
  Blynk.begin(auth, ssid, pass, "blynk.cloud", 80);
  //Blynk.begin(auth, ssid, pass, IPAddress(192,168,1,100), 8080);
  // Setup a function to be called every second
  timer.setInterval(1000L, myTimerEvent);

}


void loop() {
  if (longPress()) {
    enter_smartconfig();
    Serial.println("Enter smartconfig");
  }
  //Serial.println(digitalRead(PIN_BUTTON));
 
  
  
  
  if (WiFi.status() == WL_CONNECTED && in_smartconfig && WiFi.smartConfigDone()) {
    exit_smart();
    Serial.println("Connected, Exit smartconfig");
    getWifiInfo();
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    Blynk.run();
    timer.run();
    while(Wire.available()){
      int c = Wire.read();
      Serial.println(c);
    }
       
  }
}