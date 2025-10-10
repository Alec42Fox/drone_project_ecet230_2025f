// Three libraries needed for POC
#include <Adafruit_VL53L0X.h>
#include <ESP32Servo.h>
#include <Wire.h>
// Making objects of the distance and ESC
Adafruit_VL53L0X vl53lox = Adafruit_VL53L0X();
Servo speedCon;
// Pins to use when wiring
const int ESC_PIN = 20;
const int WHITE = 35;
const int YELLOW = 36;
const int BLUE = 37;
const int RED = 38;
const int GREEN = 39;

void setup() {

  Serial.begin(115200);
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
// Set the ESC to 0 speed so it doesn't have the motor spin right away
  speedCon.writeMicroseconds(1000);
  delay(2000);
}

void loop() {
// Variable that holds the distance being measured by the sensor
  VL53L0X_RangingMeasurementData_t distance;
// Function that reads the distance
  vl53lox.rangingTest(&distance, false);
// Choosing the speed and LEDs that turn on depending on the distance measured
// 4 means that the sensor wasn't able to make a good reading
  if (distance.RangeStatus != 4) {
    
    Serial.print("Distance (mm): " + distance.RangeMilliMeter);

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
  else
    Serial.print("Distance can't be measured!");

  delay(50);
}