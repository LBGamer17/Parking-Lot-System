#include <Servo.h>
#include <LiquidCrystal.h>
#include "SR04.h"

/************************************* ULTRA SONIC *************************************/
#define TRIG_PIN 15
#define ECHO_PIN 14
SR04 ultrasonic = SR04(ECHO_PIN,TRIG_PIN);


/***************************************** LCD *****************************************/
LiquidCrystal lcd(7, 8, 9, 10, 11, 12);


/**************************************** SERVO ****************************************/
long entrance_car_detected;

Servo myservo;  // create Servo object to control a servo
int pos = 0;    // variable to store the servo position


/************************************** IR SENSOR **************************************/
int cars_parked = 0;  // Number of cars parked
const int IRSensorPin1 = 16;  // IR sensor output pin connected to digital pin 16


void setup() 
{
  /************************************* ULTRA SONIC *************************************/
  Serial.begin(9600);
  delay(1000);

  /************************************** IR SENSOR **************************************/
  pinMode(IRSensorPin1, INPUT);  // Set IR sensor pin as input

  /**************************************** SERVO ****************************************/
  // attaches the servo on pin 5 to the Servo object
  myservo.attach(5);

  /***************************************** LCD *****************************************/
  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  // Print a message to the LCD.
  lcd.print("Hello, World!");
}

void loop() 
{
  int IRSensorValue1 = digitalRead(IRSensorPin1);  // Read the value from the IR sensor

  lcd.setCursor(0, 1);
  entrance_car_detected = ultrasonic.Distance();

  // If a car is near the gate, open the gate
  if ((entrance_car_detected <= 3) && (pos <= 45))
  {

    for (pos = 0; pos <= 135; pos += 1) { // goes from 0 degrees to 135 degrees
      // in steps of 1 degree
      myservo.write(pos);              // tell servo to go to position in variable 'pos'
      delay(5);
    }
  }

  // If there is no car on the gate, close the gate
  if ((entrance_car_detected > 3) && (pos > 45))
  {
    for (pos = 135; pos >= 0; pos -= 1) { // goes from 135 degrees to 0 degrees
      myservo.write(pos);              // tell servo to go to position in variable 'pos'
      delay(10);
    }
  }

  // If a car was parked at the sensor
  if (IRSensorValue1 == LOW) {
    cars_parked = 1;

    lcd.clear();

    // Obstacle detected
    lcd.setCursor(0, 0);
    lcd.print("Cars Parked:");
    lcd.print(cars_parked);
  }

  // If there's no car parked
  if (IRSensorValue1 == HIGH) {
    cars_parked = 0;

    lcd.clear();

    lcd.setCursor(0, 0);
    lcd.print("Cars Parked:");
    lcd.print(cars_parked);
  }
  delay(100);                       // waits 10 ms for the servo to reach the position
}
