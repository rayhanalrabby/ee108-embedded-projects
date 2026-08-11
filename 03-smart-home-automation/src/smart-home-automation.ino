#include <Servo.h>  // Include the Servo library for controlling the servo motor

// Define pin numbers for different hardware components
#define SERVO_MOTOR 2
#define CURTAIN_MOTOR_UP 3
#define CURTAIN_MOTOR_DOWN 11
#define IR_SENSOR 4
#define LIGHTS1 5
#define LIGHTS2 6
#define LIGHTS3 7
#define USS_TRIG 8
#define USS_ECHO 9
#define BUZZER_PIN 10
#define LDR_PIN A0
#define WATER_SENSOR A1
#define LIGHT_PIN 12
#define SOUNDS_SENSOR_PIN A3

// Define threshold values for sensors
#define WATER_THRESHOLD 100
#define LIGHT_THRESHOLD 300
#define CLOSED_DOOR 5
#define SOUND_THRESHOLD 10

// Define flags for curtain and light states
bool curtainsup = true;
bool lightState = false;
Servo myServo;  // Create an instance of the Servo class

void setup() {
  // Set pin modes for various components (input/output)
  pinMode(CURTAIN_MOTOR_UP, OUTPUT);
  pinMode(CURTAIN_MOTOR_DOWN, OUTPUT);
  pinMode(IR_SENSOR, INPUT);
  pinMode(LIGHTS1, OUTPUT);
  pinMode(LIGHTS2, OUTPUT);
  pinMode(LIGHTS3, OUTPUT);
  pinMode(USS_TRIG, OUTPUT);
  pinMode(USS_ECHO, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(LIGHT_PIN, OUTPUT);
  Serial.begin(9600);  // Start serial communication for debugging
  myServo.attach(SERVO_MOTOR);  // Attach the servo to the defined pin
}

// Function to monitor water level and control servo motor
void waterSensor() {
  int waterLevel = analogRead(WATER_SENSOR);  // Read the water sensor value
  if (waterLevel > WATER_THRESHOLD) {
    myServo.write(0);  // Move servo to position 0 if water level exceeds threshold
  } else {
    myServo.write(60);  // Move servo to position 60 if water level is below threshold
  }
}

// Function to control lights based on motion detection from IR sensor
void lights() {
  bool movement = digitalRead(IR_SENSOR);  // Read the IR sensor for motion detection
  int data[3] = {0, 0, 0};  // Array to hold the light pattern data

  if (movement == HIGH) {  // If motion is detected
    // Loop through binary numbers 0-7 and light up lights in patterns
    for (int i = 0; i < 8; i++) {
      int arrayPosition = 0;
      // Convert binary number to a 3-bit array (for 3 lights)
      for (int j = 4; j >= 1; j = j / 2) {
        if ((i & j) > 0) {
          data[arrayPosition] = 1;
        } else {
          data[arrayPosition] = 0;
        }
        arrayPosition++;
      }
      // Control the lights based on the binary pattern
      digitalWrite(LIGHTS1, data[0]);
      digitalWrite(LIGHTS2, data[1]);
      digitalWrite(LIGHTS3, data[2]);
      delay(500);  // Wait for 500 milliseconds before changing pattern
    }
  } else {
    // If no motion is detected, turn off the lights
    digitalWrite(LIGHTS1, LOW);
    digitalWrite(LIGHTS2, LOW);
    digitalWrite(LIGHTS3, LOW);
  }
}

// Function to control curtains based on light levels
void curtains(bool &curtainsup) {
  int lightLevel = analogRead(LDR_PIN);  // Read light level from LDR sensor
  if (lightLevel > LIGHT_THRESHOLD && !curtainsup) {
    digitalWrite(CURTAIN_MOTOR_UP, HIGH);  // Raise curtains if light exceeds threshold
    delay(400);  // Delay to ensure the curtains move
    digitalWrite(CURTAIN_MOTOR_UP, LOW);
    curtainsup = true;  // Set curtainsup flag to true
  }
  if (lightLevel < LIGHT_THRESHOLD && curtainsup) {
    digitalWrite(CURTAIN_MOTOR_DOWN, HIGH);  // Lower curtains if light level drops
    delay(400);  // Delay to ensure curtains move down
    digitalWrite(CURTAIN_MOTOR_DOWN, LOW);
    curtainsup = false;  // Set curtainsup flag to false
  }
}

// Function to monitor door status using an ultrasonic sensor
void openedDoor() {
  if (readSensor() > CLOSED_DOOR) {  // If distance is greater than CLOSED_DOOR threshold
    digitalWrite(BUZZER_PIN, HIGH);  // Activate buzzer
  } else {
    digitalWrite(BUZZER_PIN, LOW);  // Turn off buzzer if door is closed
  }
}

// Function to read distance from ultrasonic sensor
int readSensor() {
  long duration = 0;
  int distance = 0;

  // Trigger the ultrasonic sensor to send a pulse
  digitalWrite(USS_TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(USS_TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(USS_TRIG, LOW);

  duration = pulseIn(USS_ECHO, HIGH);  // Measure the pulse duration
  distance = 0.017 * duration;  // Convert duration to distance in cm
  Serial.print(distance);  // Print the distance for debugging
  return distance;  // Return the calculated distance
}

// Function to control light based on sound level from sound sensor
void soundLights() {
  int soundLevel = analogRead(SOUNDS_SENSOR_PIN);  // Read sound level from sensor

  if (soundLevel > SOUND_THRESHOLD) {  // If sound level exceeds threshold
    delay(200);  // Wait for 200 milliseconds to debounce
    lightState = !lightState;  // Toggle the light state
    digitalWrite(LIGHT_PIN, lightState ? HIGH : LOW);  // Turn light on or off
  }
}

void loop() {
  waterSensor();  // Call water sensor function
  lights();  // Call motion sensor function to control lights
  curtains(curtainsup);  // Call curtains function to control curtain movement
  openedDoor();  // Call door sensor function to check for open door
  soundLights();  // Call sound sensor function to control light based on sound
}
