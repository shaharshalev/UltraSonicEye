#include <SoftwareSerial.h>
const int trigPin1 = 8;
const int echoPin1 = 9;
const int trigPin2 = 3;
const int echoPin2 = 4;
const int THRESHOLD_RIGHT = 20;
const int THRESHOLD_LEFT = 10;
const int rxPin = 6;
const int txPin = 5;


struct UltraSonic {
  const int MAX_SAMPLES = 30;
  int trigPin;
  int echoPin;
  int initHeight;
  int threshold;

  UltraSonic(int trig_pin, int echo_pin, int thold): trigPin(trig_pin), echoPin(echo_pin),threshold(thold) {
    pinMode(trigPin, OUTPUT);
    pinMode(echoPin, INPUT);
    avg();
    initHeight=avg();
  }

  long calcDistance() {
    long duration;
    digitalWrite(trigPin, LOW);
    delayMicroseconds(5);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(100);
    digitalWrite(trigPin, LOW);
    duration = pulseIn(echoPin, HIGH);
    return duration * 0.034 / 2;
  }

  int avg() {
    int numSample = MAX_SAMPLES, sum = 0;
    for (int i = 0 ; i < MAX_SAMPLES ; ++i) {
      int temp = calcDistance();
      if (temp > 200 || temp < 20) {
        --numSample;
      } else {
        sum += temp;
      }
    }
    return sum / numSample;

  }

  bool bump(int distance){
      if(abs(distance - initHeight) > threshold) return true;
      return false;
  }


};

UltraSonic sRight(trigPin1, echoPin1, THRESHOLD_RIGHT);
UltraSonic sLeft(trigPin2, echoPin2, THRESHOLD_LEFT);
SoftwareSerial hc = SoftwareSerial(rxPin, txPin);



void setup() {
  Serial.println("setup");
  Serial.begin(9600);
  hc.begin(9600);

}

void loop() {
  int SRightAvg = sRight.avg();
  int SLeftAvg = sLeft.avg();
  delay(100);
  Serial.println("dis1 = " + (String)SRightAvg );
  hc.println("dis1 = " + (String)SRightAvg );
  delay(100);
  Serial.println("dis2 = " + (String)SLeftAvg );
  hc.println("dis2 = " + (String)SLeftAvg );
  delay(100);
  if(sRight.bump(SRightAvg)){
    Serial.println("BUMP RIGHT");
    hc.println("BUMP RIGHT");
  }
  delay(100);
  if(sLeft.bump(SLeftAvg)){ 
    Serial.println("BUMP LEFT");
    hc.println("BUMP LEFT");
  }
  delay(100);

  

}
