//Khai báo các biến

#define IN1	8
#define IN2	9
#define IN3	10
#define IN4	11
#define line1 44
#define line2 46
#define line3 48
#define line4 50
#define line5 52
#define line6 53
#define trig 36
#define echo 38
#define servo 12
#define maximum_distance 150     // khoảng các lớn nhất cần đo
#define minimum_distance 5      // khoảng các nhỏ nhất cần đo

#include <Servo.h>
#include <Wire.h>
#include <Arduino.h>
#include <NewPing.h>
#include <TimerOne.h>
NewPing sonar(trig, echo,maximum_distance); //sensor function
Servo servo_motor;

int sensitive1[] = {20,50,110,170};     // độ nhạy cho cảm biến
int sensitive2[] = {30,60,120,190};
int power,speed=0;    //Bật tắt và Tốc độ động cơ
int avg=0;    // biến dò line
int calib=0;  // biến hiệu chỉnh động cơ
long time=0;  // bộ đếm timer
int distance = 0;  // biến đo khoảng cách
int left_right = -1;  // hướng tránh vật cản

void receiveEvent(int howMany) {  //sự kiện receive i2c
  String lable="";
  while (Wire.available()>1) {
    lable += (char)Wire.read();      /* nhận lable cho data */
  }
  int data = Wire.read();
  if (lable=="speed") {         //gán data với biến tương ứng
    speed=data;
    Serial.print("receivetgSpeed");
    Serial.println(speed);
  }else if (lable=="power") {
    power=data;
    Serial.print("receivePower");
    Serial.println(power);
  }
}

void requestEvent() {       //sự kiện request i2c - gửi giá trị power và speed
  // Serial.print("send");
  // Serial.print(power);
  // Serial.println(speed);
  Wire.write(power);
  Wire.write(speed);  
}


void setup() {
  // Setup môi trường
  Wire.begin(8);                /* tham gia i2c với địa chỉ 8 */
  Wire.onReceive(receiveEvent); /* tạo sự kiện onReceive */
  Wire.onRequest(requestEvent); /* tạo sự kiện onRequest*/
  
  Serial.begin(115200);     // bật Serial baudrate 115200

  pinMode(line1, INPUT);  //Các chân cảm biến line
  pinMode(line2, INPUT);
  pinMode(line3, INPUT);
  pinMode(line4, INPUT);
  pinMode(line5, INPUT);
  pinMode(line6, INPUT);

	pinMode(IN1, OUTPUT);  //Các chân đièu khiển module động cơ
	pinMode(IN2, OUTPUT);
	pinMode(IN3, OUTPUT);
	pinMode(IN4, OUTPUT);

  digitalWrite(IN1, LOW); //Tắt tất cả chân
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);

  servo_motor.attach(servo);  //gán chân servo
  servo_motor.write(90);    // xoay servo về trước

  delay(1000);        // xác định khoảng cách phía trước

}


// Hàm điều khiển motor phải
void motor_Right(int dir=-1, int spd=0) { 
	if (dir == 2 && spd != 0){
    digitalWrite(IN3, LOW);    
    analogWrite(IN4,spd);
    
  }else if (dir==0 && spd != 0) {
    digitalWrite(IN4, LOW);
    analogWrite(IN3, spd);
  }else{
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, LOW);
  }
  
}

