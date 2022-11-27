#include <Wire.h>
#include <string.h>
#include <SoftwareSerial.h>
#include <Servo.h>
#include <NewPing.h>
#define IN1	36
#define IN2	3
#define IN3	2
#define IN4	38
#define IN8	39
#define IN7	5
#define IN6	4
#define IN5	37
#define MAX_SPEED 200 //từ 0-255
#define MIN_SPEED 0
#define trig 30
#define echo 31
#define max_dis 50
int spd=0;
int direct=1;
int dis=100;
Servo servo_motor;
NewPing sonar(trig,echo,max_dis);

void setup()
{
  Wire.begin(8);                /* join i2c bus with address 8 */
  Wire.onReceive(receiveEvent); /* register receive event */
  Wire.onRequest(requestEvent); /* register request event */
             /* start serial for debug */
  servo_motor.attach(6);
  servo_motor.write(90);
  delay(2000);

  dis = readPing();
  delay(100);
  dis = readPing();
  delay(100);
  dis = readPing();
  delay(100);
  dis = readPing();
  delay(100);


  Serial.begin(9600);
	pinMode(IN1, OUTPUT);
	pinMode(IN2, OUTPUT);
	pinMode(IN3, OUTPUT);
	pinMode(IN4, OUTPUT);
  pinMode(IN5, OUTPUT);
	pinMode(IN6, OUTPUT);
	pinMode(IN7, OUTPUT);
	pinMode(IN8, OUTPUT);
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
  digitalWrite(IN5, LOW);
  digitalWrite(IN6, LOW);
  digitalWrite(IN7, LOW);
  digitalWrite(IN8, LOW);

}


void motor_Right(int spd, int dir=-1) { //speed: từ 0 - MAX_SPEED
	//spd = constrain(spd, MIN_SPEED, MAX_SPEED);//đảm báo giá trị nằm trong một khoảng từ 0 - MAX_SPEED - http://arduino.vn/reference/constrain
	if (dir == 2){
    digitalWrite(IN1, HIGH);// chân này không có PWM
    digitalWrite(IN4, HIGH);
    
    analogWrite(IN2, 255 - spd);
    analogWrite(IN3, 255 - spd);
    
  }else if (dir==0) {
    digitalWrite(IN1, LOW);// chân này không có PWM
    digitalWrite(IN4, LOW);
    analogWrite(IN2, spd);
    analogWrite(IN3, spd);
  }else{
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, LOW);
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, LOW);
  }
  
}

void motor_Left(int spd, int dir=-1) { //speed: từ 0 - MAX_SPEED
	//spd = constrain(spd, MIN_SPEED, MAX_SPEED);//đảm báo giá trị nằm trong một khoảng từ 0 - MAX_SPEED - http://arduino.vn/reference/constrain
	if(dir ==2){
    digitalWrite(IN5, HIGH);// chân này không có PWM
    digitalWrite(IN8, HIGH);// chân này không có PWM
    analogWrite(IN6, 255 - spd);
    analogWrite(IN7, 255 - spd);
  }else if (dir == 0) {
    digitalWrite(IN5, LOW);// chân này không có PWM
    digitalWrite(IN8, LOW);// chân này không có PWM
    analogWrite(IN6, spd);
    analogWrite(IN7, spd);
  }else{
    digitalWrite(IN5, LOW);
    digitalWrite(IN6, LOW);
    digitalWrite(IN7, LOW);
    digitalWrite(IN8, LOW);
  }
}
int data;
int msg;
void receiveEvent(int howMany) {
  while(Wire.available()){
    msg = Wire.read();        /* print the character */
    data = Wire.read();
      
  }
        
  Serial.println(msg);
  Serial.println(data);    /* to newline */
  if (msg==23){
    spd=data;
  }else if(msg==24){
    direct=data;
  }
}

// function that executes whenever data is requested from master
void requestEvent() {
  Wire.write(data+2);  /*send string on request */
}

void forward(int spd){
  motor_Right(spd,2);
  motor_Left(spd,2);
}
int readPing(){
  delay(70);
  int cm = sonar.ping_cm();
  if (cm==0){
    cm=250;
  }
  return cm;
}


int right_dis=0;
int left_dis=0;

void avoid_obj(){
  right_dis=0;
  left_dis=0;
  for(int i=85;i>=5;i=i-5){
    servo_motor.write(i);
    right_dis=readPing();
    if (right_dis>=25){
      right_dis=i;
      break;
    }
    right_dis=i;   
    delay(80);   
  }
  for(int i=95;i<=175;i=i+5){
    servo_motor.write(i);
    left_dis=readPing();   
    if (left_dis>=25){
      left_dis=i;
      break;
    }
    left_dis=i;  
    delay(80);   
  }
  if(left_dis-90<90-right_dis){
    
    Serial.println("turn left");
  }else {
    Serial.println("turn right");
  }
  Serial.println(left_dis-90);
  Serial.println(90-right_dis);
  servo_motor.write(90);
}
int from=60;
int to=120;
int step=10;
int temp=0;
void loop()
{ dis=readPing();
  if (from<90){
    // for(int i=from;i<=to;i=i+step){
    //   servo_motor.write(i);
    //   //delay(10);
    //   if (readPing()<dis){
    //     dis=readPing();
    //   }
    //   if(dis<=15){
    //     avoid_obj();
    //     delay(30);
    //     break;
    //   }
    // }
    // from=120;
    // to=60;
  }else {
    // for(int i=from;i>=to;i=i-step){
    //   servo_motor.write(i);
    //   //delay(0);
    //   if (readPing()<dis){
    //     dis=readPing();
        
    //   }
    //   if(dis<=15){
    //     avoid_obj();
    //     delay(30);
    //     break;

    //   }
    //  }
    // from=60;
    // to=120;
  }

  

  Serial.println(dis);
  servo_motor.write(90);
  delay(1000);
  
  
  //Serial.print(spd);
  //Serial.print("--");
  //Serial.println(direct);
	motor_Right(spd,direct);
  motor_Left(spd,direct);
  // Serial.print(msg);
  // Serial.print("--");
  // Serial.println(data);
  
	
}