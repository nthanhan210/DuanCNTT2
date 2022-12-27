//Khai báo các biến

#define IN1	2
#define IN2	3
#define IN3	4
#define IN4	5
#define POWER 24
#define MAX_SPEED 200 //từ 0-255
#define MIN_SPEED 0
#include <Wire.h>
#include <Arduino.h>
int speed=0; //Tốc độ động cơ
int direct=0;  // Hướng quay động cơ

void receiveEvent(int howMany) {
  String lable="";
  while (Wire.available()>1) {
    lable += (char)Wire.read();      /* receive byte as a character */
  }
  int data = Wire.read();
  if (lable=="direct"){
    direct=data;
  }else if (lable=="speed") {
    speed=data;
  }
  Serial.print("Speed");
  Serial.print(speed);
  Serial.print( " Direct " );
  Serial.println(direct);
    

}
void requestEvent() {
  Wire.write(direct);
  Wire.write(speed);  /*send string on request */
}


void setup() {
  // Setup môi trường
  Wire.begin(8);                /* join i2c bus with address 8 */
  Wire.onReceive(receiveEvent); /* register receive event */
  Wire.onRequest(requestEvent); /* register request event */
  Serial.begin(115200);
  pinMode(POWER, INPUT_PULLUP);  //Nút nhấn tắt mở
  pinMode(22, OUTPUT);
	pinMode(IN1, OUTPUT);  //Các chân đièu khiển module
	pinMode(IN2, OUTPUT);
	pinMode(IN3, OUTPUT);
	pinMode(IN4, OUTPUT);

  digitalWrite(IN1, LOW); //Tắt tất cả chân
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
  digitalWrite(22, LOW);
}


// Hàm điều khiển motor phải
void motor_Right(int dir=-1, int spd=0) { //speed: từ 0 - MAX_SPEED
	//spd = constrain(spd, MIN_SPEED, MAX_SPEED);//đảm báo giá trị nằm trong một khoảng từ 0 - MAX_SPEED - http://arduino.vn/reference/constrain
	if (dir == 2){
    digitalWrite(IN3, LOW);    
    analogWrite(IN4,spd);
    
  }else if (dir==0) {
    digitalWrite(IN4, LOW);
    analogWrite(IN3, spd);
  }else{
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, LOW);
  }
  
}

// Hàm điều khiển motor trái
void motor_Left(int dir=-1, int spd=0) { //speed: từ 0 - MAX_SPEED
	//spd = constrain(spd, MIN_SPEED, MAX_SPEED);//đảm báo giá trị nằm trong một khoảng từ 0 - MAX_SPEED - http://arduino.vn/reference/constrain
	if (dir == 2){
    digitalWrite(IN1, LOW);    
    analogWrite(IN2,spd);
    
  }else if (dir==0) {
    digitalWrite(IN2, LOW);
    analogWrite(IN1, spd);
  }else{
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, LOW);
  }
}

// Hàm di chuyển tiến
void forward(int spd){
  motor_Right(2,spd);
  motor_Left(2,spd);
}
// Hàm di chuyển lùi
void backward(int spd){
  motor_Right(0,spd);
  motor_Left(0,spd);
}
//Hàm dừng lại
void stop(){
  motor_Right(1);
  motor_Left(1);
}

//Hàm rẻ trái
void turn_Left(int spd){
  motor_Right(2,spd);
  motor_Left(0,spd);
}
//Hàm rẻ phải
void turn_Right(int spd){
  motor_Right(0,spd);
  motor_Left(2,spd);
}

void loop() {  
  if (direct == 1){
    forward(speed);
  }else if (direct == 2){
    turn_Left(speed);
  }else if (direct == 3){
    backward(speed);
  }else if (direct == 4){
    turn_Right(speed);
  }else{
    stop();
  }
  
}
