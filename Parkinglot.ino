#include <Servo.h>
#include <LiquidCrystal.h>
#include "SR04.h"


/***********************************************************************************************************/
/*********************************************** SENSOR SETUP **********************************************/
/***********************************************************************************************************/

/******************************** ULTRA SONIC ********************************/
#define TRIG_PIN 15
#define ECHO_PIN 14
SR04 ultrasonic = SR04(ECHO_PIN,TRIG_PIN);


/************************************ LCD ************************************/
LiquidCrystal lcd(7, 8, 9, 10, 11, 12);


/*********************************** SERVO ***********************************/
long entrance_car_detected;

Servo myservo;            // create Servo object to control a servo
int servo_pos = 0;        // variable to store the servo servo_position
int target_pos = 135;     // Limit at an angle of 135 degrees

unsigned long last_servo_millis = 0;  // Global previous time value for servo
const int servo_move_interval = 1;    // Time to wait between steps (1 ms)
int servo_steps = 5;                  // Steps that influence the speed of the opening (can vary)


/********************************* IR SENSOR *********************************/
int cars_parked = 0;  // Number of cars parked
const int IRSensorPin1 = 16;  // IR sensor output pin connected to digital pin 21
const int IRSensorPin2 = 17;
// const int IRSensorPin3 = 18;
// const int IRSensorPin4 = 17;
// const int IRSensorPin5 = 16;
// const int IRSensorPin6 = 21;
// const int IRSensorPin7 = 22;

// Global previous IR Sensor values (Checks if it has been parked):
int lastIRSensorValue1 = HIGH;  // All initialized at HIGH, meaning off.
int lastIRSensorValue2 = HIGH;
int lastIRSensorValue3 = HIGH;
int lastIRSensorValue4 = HIGH;
int lastIRSensorValue5 = HIGH;
int lastIRSensorValue6 = HIGH;
int lastIRSensorValue7 = HIGH;



/***********************************************************************************************************/
/********************************************* MAIN FUNCTIONS **********************************************/
/***********************************************************************************************************/

/************************* INITIALIZATION OF SENSORS *************************/
void setup() 
{
  /***************** ULTRA SONIC *****************/
  Serial.begin(9600);
  delay(1000);

  /****************** IR SENSOR ******************/
  pinMode(IRSensorPin1, INPUT);  // Set IR sensor pin as input
  pinMode(IRSensorPin2, INPUT);  // Set IR sensor pin as input
  // pinMode(IRSensorPin3, INPUT);  // Set IR sensor pin as input
  // pinMode(IRSensorPin4, INPUT);  // Set IR sensor pin as input
  // pinMode(IRSensorPin5, INPUT);  // Set IR sensor pin as input
  // pinMode(IRSensorPin6, INPUT);  // Set IR sensor pin as input
  // pinMode(IRSensorPin7, INPUT);  // Set IR sensor pin as input

  /******************** SERVO ********************/
  // attaches the servo on pin 5 to the Servo object
  myservo.attach(5);
  myservo.write(servo_pos);

  /********************* LCD *********************/
  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  // Print a message to the LCD.
  lcd.print("Hello, World!");
}


/************************* FUNCTION FOR  PARKED CARS *************************/
void ParkToggle(int IRSensorValue, int &lastIRSensorValue) 
{
  if (IRSensorValue == LOW && lastIRSensorValue == HIGH) {
    // If a car parked, increase the number of cars parked
    cars_parked++;
  }
  if (IRSensorValue == HIGH && lastIRSensorValue == LOW) {
    // If a car is not parked, decrease the number of cars parked
    cars_parked--;
  }
  if (cars_parked < 0) {
    cars_parked = 0;
  }

  // Update LCD
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Cars Parked:");
  lcd.print(cars_parked);

  // Save current state for next loop
  lastIRSensorValue = IRSensorValue;
}


/******************************* MAIN FUNCTION *******************************/
void loop() 
{
  int IRSensorValue1 = digitalRead(IRSensorPin1);  // Read the value from the IR sensor
  int IRSensorValue2 = digitalRead(IRSensorPin2);  // Read the value from the IR sensor
  // int IRSensorValue3 = digitalRead(IRSensorPin3);  // Read the value from the IR sensor
  // int IRSensorValue4 = digitalRead(IRSensorPin4);  // Read the value from the IR sensor
  // int IRSensorValue5 = digitalRead(IRSensorPin5);  // Read the value from the IR sensor
  // int IRSensorValue6 = digitalRead(IRSensorPin6);  // Read the value from the IR sensor
  // int IRSensorValue7 = digitalRead(IRSensorPin7);  // Read the value from the IR sensor

  lcd.setCursor(0, 1);

  /*************** SERVO ***************/
  unsigned long servo_millis = millis();
  entrance_car_detected = ultrasonic.Distance();

  // If a car is near the gate (entrance_car_detected is within 10m), open the gate
  if ((entrance_car_detected <= 10)) {
    target_pos = 135; // Open
  }
  // Otherwise, close it
  else {
    target_pos = 0;   // close
  }

  // Then, control the servo without blocking
  if (servo_millis - last_servo_millis >= servo_move_interval) {
    last_servo_millis = servo_millis; // Save the current timestamp

    if (servo_pos <= target_pos) {
      servo_steps = 10;
      servo_pos += servo_steps;   // Open the gate
    }
    else if (servo_pos > target_pos) {
      servo_steps = 3;
      servo_pos -= servo_steps;   // Close the gate
    }
    myservo.write(servo_pos);
  }

  /*************** PARKING ***************/
  ParkToggle(IRSensorValue1, lastIRSensorValue1); // For parking slot 1
  ParkToggle(IRSensorValue2, lastIRSensorValue2); // For parking slot 2
  // ParkToggle(IRSensorValue3, lastIRSensorValue3); // For parking slot 3
  // ParkToggle(IRSensorValue4, lastIRSensorValue4); // For parking slot 4
  // ParkToggle(IRSensorValue5, lastIRSensorValue5); // For parking slot 5
  // ParkToggle(IRSensorValue6, lastIRSensorValue6); // For parking slot 6
  // ParkToggle(IRSensorValue7, lastIRSensorValue7); // For parking slot 7
}
