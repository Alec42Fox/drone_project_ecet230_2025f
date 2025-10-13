// Three libraries needed for POC
#include <Adafruit_VL53L0X.h>
#include <ESP32Servo.h>
#include <Wire.h>
// Making objects of the distance and ESC
Adafruit_VL53L0X vl53lox = Adafruit_VL53L0X();
Servo speedCon;
// Pins to use when wiring
const int ESC_PIN = 7;
const int WHITE = 13;
const int YELLOW = 12;
const int BLUE = 11;
const int RED = 10;
const int GREEN = 3;
const int USER = 6;

void setup() {

  Serial.begin(9600);
// Two pins being used for SDA and SCL
  Wire.setPins(8, 9);
// Setting the pin used for ESC and max/min microseconds to controll the speed
  speedCon.attach(ESC_PIN, 1000, 2000); 
  Wire.begin();

  pinMode(WHITE, OUTPUT);
  pinMode(YELLOW, OUTPUT);
  pinMode(BLUE, OUTPUT);
  pinMode(RED, OUTPUT);
  pinMode(GREEN, OUTPUT);
  pinMode(USER, OUTPUT);
// Wait for serial port to connect
  while (!Serial) {
    delay(1);
  }
  
  Serial.println("Proof Of Concept!\n\n");

  if (!vl53lox.begin()) {
    Serial.println("Failed to boot VL53L0X");
    while (1);
  }

// Set the ESC to 0 speed so it doesn't have the motor spin right away
  speedCon.writeMicroseconds(1000);

  delay(2000);
}

void loop() {

  int button = false; 

  VL53L0X_RangingMeasurementData_t distance;

  if (digitalRead(USER) == HIGH) {
    button = true;
    Serial.println("Turned on!");
    delay(2000);
  }

  while (button == true) {

      vl53lox.rangingTest(&distance, false);

      if (distance.RangeStatus != 4) {
      
      Serial.print("Distance (mm): ");
      Serial.println(distance.RangeMilliMeter);

        if (distance.RangeMilliMeter <= 200) {
          speedCon.writeMicroseconds(1200);
          digitalWrite(WHITE, HIGH);
          digitalWrite(YELLOW, LOW);
          digitalWrite(BLUE, LOW);
          digitalWrite(RED, LOW);
          digitalWrite(GREEN, LOW);
        }
        else if (distance.RangeMilliMeter > 200 && distance.RangeMilliMeter <= 400) {
          speedCon.writeMicroseconds(1400);
          digitalWrite(WHITE, HIGH);
          digitalWrite(YELLOW, HIGH);
          digitalWrite(BLUE, LOW);
          digitalWrite(RED, LOW);
          digitalWrite(GREEN, LOW);
        }
        else if (distance.RangeMilliMeter > 400 && distance.RangeMilliMeter <= 600) {
          speedCon.writeMicroseconds(1600);
          digitalWrite(WHITE, HIGH);
          digitalWrite(YELLOW, HIGH);
          digitalWrite(BLUE, HIGH);
          digitalWrite(RED, LOW);
          digitalWrite(GREEN, LOW);
        }
        else if (distance.RangeMilliMeter > 600 && distance.RangeMilliMeter <= 800) {
          speedCon.writeMicroseconds(1800);
          digitalWrite(WHITE, HIGH);
          digitalWrite(YELLOW, HIGH);
          digitalWrite(BLUE, HIGH);
          digitalWrite(RED, HIGH);
          digitalWrite(GREEN, LOW);
        }
        else if (distance.RangeMilliMeter > 800 && distance.RangeMilliMeter <= 1000) {
          speedCon.writeMicroseconds(2000);
          digitalWrite(WHITE, HIGH);
          digitalWrite(YELLOW, HIGH);
          digitalWrite(BLUE, HIGH);
          digitalWrite(RED, HIGH);
          digitalWrite(GREEN, HIGH);
        }
      }
      else {
        Serial.println("Distance can't be measured!");
          speedCon.writeMicroseconds(1000);
          digitalWrite(WHITE, LOW);
          digitalWrite(YELLOW, LOW);
          digitalWrite(BLUE, LOW);
          digitalWrite(RED, LOW);
          digitalWrite(GREEN, LOW);
      }

    if (digitalRead(USER) == HIGH) {
      button = false;
      Serial.println("Turned off!");
      speedCon.writeMicroseconds(1000);
      digitalWrite(WHITE, LOW);
      digitalWrite(YELLOW, LOW);
      digitalWrite(BLUE, LOW);
      digitalWrite(RED, LOW);
      digitalWrite(GREEN, LOW);
      delay(2000);
    }
  }
}