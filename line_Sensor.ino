#define lineSen1 26
#define lineSen2 28
#define lineSen3 30
#define lineSen4 32
#define lineSen5 34
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(lineSen1, INPUT);
  pinMode(lineSen2, INPUT);
  pinMode(lineSen3, INPUT);
  pinMode(lineSen4, INPUT);
  pinMode(lineSen5, INPUT);
  
}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.print(digitalRead(lineSen1));
  Serial.print("-");
  Serial.print(digitalRead(lineSen2));
  Serial.print("-");
  Serial.print(digitalRead(lineSen3));
  Serial.print("-");
  Serial.print(digitalRead(lineSen4));
  Serial.print("-");
  Serial.print(digitalRead(lineSen5));
  Serial.println();
  delay(200);
}
