/*
 * Home Security Alarm System
 * EE108 Computing for Engineers - Assignment 01, Team #04
 * Maynooth University, October 2024
 *
 * Arduino Uno + HC-SR04 ultrasonic sensor + 16x2 I2C LCD
 * + 4x4 matrix keypad + buzzer + LED.
 *
 * Enter the passcode on the keypad and press '#' to arm or disarm.
 * While armed, the ultrasonic sensor trips the buzzer and prompts
 * for the code. Three wrong attempts latch the buzzer on.
 */

#include <Keypad.h>             // Library for keypad input
#include <LiquidCrystal_I2C.h>  // Library for LCD with I2C

// ---------- Pin definitions ----------
const byte TRIGGER_PIN = 2;  // Trigger pin for ultrasonic sensor
const byte ECHO_PIN    = 3;  // Echo pin for ultrasonic sensor
const byte LED_PIN     = 4;  // NOTE: not shown on slides - set to your wiring
const byte BUZZER_PIN  = 5;  // NOTE: not shown on slides - set to your wiring

// ---------- LCD ----------
const byte LCD_COLUMNS = 16;  // Number of columns in LCD
const byte LCD_ROWS    = 2;   // Number of rows in LCD

LiquidCrystal_I2C lcd(0x27, LCD_COLUMNS, LCD_ROWS);  // I2C address 0x27

// ---------- Keypad ----------
const byte ROWS    = 4;
const byte COLUMNS = 4;

char keys[ROWS][COLUMNS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

byte columPins[ROWS]    = {9, 8, 7, 6};      // Column pin connections
byte rowPins[COLUMNS]   = {13, 12, 11, 10};  // Row pin connections

Keypad keypad = Keypad(makeKeymap(keys), rowPins, columPins, ROWS, COLUMNS);

// ---------- State ----------
bool alarmOn = false;         // Is the alarm currently armed?
bool hasRun  = false;         // Guard so the trigger fires once, not every loop
byte tries   = 0;             // Number of incorrect attempts
String enteredPassword = "";  // Digits entered so far

// ---------- Ultrasonic sensor ----------
int readSensor() {
  long duration = 0;
  int distance  = 0;

  // Send a 10 us pulse from the trigger pin
  digitalWrite(TRIGGER_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIGGER_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIGGER_PIN, LOW);

  // Measure the time taken for the echo to return
  duration = pulseIn(ECHO_PIN, HIGH);

  // Convert round-trip time into distance in centimetres
  distance = 0.017 * duration;

  Serial.print(distance);  // Debug output
  return distance;
}

// ---------- Password check ----------
bool passwordChecker(String userPassword) {
  String PassWord = "1234";  // Hardcoded correct password

  if (PassWord.compareTo(userPassword) == 0) {
    lcd.clear();
    lcd.print("Password OK");
    enteredPassword = "";
    tries = 0;

    if (alarmOn) {
      // Disarm
      digitalWrite(LED_PIN, LOW);
      digitalWrite(BUZZER_PIN, LOW);
      alarmOn = false;
      hasRun  = false;  // Re-arm the one-shot trigger for next time
    } else {
      // Arm
      digitalWrite(LED_PIN, HIGH);
      delay(5000);  // Blocking - see README
      alarmOn = true;
    }

    delay(500);
    return true;

  } else {
    lcd.clear();
    lcd.print("Denied");
    delay(1000);
    enteredPassword = "";
    tries++;
    Serial.print(tries);

    lcd.clear();
    lcd.print("Enter password");
    lcd.setCursor(0, 1);

    if (tries >= 3) {
      digitalWrite(BUZZER_PIN, HIGH);  // Latch buzzer after 3 failures
    }
    return false;
  }
}

// ---------- Setup ----------
void setup() {
  Serial.begin(9600);

  // NOTE: pinMode calls not shown on slides - required for correct operation
  pinMode(TRIGGER_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(LED_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.print("Enter password");
  lcd.setCursor(0, 1);
}

// ---------- Main loop ----------
void loop() {
  char key = keypad.getKey();

  if (key == '#') {  // Submit the entered password
    passwordChecker(enteredPassword);
    Serial.print(key);
    return;
  }

  if (key) {  // Any other key: echo to LCD and append
    lcd.print(key);
    enteredPassword += key;
    Serial.print(key);
  }

  // Trip the alarm when armed and the sensor reading passes the threshold
  if ((readSensor() > 20) & alarmOn & !hasRun) {
    lcd.clear();
    lcd.print("Enter password");
    digitalWrite(BUZZER_PIN, HIGH);
    hasRun = true;
    lcd.setCursor(0, 1);
  }
}
