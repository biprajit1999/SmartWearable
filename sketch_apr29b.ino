#include <MPU6050.h>
#include <Wire.h>

#define trigPin 4
#define echoPin 5
#define CW 6

MPU6050 mpu;
char c;

String str; // declare str as a global variable

void setup() {
  Serial.begin(9600);
  Wire.begin();
  mpu.initialize();
  Wire.begin(8);  
  Wire.onReceive(receiveEvent);
  Wire.onRequest(sendEvent);
  
  Serial.begin(115200);

  pinMode(trigPin, OUTPUT); 
  pinMode(echoPin, INPUT);
  
  pinMode(CW, OUTPUT);
  digitalWrite(CW,HIGH);
}

void loop() {
  // Read sensor data
  int heartRate = analogRead(A0);
  float temperature = (analogRead(A1) * 5.0 / 1024.0 - 0.5) * 100.0;
  int16_t ax, ay, az;
  mpu.getAcceleration(&ax, &ay, &az);
  float xAcc = ax / 16384.0;
  float yAcc = ay / 16384.0;
  float zAcc = az / 16384.0;
  float totalAcc = sqrt(xAcc*xAcc + yAcc*yAcc + zAcc*zAcc);

  ///////////////////////////Sending data to esp8266////////////////////
  str = String(heartRate) + "," + String(temperature) + "," + String(totalAcc);
  Serial.println(str);
  delay(1000);
}

void receiveEvent(int howMany){
  while (Wire.available()>0) {
     c = Wire.read();
    // Serial.print(c);
  }
}

void sendEvent(int howmany){
  for(int i=0;i<str.length();i++){
     Wire.write(str[i]);
  }  
}
