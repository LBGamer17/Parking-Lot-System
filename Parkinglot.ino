#include <Wire.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <LiquidCrystal_I2C.h>
#include <ESP32Servo.h>


/***********************************************************************************************************/
/*********************************************** SENSOR SETUP **********************************************/
/***********************************************************************************************************/

/******************************** IR  SENSORS ********************************/
const int IRSensorPin1 = 4;   // D4 (GPIO 4)
const int IRSensorPin2 = 17;  // TX2 (GPIO 17)

// Global previous IR Sensor values (Checks if a car is entering or exitting)
int lastIRSensorValue1 = HIGH;  // All initiated at HIGH, meaning off
int lastIRSensorValue2 = HIGH;


/*********************************** SERVO ***********************************/
const int ServoPin = 16;     // RX2 (GPIO 16)

Servo myServo;        // create Servo object to control a servo
int servo_pos = 0;    // varioable to store the servo servo_position
int target_pos = 90;  // Limit at an angle of 90 degrees

unsigned long last_servo_millis = 0;  // Global previous time value for servo
const int servo_move_interval = 1;    // Time to wait between steps (1 ms)
int servo_steps = 5;                  // Steps that influence the speed of the opening (can vary)


/************************************ LCD ************************************/
const int SDA_PIN = 21;       // D21 (SDA)
const int SCL_PIN = 22;       // D22 (SCL)

LiquidCrystal_I2C lcd(0x27, 16, 2);   // Initialize LCD at address 0x27


/******************************** WIFI & MQTT ********************************/
// Wi-fi
const char *ssid = "MyWiFi";            // WiFi Name
const char *password = "MyPassword";  // WiFi Password

// Broker
const char *mqtt_broker = "broker.emqx.io";
const int mqtt_port = 1883;
const char *spots_topic = "lance/parking/spots";
const char *gate_topic = "lance/parking/gate";
const char *mqtt_username = "emqx";
const char *mqtt_password = "public";

WiFiClient wifiClient;
PubSubClient client(wifiClient);

int cars_parked = 0;


/***********************************************************************************************************/
/********************************************* MAIN FUNCTIONS **********************************************/
/***********************************************************************************************************/

/************************* INITIALIZATION OF SENSORS *************************/
void setup() 
{
  /***************** WIFI & MQTT *****************/
  Serial.begin(115200);   // 115200 baud rate
  WiFi.begin(ssid, password);

  // Test WiFi connection
  while (WiFi.stats() != WL_CONNECTED) {
    delay(500);
    Serial.println("Connecting to Wi-Fi...");
  }
  Serial.println("Successfully connected to the Wi-Fi! :)");

  // Connecting to the MQTT broker
  client.setServer(mqtt_broker, mqtt_port);
  client.setCallback(callback);

  connectMQTT();


  /****************** IR SENSOR ******************/
  pinMode(IRSensorPin1, INPUT);  // Set IR sensor pin as input
  pinMode(IRSensorPin2, INPUT);  // Set IR sensor pin as input


  /******************** SERVO ********************/
  // attaches the servo on pin 16 (RX2, GPIO16) to the Servo object
  myServo.attach(ServoPin, 500, 2400);
  myServo.write(servo_pos);


  /********************* LCD *********************/
  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  // Print a message to the LCD.
  lcd.print("Hello, World!");
}


/***************** FUNCTION FOR CHECKING IF CAR ENTERED/EXIT *****************/
void loop() {
  int IRSensorValue1 = digitalRead(IRSensorPin1);
  int IRSensorValue2 = digitalRead(IRSensorPin2);

  lcd.setCursor(0, 1);


  /**************** SERVO AND  IR ****************/
  unsigned long servo_millis = millis();

  // If a car enters
  if (IRSensorValue1 == LOW || IRSensorValue2 == LOW) {
    target_pos = 90;  // Open
  }
  // If a car leaves
  else {
    target_pos = 0;   // Close
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
    myServo.write(servo_pos);
  }


  /***************** WIFI & MQTT *****************/
  if (!client.connected()) {
    connectMQTT();
  }
  client.loop();
}


/******************** FUNCTION FOR CONNECTING TO THE MQTT ********************/
void connectMQTT() {
  while (!client.connected()) {
    String client_id = "esp32-client-";
    client_id += String(WiFi.macAddress());

    Serial.println("Connecting to EMQX as:");
    Serial.println(client_id);

    if (client.connect(client_id.c_str(), mqtt_username, mqtt_password)) {
      Serial.println("Connected to the public broker");
    }
    else {
      Serial.print("Failed. State:");
      Serial.println(client.state());
      delay(2000);
    }

    // Publish and subscribe
    client.subscribe(spots_topic);
    client.publish(gate_topic, "ESP32 Connected :)")
  }

}


/************************** FUNCTION FOR  CALLBACKS **************************/
void callback(char *topic, byte *payload, unsigned int length) {
  Serial.print("Message arrived in topic: ");
  Serial.println(topic);

  Serial.print("Message:");
  for (int i = 0; i < length; i++) {
    Serial.print((char) payload[i]);
  }
  Serial.println();
  Serial.println("--------------------");
}
