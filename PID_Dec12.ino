#include <Adafruit_VL53L0X.h> //distance sensor library
#include <ESP32Servo.h> //servo library
#include <Wire.h> //I2C library

//Making objects of the distance and ESC
Adafruit_VL53L0X vl53lox = Adafruit_VL53L0X();
Servo speedCon;
//Pins to use when wiring
#define pin_ESC 7
#define HOVER 6
#define LAND 15
#define pin_SDA 8
#define pin_SCL 9
// Zero value for the motor
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

    pinMode(HOVER, OUTPUT);

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

    if (digitalRead(HOVER) == HIGH) {
        button = true;
        Serial.println("Turned on!");
        delay(2000);
    }

    //PID loop speed control:
    //PID constants
    const float Kp = 0.4;
    const float Ki = 0.2;
    const float Kd = 0.1;
    //PID variables
    float setpoint = 200.0; //desired distance in mm
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
                output = constrain(output, 0, 1000);
                speedCon.writeMicroseconds(servoZero + output);

                previous_error = error;

                Serial.print("Distance (mm): ");
                Serial.print(distance.RangeMilliMeter);
                Serial.print(" | Output: ");
                Serial.print(output);
                Serial.print(" | Total: ");
                Serial.println(servoZero + output);
            }
            else {
                Serial.println("Distance can't be measured!");
                speedCon.writeMicroseconds(servoZero);
            }
        }

        if (digitalRead(HOVER) == HIGH) {
        button = false;
        Serial.println("Turned off!");
        speedCon.writeMicroseconds(servoZero);
        delay(2000);
        }

        //safety measures, stop motor if too close
        if (distance.RangeMilliMeter <= 10) {
            speedCon.writeMicroseconds(servoZero);
            Serial.println("Too close! Stopping motor.");
            button = false;
            delay(100);
        }
    }
}
