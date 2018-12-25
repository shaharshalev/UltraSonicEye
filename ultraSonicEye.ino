
#include <SoftwareSerial.h>
struct UltraSonic {

  
    const int MAX_SAMPLES = 10;
    String objName;
    int initHeight;
    int trigPin;
    int echoPin;

    
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
    
    UltraSonic(String objectName, int triger, int echo) : objName(objectName), trigPin(triger), echoPin(echo)  {
      pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
      pinMode(echoPin, INPUT); // Sets the echoPin as an Input
      //we start taking a first sample for init height
      calcAvg(); //to get more clear results we dump first results
      initHeight = calcAvg();
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
UltraSonic leftUS = UltraSonic("left", trigPin1, echoPin1);
UltraSonic rightUS = UltraSonic("right", trigPin2, echoPin2);





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
  
  int dis_right = rightUS.calcDistance();
  int dis_left = leftUS.calcDistance();
  
  hc.println("right:" + String(dis_right));
  hc.println("left:" + String(dis_left));

  if(dis_left > (20 + leftUS.initHeight) || dis_left < (leftUS.initHeight - 20) || dis_right > (20 + rightUS.initHeight) || dis_right < (rightUS.initHeight - 20))
    vibAll();
}
