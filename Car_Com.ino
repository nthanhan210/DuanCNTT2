//Khai báo các biến

#define IN1	2
#define IN2	3
#define IN3	4
#define IN4	5
#define POWER 24
#define MAX_SPEED 200 //từ 0-255
#define MIN_SPEED 0
#define line1 44
#define line2 46
#define line3 48
#define line4 50
#define line5 52

#include <Wire.h>
#include <Arduino.h>
int power=0;
int speed=0; //Tốc độ động cơ
int a=0;
int direct=0;  // Hướng quay động cơ
int target_speed=0;
int sen1,sen2,sen3,sen4,sen5=0;
int ln1,ln2,ln3,ln4,ln5=0;
int avg=0;
int calib=0;
long time,stopcnt,traceLine=0;
int calib1,calib2,calib3,calib4=0;
void receiveEvent(int howMany) {
  String lable="";
  while (Wire.available()>1) {
    lable += (char)Wire.read();      /* receive byte as a character */
  }
  int data = Wire.read();
  if (lable=="direct"){
    direct=data;
    Serial.print( " receiveDirect " );
    Serial.println(direct);
  }else if (lable=="speed") {
    target_speed=data;
    Serial.print("receivetgSpeed");
    Serial.println(target_speed);
  }else if (lable=="power") {
    power=data;
    Serial.print("receivePower");
    Serial.println(power);
  }else if (lable=="calib1") {
    calib1=data;
    Serial.print("receiveCalib1");
    Serial.println(calib1);
  }else if (lable=="calib2") {
    calib2=data;
    Serial.print("receiveCalib2");
    Serial.println(calib2);
  }else if (lable=="calib3") {
    calib3=data;
    Serial.print("receiveCalib3");
    Serial.println(calib3);
  }else if (lable=="calib4") {
    calib4=data;
    Serial.print("receiveCalib4");
    Serial.println(calib4);
  }
}

void requestEvent() {
  Serial.print("send");
  Serial.print(power);
  Serial.print(direct);
  Serial.println(speed);
  Wire.write(power);
  Wire.write(direct);
  Wire.write(speed);  /*send string on request */
}
void updateInfo()
{
  
}

void setup() {
  // Setup môi trường
  Wire.begin(8);                /* join i2c bus with address 8 */
  Wire.onReceive(receiveEvent); /* register receive event */
  Wire.onRequest(requestEvent); /* register request event */
  
  Serial.begin(115200);
  pinMode(line1, INPUT);
  pinMode(line2, INPUT);
  pinMode(line3, INPUT);
  pinMode(line4, INPUT);
  pinMode(line5, INPUT);

	pinMode(IN1, OUTPUT);  //Các chân đièu khiển module
	pinMode(IN2, OUTPUT);
	pinMode(IN3, OUTPUT);
	pinMode(IN4, OUTPUT);
  digitalWrite(IN1, LOW); //Tắt tất cả chân
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);


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
void forward(int spd, int deg){
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
  motor_Left(2,spd);
}
//Hàm rẻ phải
void turn_Right(int spd){
  motor_Right(2,spd);
  motor_Left(2,spd);
}

void loop() {
    
  if(power==1){
    sen1=digitalRead(line1);
    sen2=digitalRead(line2);
    sen3=digitalRead(line3);
    sen4=digitalRead(line4);
    sen5=digitalRead(line5);
    ln1= sen1==0 ? -4:0;
    ln2= sen2==0 ? -2:0;
    ln3= sen3==0 ?  0:0;
    ln4= sen4==0 ?  2:0;
    ln5= sen5==0 ?  4:0;
    
    
    if ((sen1+sen2+sen3+sen4+sen5)!=5){
      avg = (ln1+ln2+ln3+ln4+ln5)/(5-(sen1+sen2+sen3+sen4+sen5));
      time=millis();
      stopcnt=0;
      traceLine=0;
    }else{
      stopcnt=millis()-time;

      if(stopcnt > 50){
        speed=0;        
        backward(80);
        delay(100);
      }      
    }
   
    switch (avg) {
      case 1:
        calib = calib1;
        break;
      case -1:
        calib = calib1;
        break;
      case 2:
        calib = calib2;
        break;
      case -2:
        calib = calib2;
        break;
      case 3:
        calib = calib3;
        break;
      case -3:
        calib = calib3;
        break;
      case 4:
        calib = calib4;
        break;
      case -4:
        calib = calib4;
        break;
      default:
        calib=0;
        break;
    }

    if(speed > 0 && (speed-calib)<75){
      calib = speed;
    }
    Serial.println(calib);
    
    if (speed!=0){
      Serial.println(speed);
      if(avg>0){
        motor_Right(2,speed-calib);
        motor_Left(2,speed);
      }else if (avg<0) {
        motor_Right(2,speed);
        motor_Left(2,speed-calib);
      }else{
        motor_Right(2,speed);
        motor_Left(2,speed);
      }
      
    }
      




    // if(target_speed>0 && speed==0){
    //   speed=75;
    // }else if(target_speed==0 && speed-5<75){
    //   speed=0;
    // }else if(speed<target_speed-5){
    //   speed+=5;
    // }else if(speed>target_speed+5){
    //   speed-=5;
    // }else{
      speed = target_speed;
  //  }
  }else{
    speed=0;
    stop();
  }
  if(speed<150){
    a=150;
  }else if(speed<200){
    a=100;
  }else{
    a=50;
  }
  
  
 

  // delay(a);
  // Serial.print(sen1);Serial.print(sen2);Serial.print(sen3);Serial.print(sen4);Serial.print(sen5);Serial.print("--");Serial.print(avg);
  // Serial.print((sen1+sen2+sen3+sen4+sen5));Serial.print((sen1+sen2+sen3+sen4+sen5));

  Serial.println();


}
