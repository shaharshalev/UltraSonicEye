
#include <SoftwareSerial.h>
struct UltraSonic {

  
    const int MAX_SAMPLES = 20;
    String objName;
    int maxThreshold;
    int initHeight;
    int trigPin;
    int echoPin;
    bool flag;
    int lastTime;

    
  public:
    int calcDistance() {
      long duration;
      digitalWrite(trigPin, LOW);
      delayMicroseconds(5);
      digitalWrite(trigPin, HIGH);
      delayMicroseconds(100);
      digitalWrite(trigPin, LOW);
      duration = pulseIn(echoPin, HIGH);
      return duration * 0.034 / 2;
    }

    int calcAvg() {
      int sum = 0;
      for (int i = 0; i < MAX_SAMPLES; i++) {
        sum += calcDistance();
      }
      return sum / MAX_SAMPLES;
    }
    
    UltraSonic(String objectName,int max_threshold, int triger, int echo) : objName(objectName),lastTime(0),maxThreshold(max_threshold), trigPin(triger), echoPin(echo)  {
      pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
      pinMode(echoPin, INPUT); // Sets the echoPin as an Input
      //we start taking a first sample for init height
      calcAvg(); //to get more clear results we dump first results
      initHeight = calcAvg();
      flag=false;
    }

    bool isObstacleDetected(int height) {
      return abs(height - initHeight) > maxThreshold;
    }


};

const int yellowPin = 13;
const int whitePin = 12;
const int greenPin = 11;
const int redPin = 10;
const int rxPin = 6;
const int txPin = 5;
SoftwareSerial hc = SoftwareSerial(rxPin, txPin);
//1 is blue

const int trigPin1 = 8;
const int echoPin1 = 9;
const int trigPin2 = 3;
const int echoPin2 = 4;
UltraSonic leftUS = UltraSonic("left",25, trigPin1, echoPin1);
UltraSonic rightUS = UltraSonic("right",10, trigPin2, echoPin2);






void initVibrationMotors() {
  pinMode(yellowPin, OUTPUT);
  pinMode(whitePin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(redPin, OUTPUT);
}


void vib(int pin) {
  digitalWrite(pin, HIGH);
  delay(500);
  digitalWrite(pin, LOW);
}





void setup() {
  Serial.begin(9600); // Starts the serial communication
  initVibrationMotors();
  hc.begin(9600); //init bluetooth
  Serial.println("The initHeight of " + rightUS.objName + " is:" + rightUS.initHeight);
  Serial.println("The initHeight of " + leftUS.objName + " is:" + leftUS.initHeight);
  hc.println("The initHeight of " + rightUS.objName + " is:" + rightUS.initHeight);
  hc.println("The initHeight of " + leftUS.objName + " is:" + leftUS.initHeight);
  
  
}


void readColor() {
  char c;
  c = (char)hc.read();
  if (c == 'Y') {
    vib(yellowPin);
  }
  if (c == 'G') {
    vib(greenPin);
  }
  if (c == 'R') {
    vib(redPin);
  }
  if (c == 'W') {
    vib(whitePin);
  }
}

void vibAll(){
  digitalWrite(yellowPin, HIGH);
  digitalWrite(greenPin, HIGH);
  digitalWrite(redPin, HIGH);
  delay(500);
  digitalWrite(redPin, LOW);
  digitalWrite(greenPin, LOW);
  digitalWrite(yellowPin, LOW);
}



void loop() {
  readColor();
  
  int dis_right = rightUS.calcAvg();
  int dis_left = leftUS.calcAvg();

  
  Serial.println("right:" + String(dis_right));
  Serial.println("left:" + String(dis_left));

  if(rightUS.isObstacleDetected(dis_right) || leftUS.isObstacleDetected(dis_left)){
    vibAll();
  }
/*
  if ((rightUS.isObstacleDetected(dis_right) && !rightUS.flag) ) {
    for(int i=0;i<3;i++) {
      vibAll();
      hc.println("right vib - long dis" );
      }
    //rightUS.flag=true;
    rightUS.lastTime=millis();
  }

  if(leftUS.isObstacleDetected(dis_left) && !leftUS.flag){
    for(int i=0;i<2;i++) {
      vibAll();
      hc.println("left vib" );
    }
    //leftUS.flag=true;
    leftUS.lastTime=millis();
  }
*/
  
  /* if(millis()-rightUS.lastTime >= 2000) rightUS.flag = false;
  // if(millis()-leftUS.lastTime >= 2000) leftUS.flag = false;
  */

}
