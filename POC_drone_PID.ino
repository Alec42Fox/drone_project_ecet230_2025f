

#include <Adafruit_VL53L0X.h> //distance sensor library
#include <ESP32Servo.h> //servo library
#include <Wire.h> //I2C library

//Making objects of the distance and ESC
Adafruit_VL53L0X vl53lox = Adafruit_VL53L0X();
Servo speedCon;
//Pins to use when wiring
#define pin_ESC 7
#define WHITE 13
#define YELLOW 12
#define BLUE 11
#define RED 10
#define GREEN 3
#define USER 6
#define pin_SDA 8
#define pin_SCL 9

#define servoZero 1000

void setup() {
    Serial.begin(9600);
    //Two pins being used for SDA and SCL
    Wire.setPins(pin_SDA, pin_SCL);
    //Setting the pin used for ESC and max/min microseconds to controll the speed
    speedCon.attach(pin_ESC, 1000, 2000); 
    Wire.begin();


    // Set the ESC to 0 speed so it doesn't have the motor spin right away
    speedCon.writeMicroseconds(servoZero);

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
        Serial.println("Failed to boot VL53L0X\n\n");
        while (1);
    } else {
        Serial.println("VL53L0X booted!\n\n");
    }

    

    delay(100);
}



void loop() {

    int button = false; 

    VL53L0X_RangingMeasurementData_t distance;

    if (digitalRead(USER) == HIGH) {
        button = true;
        Serial.println("Turned on!");
        delay(2000);
    }

    //hardcoded non-PID loop speed control:
    /*
    while (button == true) {

        vl53lox.rangingTest(&distance, false);

        if (distance.RangeStatus != 4) {
        
        Serial.print("Distance (mm): ");
        Serial.println(distance.RangeMilliMeter);

            if (distance.RangeMilliMeter <= 200) {
            speedCon.writeMicroseconds(1300);
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
    */


    //PID loop speed control:
    //PID constants
    const float Kp = 0.5;
    const float Ki = 0.0;
    const float Kd = 0.1;
    //PID variables
    float setpoint = 500.0; //desired distance in mm
    float input, output;
    float error;
    float previous_error = 0.0;
    float integral = 0.0;
    unsigned long lastTime = 0;
    unsigned long sampleTime = 100; //sample time in milliseconds

    while (button == true) {

        unsigned long now = millis();
        if (now - lastTime >= sampleTime) {
            lastTime = now;

            vl53lox.rangingTest(&distance, false);

            if (distance.RangeStatus != 4) {
                input = distance.RangeMilliMeter;
                error = setpoint - input;
                integral += error * (sampleTime / 1000.0);
                float derivative = (error - previous_error) / (sampleTime / 1000.0);
                output = Kp * error + Ki * integral + Kd * derivative;

                //Constrain output to valid ESC range
                output = constrain(output, 0, 500);
                speedCon.writeMicroseconds(servoZero + output);

                previous_error = error;

                Serial.print("Distance (mm): ");
                Serial.print(distance.RangeMilliMeter);
                Serial.print(" | Output: ");
                Serial.println(output);
            }
            else {
                Serial.println("Distance can't be measured!");
                speedCon.writeMicroseconds(servoZero);
            }
        }

        if (digitalRead(USER) == HIGH) {
        button = false;
        Serial.println("Turned off!");
        speedCon.writeMicroseconds(servoZero);
        delay(2000);
        }

        //safety measures, stop motor if too close
        if (distance.RangeMilliMeter <= 100) {
            speedCon.writeMicroseconds(servoZero);
            Serial.println("Too close! Stopping motor.");
            button = false;
            delay(100);
        }
    }
}