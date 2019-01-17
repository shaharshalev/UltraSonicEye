#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_LSM303_U.h>
//CLOCK (SCL) on pin A5, DATA( SDA) on pin A4
/* Assign a unique ID to this sensor at the same time */
Adafruit_LSM303_Accel_Unified accel = Adafruit_LSM303_Accel_Unified(54321);
const int trigPin1 = 8;
const int echoPin1 = 9;
const int trigPin2 = 3;
const int echoPin2 = 4;
const int vibPin = A1;
const int MAX_VIB = 200;
const int THRESHOLD_FOOT = 20;
const int THRESHOLD_ANKLE = 50;
const double Z_THRESHOLD = 50;



struct UltraSonic {
  const int MAX_SAMPLES = 20;
  const int MAX_DISTANCE = 100;
  int trigPin;
  int echoPin;
  int initHeight;
  int threshold;
  

  UltraSonic(int trig_pin, int echo_pin, int thold): trigPin(trig_pin), echoPin(echo_pin), threshold(thold) {
    pinMode(trigPin, OUTPUT);
    pinMode(echoPin, INPUT);
    initHeight = avg();
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
    int sum = 0, num_samples = MAX_SAMPLES;
    for (int i = 0; i < MAX_SAMPLES ; ++i) {
      int dis = calcDistance();
      if ( dis > MAX_DISTANCE || dis < 0 ) {
        --num_samples;
      } else {
        sum += dis;
      }
    }
    if (num_samples < MAX_SAMPLES / 2) return -1;

    return (sum / num_samples);
  }


  bool DownStairs(int distance) {
    if (distance > threshold) return true;
    return false;
  }

  bool upStairs(int distance) {
    if (distance < threshold) return true;
    return false;
  }


};

UltraSonic sFoot(trigPin1, echoPin1, THRESHOLD_FOOT);
UltraSonic sAnkle(trigPin2, echoPin2, THRESHOLD_ANKLE);
double initZ = 0;



bool isStraight(double z) {
  if (abs(z - initZ) > Z_THRESHOLD) return false;
  return true;
}

void setup() {
  pinMode(vibPin, OUTPUT);
  Serial.println("setup");
  Serial.begin(9600);
#ifndef ESP8266
  while (!Serial);     // will pause Zero, Leonardo, etc until serial console opens
#endif

  /* Initialise the sensor */
  if (!accel.begin())
  {
    /* There was a problem detecting the ADXL345 ... check your connections */
    Serial.println("Ooops, no LSM303 detected ... Check your wiring!");
    while (1);
  }



  sensors_event_t event;
  accel.getEvent(&event);
  initZ = radianToDegree(event.acceleration.z);

  Serial.println("initHeight foot = " + (String)sFoot.initHeight);
  Serial.println("initHeight ankle = " + (String)sAnkle.initHeight);
  Serial.println("init Z = " + (String)initZ);

}

  void vib(int num) {
  analogWrite(vibPin, MAX_VIB);
  delay(num);
  analogWrite(vibPin, 0);
}

double radianToDegree(double z) {
  double temp = ((z * 180) / 3.14159);
  //if(temp < 0) temp += 360;
  return temp;
}

int t1, t2;

void loop() {
  sensors_event_t event;
  accel.getEvent(&event);

  if (isStraight(radianToDegree(event.acceleration.z))) {
    int SFootAvg = sFoot.avg();

    if (isStraight(radianToDegree(event.acceleration.z))) {
      Serial.println("foot = " + (String)SFootAvg );
      if (SFootAvg != -1) {

        if (sFoot.DownStairs(SFootAvg)) {
          Serial.println("Down stairs");
          vib(500);
        }
      }
      if (!sFoot.DownStairs(SFootAvg)) {
        int SAnkleAvg = sAnkle.avg();
        if (isStraight(radianToDegree(event.acceleration.z))) {

          if (SAnkleAvg != -1) {
            //Serial.println("ankle = " + (String)SAnkleAvg );
            if (sAnkle.upStairs(SAnkleAvg)){
              Serial.println("Up stairs");
              vib(200);
              delay(200);
              vib(200);
            }
            
          }
        }
      }

    }
  }

  //Serial.print("Z: "); Serial.print(abs(radianToDegree(event.acceleration.z)-initZ)); Serial.print("  "); Serial.println("m/s^2 ");
}
