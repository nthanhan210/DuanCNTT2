/*************************************************************

  This is a simple demo of sending and receiving some data.
  Be sure to check out other examples!
 *************************************************************/

// Template ID, Device Name and Auth Token are provided by the Blynk.Cloud
// See the Device Info tab, or Template settings
#include <Wire.h>
#define BLYNK_TEMPLATE_ID "TMPLULSEfJ_m"
#define BLYNK_DEVICE_NAME "AGV"
#define BLYNK_AUTH_TOKEN "dorvkYcxlOdLwqXg-M_y4iKeZJEJuPOo"


// Comment this out to disable prints and save space
#define BLYNK_PRINT Serial


#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

char auth[] = BLYNK_AUTH_TOKEN;

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "NGAN QUYNH 18-21";
char pass[] = "nganquynh012018";

BlynkTimer timer;

// This function is called every time the Virtual Pin 0 state changes
//BLYNK_WRITE(V0)
//{
  // Set incoming value from pin V0 to a variable
  //int value = param.asInt();

  // Update state
  //Blynk.virtualWrite(V1, value);
//}
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
  Blynk.virtualWrite(V2, millis() / 1000);
}

void setup()
{
  // Debug console
  
  Serial.begin(9600);
  Wire.begin(D1, D2);
  Blynk.begin(auth, ssid, pass);
  // You can also specify server:
  //Blynk.begin(auth, ssid, pass, "blynk.cloud", 80);
  //Blynk.begin(auth, ssid, pass, IPAddress(192,168,1,100), 8080);

  // Setup a function to be called every second
  timer.setInterval(1000L, myTimerEvent);
}

void loop()
{
  Blynk.run();
  timer.run();
  while(Wire.available()){
    int c = Wire.read();
    Serial.println(c);
 }
  // You can inject your own code or combine it with other sketches.
  // Check other examples on how to communicate with Blynk. Remember
  // to avoid delay() function!
 
 
}
