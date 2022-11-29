#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <Ticker.h>
#include <time.h>
#include <Wire.h>
#include <BlynkSimpleEsp8266.h>
#include <EEPROM.h>

//___________Smart Config_______________

#define PIN_LED 16
#define PIN_BUTTON 0
#define LED_ON() digitalWrite(PIN_LED, HIGH)
#define LED_OFF() digitalWrite(PIN_LED, LOW)
#define LED_TOGGLE() digitalWrite(PIN_LED, digitalRead(PIN_LED) ^ 0x01)

Ticker ticker;

String wifiSSID;
String wifiPass;


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




//___________Blynk____________
#define BLYNK_TEMPLATE_ID "TMPLULSEfJ_m"
#define BLYNK_DEVICE_NAME "AGV"
#define BLYNK_AUTH_TOKEN "dorvkYcxlOdLwqXg-M_y4iKeZJEJuPOo"

#define BLYNK_PRINT Serial

char auth[] = BLYNK_AUTH_TOKEN;



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
BLYNK_WRITE(V2){
  int t15 = param.asInt();
  Serial.println(t15);
  Wire.beginTransmission(8); /* begin with device address 8 */
  Wire.write(15);
  Wire.write(t15);  /* sends hello slave string */
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
  Serial.println("1");
  Serial.setDebugOutput(true);
  EEPROM.begin(512);
  int add=0;
  int startAdd=0;
  int ssid_len = int(EEPROM.read(add));
  add++;  
  startAdd=add;
  wifiSSID="";
  for(int i=0; i<ssid_len;i++){
    wifiSSID = wifiSSID + char(EEPROM.read(startAdd+i));
    add++;
  }

  int pass_len = int(EEPROM.read(add));
  add++;
  startAdd=add;
  wifiPass="";
  for(int i=0; i<pass_len;i++){
    wifiPass = wifiPass + char(EEPROM.read(startAdd+i));
    add++;
  }
  char ssid[wifiSSID.length()+1];
  char pass[wifiPass.length()+1];
  strcpy(ssid, wifiSSID.c_str());
  strcpy(pass, wifiPass.c_str());

  Serial.print("EEP ssid: ");
  Serial.println(wifiSSID);
  Serial.print("EEP pass: ");
  Serial.println(wifiPass);
  Serial.print("length ");
  Serial.println(wifiPass.length());
  
  
  
  //For Smart config
  pinMode(PIN_LED, OUTPUT);
  pinMode(PIN_BUTTON, INPUT);
  ticker.attach(1, tick);
  
  Wire.begin(D1, D2);
  Serial.println("2");
  //For Blynk
 
  // Blynk.begin(auth, ssid, pass);
  // You can also specify server:
  Serial.println(sizeof(ssid));
  
  if( wifiSSID !="" && wifiPass.length() <= 64){
    Serial.println("3");
    Blynk.begin(auth, ssid, pass);
    for (int i=0; i<1024;i++){

    }
    //Blynk.begin(auth, ssid, pass, IPAddress(192,168,1,100), 8080);
    // Setup a function to be called every second
    timer.setInterval(1000L, myTimerEvent);
    Serial.println("4");
  }
    
  EEPROM.end();
  Serial.println("Setup done");
}

void getWifiInfo(){
  int add=0;
  int startAdd=0;
  wifiSSID =WiFi.SSID();
  wifiPass =WiFi.psk();

  EEPROM.write(add,wifiSSID.length());
  add++;
  startAdd=add;
  for(int i=0; i<wifiSSID.length();i++){
    EEPROM.write(startAdd+i,wifiSSID[i]);
    add++;
  }
  EEPROM.write(add,wifiPass.length());
  add++;
  startAdd=add;
  for(int i=0; i<wifiPass.length();i++){
    EEPROM.write(startAdd+i,wifiPass[i]);
    add++;
  }
  EEPROM.commit();
}

void loop() {
  if (longPress() && !Blynk.connected()) {
    enter_smartconfig();
    Serial.println("Enter smartconfig");
  }
  
  if (WiFi.status() == WL_CONNECTED && in_smartconfig && WiFi.smartConfigDone()) {
    exit_smart();
    Serial.println("Connected, Exit smartconfig");
    getWifiInfo();
  }
  
  if (WiFi.status() == WL_CONNECTED && Blynk.connected()) {
    Blynk.run();
    timer.run();
    while(Wire.available()){
      int c = Wire.read();
      Serial.println(c);
    }
    Serial.println("cncncn");
  }
}