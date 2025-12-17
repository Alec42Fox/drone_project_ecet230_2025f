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
    pinMode(LAND, OUTPUT);

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

    int buttonH = false; 
    int buttonL = false;

    //PID loop speed control:
    //PID constants
    const float Kp = 1.0;
    const float Ki = 0.8;
    const float Kd = 0.0;
    //PID variables
    float setpointH = 130.0; //desired hovering distance in mm
    float setpointL = 0.0; //desired landing distance in mm
    float input, output;
    float error;
    float previous_error = 0.0;
    float integral = 0.0;
    float derivative = 0.0;
    unsigned long lastTime = 0;
    unsigned long sampleTime = 100; //sample time in milliseconds

    VL53L0X_RangingMeasurementData_t distance;

    if (digitalRead(HOVER) == HIGH) {

        buttonH = true;
        Serial.println("Hovering Now!");
        delay(2000);
    }

    while (buttonH == true) {

        unsigned long now = millis();

        if (now - lastTime >= sampleTime) {

            lastTime = now;

            vl53lox.rangingTest(&distance, false);

            if (distance.RangeStatus != 4) {
                input = distance.RangeMilliMeter;
                error = setpointH - input;
                integral += error * (sampleTime / 1000.0);
                derivative = (error - previous_error) / (sampleTime / 1000.0);
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
        }

        if (digitalRead(LAND) == HIGH) {

            buttonH = false;
            buttonL = true;
            derivative = 0.0;
            Serial.println("Landing Now!");
            delay(2000);

            while (buttonL == true) {

                unsigned long now = millis();

                if (now - lastTime >= sampleTime) {

                    lastTime = now;

                    vl53lox.rangingTest(&distance, false);

                    if (distance.RangeStatus != 4) {

                        setpointL = distance.RangeMilliMeter - 5;
                        input = distance.RangeMilliMeter;
                        error = setpointL - input;
                        integral += error * (sampleTime / 1000.0);
                        derivative = (error - previous_error) / (sampleTime / 1000.0);
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
                }

                if (distance.RangeMilliMeter == 30) {
                    buttonL = false;
                    speedCon.writeMicroseconds(servoZero);
                }
            }   
        }
    }
}