//Khai báo macro và thư viện
#define PIN_LED 16          // Pin out LED
#define PIN_BUTTON 0        // Pin out nút FLASH
#define LED_ON() digitalWrite(PIN_LED, LOW)         //bật LED
#define LED_OFF() digitalWrite(PIN_LED, HIGH)       //tắt LED

#define BLYNK_TEMPLATE_ID "TMPLULSEfJ_m"              // các thông tin xác thực với Server Blynk
#define BLYNK_DEVICE_NAME "AGV"
#define BLYNK_AUTH_TOKEN "dorvkYcxlOdLwqXg-M_y4iKeZJEJuPOo"
#define BLYNK_PRINT Serial

#include <Wire.h>                                      //Thư viện giao tiếp I2C 
#include <ESP8266WiFi.h>
#include <Ticker.h>
#include <time.h>
#include <BlynkSimpleEsp8266.h>
//==================================================================================

//Các hàm cho Smart Config
Ticker ticker;     // bộ lặp hàm theo thời gian

bool longPress()     //kiểm tra nhấn nút 3s
{
  static int lastPress = 0;
  if (millis() - lastPress > 3000 && digitalRead(PIN_BUTTON) == 0) {
    return true;
  } else if (digitalRead(PIN_BUTTON) == 1) {
    lastPress = millis();
  }
  return false;
}
void tick()       //Hàm chuyển trạng thái LED
{
  //toggle state
  int state = digitalRead(PIN_LED);  // get the current state of GPIO1 pin
  digitalWrite(PIN_LED, !state);     // set pin to the opposite state
}

bool in_smartconfig = false;        //flag trạng thái smart config
void enter_smartconfig()            //bật chế độ smart config
{
  if (in_smartconfig == false) {
    in_smartconfig = true;
    ticker.attach(0.1, tick);         // LED chớp nhanh
    WiFi.beginSmartConfig();
  }
}
void exit_smart()                   //thoát chế độ smart config
{
  ticker.detach();
  LED_OFF();
  in_smartconfig = false;
}

//========================================================================================

//Khởi tạo các biến và hàm cho Blynk
char auth[] = BLYNK_AUTH_TOKEN;           //Blynk auth token     
BlynkTimer timer;                         //Blynk timer
int speed,tmp_speed=0;                    //tốc độ động cơ
int power = 0;                            //bật tắt động cơ
int spd[]={0,75,80,85,90,95,100};         //mảng xung tương ứng với từng mức tốc độ
char ssid[100];                           //thông tin SSID - PASSWORD
char pass[100];

BLYNK_WRITE(V0){                    // nhận tín hiệu điều khiển V0 từ Server
  power = param.asInt();            // lấy giá trị V0
  Serial.println(power);
  Wire.beginTransmission(8);        //truyền qua I2C địa chỉ 8
  Wire.write("power");              //lable cho data
  Wire.write(power);                //data
  Wire.endTransmission();           //dừng truyền
}
BLYNK_WRITE(V1){                    // nhận tín hiệu điều khiển V1 từ Server
  speed = param.asInt();            // lấy giá trị V1
  Serial.println(speed);
  Wire.beginTransmission(8);        //truyền qua I2C địa chỉ 8
  Wire.write("speed");              //lable cho data
  Wire.write(spd[speed]);           //data
  Wire.endTransmission();           //dừng truyền
}

// Hàm được gọi mỗi khi thiết bị kết nối tới Blynk.Cloud
BLYNK_CONNECTED()
{
  // Change Web Link Button message to "Congratulations!"
  Blynk.setProperty(V3, "offImageUrl", "https://static-image.nyc3.cdn.digitaloceanspaces.com/general/fte/congratulations.png");
  Blynk.setProperty(V3, "onImageUrl",  "https://static-image.nyc3.cdn.digitaloceanspaces.com/general/fte/congratulations_pressed.png");
  Blynk.setProperty(V3, "url", "https://docs.blynk.io/en/getting-started/what-do-i-need-to-blynk/how-quickstart-device-was-made");
}

// Hàm được gọi mỗi 1 giây
void myTimerEvent()       //update các thông tin điều khiển xe lên Server
{
  Wire.requestFrom(8,2);
  Blynk.virtualWrite(V3,tmp_speed);
  Blynk.virtualWrite(V0,power);
  for(int i=0 ;i<7;i++){
    if (spd[i]==tmp_speed){
      Blynk.virtualWrite(V1,i);
      break;
    }
  }
}

//=================================================================================================

void setup() {
  
  Wire.begin(D1,D2);          // Khởi tạo giao tiếp I2C

  Serial.begin(115200);       // Khởi tạo Serial
  Serial.setDebugOutput(true);

  pinMode(PIN_LED, OUTPUT);       //đặt mode cho pinout
  pinMode(PIN_BUTTON, INPUT);
  

  Serial.println(WiFi.SSID());
  Serial.println(WiFi.psk());
  
  Serial.printf("Connection status: %d\n", WiFi.status());
  if(WiFi.SSID()!=""){        // Kết nối tới SSID đã lưu trước đó
    WiFi.begin();
    
    ticker.attach(0.1, tick);       //LED chớp nhanh
    int cntimer=millis();
    // Đợi 10s để kết nối
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
      // Nếu kết nối thành công, Khởi tạo kết nối tới Blynk
      Blynk.begin(auth,ssid,pass);    
    }else {
      // Nếu kết nối không thành công, LED chớp chậm
      ticker.attach(1, tick);
      WiFi.disconnect();
    }    
  }else {
    ticker.attach(1, tick);
  }
  timer.setInterval(1000L, myTimerEvent); //đặt lời gọi cho hàm cập nhật thông tin mỗi 1s
  Serial.println("Setup done");
}

void loop() {
  if (longPress()) {      // kiểm tra nhấn nút bật Smart Config
    enter_smartconfig();
    Serial.println("Enter smartconfig");
  }
  //Thoát chế độ Smart Config sau khi kết nối tới Wifi
  if (WiFi.status() == WL_CONNECTED && in_smartconfig && WiFi.smartConfigDone()) {
    exit_smart();    
    WiFi.hostname("esp");
    WiFi.setAutoReconnect(true);     
    Serial.println("Connected, Exit smartconfig");
    Serial.printf("Connection status: %d\n", WiFi.status());
    Blynk.begin(auth,ssid,pass);        // Khởi tạo kết nối tới Blynk bằng token xác thực và thông tin Wifi
  }
  
  if (WiFi.status() == WL_CONNECTED) {    //Khi đã kết nối được Wifi
    LED_ON();                     //LED sáng
    timer.run();                  //Khởi chạy hàm cập nhật
    if(Blynk.connected()) {       // Khởi chạy Blynk
      Blynk.run();
    }else {
    Blynk.connect();
    }
    while(Wire.available()>0){    // liên tục update thông tin speed và power từ Arduino
      power = Wire.read();
      tmp_speed = Wire.read();    
      Serial.print("receive");
      Serial.print(power) ;
      Serial.println(tmp_speed) ;
    } 
  }else {
    if(!ticker.active()){
      ticker.attach(1, tick);  // Flash chậm khi không có Wifi
    }        
  }
}