// Hàm điều khiển motor trái
void motor_Left(int dir=-1, int spd=0) { 
	if (dir == 2 && spd != 0){
    digitalWrite(IN1, LOW);    
    analogWrite(IN2,spd);
    
  }else if (dir==0 && spd != 0) {
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
  delay(50);
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

// các function chức năng dò line

int getSensitive(int avgValue){    // lây giá trị độ nhạy theo cảm biến và tốc độ
  if (speed == 0){
    return 0;
  }else{
    int tmp_calib = 0;
    
    
    switch (avgValue) {
      case 1:
        tmp_calib = map(speed, 75, 100, sensitive1[0], sensitive2[0]);
        break;
      case -1:
        tmp_calib = map(speed, 75, 100, sensitive1[0], sensitive2[0]);
        break;
      case 2:
        tmp_calib = map(speed, 75, 100, sensitive1[1], sensitive2[1]);
        break;
      case -2:
        tmp_calib = map(speed, 75, 100, sensitive1[1], sensitive2[1]);
        break;
      case 3:
        tmp_calib = map(speed, 75, 100, sensitive1[2], sensitive2[2]);
        break;
      case -3:
        tmp_calib = map(speed, 75, 100, sensitive1[2], sensitive2[2]);
        break;
      case 4:
        tmp_calib = map(speed, 75, 100, sensitive1[3], sensitive2[3]);
        break;
      case -4:
        tmp_calib = map(speed, 75, 100, sensitive1[3], sensitive2[3]);
        break;
      default:
        tmp_calib=0;
        break;
    }
    return tmp_calib;
  }
}

int getAvg(){     // kiểm tra trạng thái dò line
  int sen1=digitalRead(line1);
  int sen2=digitalRead(line2);
  int sen3=digitalRead(line3);
  int sen4=digitalRead(line4);
  int sen5=digitalRead(line5);
  if (sen1 == 1 && sen2 ==1 && sen3 == 1 && sen4 ==1 && sen5 == 1){
    return 6;
  }else{
    int ln1= sen1==0 ? -4:0;
    int ln2= sen2==0 ? -2:0;
    int ln3= sen3==0 ?  0:0;
    int ln4= sen4==0 ?  2:0;
    int ln5= sen5==0 ?  4:0;
    return (ln1+ln2+ln3+ln4+ln5)/(5-(sen1+sen2+sen3+sen4+sen5));
  }
}

//Các function chức năng tránh vật cản
int readPing(int main){     // đo khoảng cách 
  if(main!=1){
    delay(80);
  } 
  int cm = sonar.ping_cm(); //ping lấy khoảng cách
  if (cm==0){
    cm=maximum_distance;    //nếu distance lớn hơn max => distance=max
  }else if (cm<minimum_distance){
    cm=minimum_distance;  //nếu distance nhỏ hơn min => distance=min
  }
  return cm;
}

int scan_Space(int edge = 30){    //quét vật cản, chọn hướng tránh
  int right_dis=0;
  int left_dis=0;
  for(int i=85;i>=5;i=i-5){       // quét bên phải
    servo_motor.write(i);
    right_dis=readPing(0);
    if (right_dis>=edge){
      right_dis=i;
      break;
    }
    right_dis=i; 
  }
  for(int i=95;i<=175;i=i+5){     //quét bên trái
    servo_motor.write(i);
    left_dis=readPing(0);  
    if (left_dis>=edge){
      left_dis=i;
      break;
    }
    left_dis=i;
  }
  Serial.println(left_dis-90);
  Serial.println(90-right_dis);
  
  servo_motor.write(90);
  if(left_dis-90<90-right_dis){       //tránh về phía gần hơn
    Serial.println("turn left"); 
    return left_dis;
  }else {
    Serial.println("turn right");
    return right_dis;
  }
}


void avoid_Obj(){       // Tránh vật cản
  int left_right = scan_Space();    // xác định hướng tránh
  while(readPing(0) <16 ){ backward(80);  }    // lùi giữ khoảng cách
  stop();

  if (left_right < 90){           // tránh sang phải
    servo_motor.write(110);    
    delay(100);
    distance = readPing(0);        // lấy khoảng cách
    while (distance > 50) {       // xoay phải để tránh
      turn_Right(80);
      delay(100);
      stop();
      distance = readPing(0);
    }
    while (distance < 50) {
      turn_Right(80);
      delay(100);
      stop();
      distance = readPing(0);
    }
    servo_motor.write(145);
    int flag_line = getAvg()!=6 || digitalRead(line6);         // đặt flag cho các cảm biến dò line
    
    distance = readPing(0);    
    while((getAvg()== 6 || flag_line==1 ) ){  // di chuyển giữ khoảng cách nhất định với vật thể đến khi gặp line
      int spd_L = 80;
      int spd_R = 150;
      if (distance>100){
        spd_R = 170;
        spd_L = 80;

      }else 
      if (distance >15 && distance<20) {
        spd_R = 80;
        spd_L = 130;
      }else if (distance < 15 ) {
        spd_R = 0;
        spd_L = 0;
        
        motor_Right(0,80);
        motor_Left(1);
        delay(150);
      }
      motor_Right(2,spd_R);
      motor_Left(2,spd_L);
      if (digitalRead(line6)==0 && getAvg()== 6){           // tắt flag khi xe hoàn toàn ra khỏi line khi bắt đầu tránh
        flag_line = 0;
      }
      distance = readPing(0);
      while(getAvg()== 6 && flag_line == 0 && digitalRead(line6)==1){    // lùi xe khi cảm biến dò line bỏ lỡ line
        while(getAvg()== 6){
          backward(80);
        }
      }
    }
    stop();
    delay(100);
    servo_motor.write(90);
    
    //Xoay xe đúng hướng với line
    while (getAvg()!=6 || digitalRead(line6)==1) {
      forward(80);
    }
    stop();
    delay(100);
    while (getAvg()==6) {
     turn_Right(80);
    }
    stop();

  }else{        // tránh sang trái
    servo_motor.write(70);
    delay(100);
    distance = readPing(0);      //lấy khoảng cách
    while (distance > 50) {     // xoay trái để tránh
      turn_Left(80);
      delay(100);
      stop();
      distance = readPing(0);
    }
    while (distance < 50) {
      turn_Left(80);
      delay(100);
      stop();
      distance = readPing(0);
    }
    
    servo_motor.write(35);
    int flag_line = getAvg()!=6 || digitalRead(line6);      // đặt flag cho các cảm biến dò line
    distance = readPing(0);
    while((getAvg()== 6 || flag_line==1 )){ // di chuyển giữ khoảng cách nhất định với vật thể đến khi gặp line
      int spd_L = 150;
      int spd_R = 80;
      if (distance>100){
        spd_R = 80;
        spd_L = 170;
      }else 
      if (distance>15 && distance<20) {
        spd_R = 130;
        spd_L = 80;
      }else if (distance<15) {
        motor_Right(1);
        motor_Left(0,80);
        delay(150);
        spd_R = 0;
        spd_L = 0;
      }
      motor_Right(2,spd_R);
      motor_Left(2,spd_L);
      // delay(150);
      // stop();
      if (digitalRead(line6)==0 && getAvg()== 6){       // tắt flag khi xe hoàn toàn ra khỏi line khi bắt đầu tránh
        flag_line = 0;
      }
      distance = readPing(0);
      while(flag_line==0 && flag_line == 0 && digitalRead(line6)==1){   // lùi xe khi cảm biến dò line bỏ lỡ line
        while(getAvg()== 6){
          backward(80);
        }
      }
    }
    
    stop();
    delay(100);
    servo_motor.write(90);
    //Xoay xe đúng hướng với line
    while (getAvg()!=6 || digitalRead(line6)==1) {
      forward(80);
    }
    stop();
    delay(100);
    while (getAvg()==6) {
     turn_Left(80);
    }
    stop();
  }
}



// Hàm thực thi
void loop() {
    
  if(power==1 && speed!=0){
    avg = getAvg();
    if (avg!=6){          // nếu xe trên line => reset timer
      time=millis();
    }else{
      if(millis()-time > 100){         // nếu xe không trên line => debounce 100ms
        stop();
        delay(100);     
        while(getAvg()==6 && millis()-time<1500){       // lùi xe đến khi chạm line
          backward(80);
        }
        stop();
      }      
    }
    
    calib = getSensitive(avg);
    

    if(speed > 0 && (speed-calib)<75){
      calib = speed;
    }
       
    distance = readPing(1);
    Serial.println(distance);
    if (speed!=0 && distance >15){
      // Serial.println(speed);
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
      
    }else if(speed!=0 && distance <15){
      stop();
      delay(1000);
      if (readPing(1) <= 15 ){
        avoid_Obj();
      }
    }
    
  }else{
    stop();
  }
}
