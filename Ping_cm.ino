#include <NewPing.h>
const int trig = 30;     // chân trig của HC-SR04
const int echo = 31;     // chân echo của HC-SR04
#define maximum_distance 30     // khoảng các lớn nhất cần đo
#define minimum_distance 5      // khoảng các nhỏ nhất cần đo
NewPing sonar(trig, echo,maximum_distance); //sensor function
void setup() {
  Serial.begin(9600);   // tạo đường Serial giao tiếp
}
void loop() {  
  int distance=readPing();  //đọc khoảng cách từ sensor
  Serial.print("distance: ");  //in khoảng cách ra Serial
  Serial.print(distance);
  Serial.print("cm");
  switch (distance){    
    case maximum_distance:
      Serial.println("\t-\t MAX DISTANCE");
      break;
    case minimum_distance:
      Serial.println("\t-\t MIN DISTANCE");
      break;
    default:
      Serial.println("\t-\t ACCEPTABLE DISTANCE");
      break;
  }    
}
int readPing(){
  delay(70);
  int cm = sonar.ping_cm(); //ping lấy khoảng cách
  if (cm==0){
    cm=maximum_distance;    //nếu distance lớn hơn max => distance=max
  }else if (cm<minimum_distance){
    cm=minimum_distance;  //nếu distance nhỏ hơn min => distance=min
  }
  return cm;
}