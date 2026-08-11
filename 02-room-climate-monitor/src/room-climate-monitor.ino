/*
 * Room Climate Monitor
 * EE108 Computing for Engineers - Assignment 02, Team #04
 * Maynooth University, November 2024
 *
 * Arduino Uno + TMP36 temperature sensor + 2x potentiometer
 * + 16x2 I2C LCD + 4x4 matrix keypad + RGB status LEDs.
 *
 * Press 1 for humidity, 2 for temperature, 3 for pressure.
 * The reading is shown on the LCD and an LED indicates whether the
 * value is high (red), normal (green), or low (blue).
 *
 * Built in Tinkercad: potentiometers stand in for humidity and
 * pressure sensors, which the kit did not include.
 */

#include <Wire.h>
#include <Adafruit_LiquidCrystal.h>
#include <Keypad.h>

// Set up LCD with I2C address 0x3F (most common)
Adafruit_LiquidCrystal lcd(0);  // The default I2C address for the display (0x3F)

// Keypad setup
const byte ROWS = 4; // Four rows
const byte COLS = 4; // Four columns
char keys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

byte rowPins[ROWS] = {9, 8, 7, 6};  // Row pins connected to Arduino
byte colPins[COLS] = {5, 4, 3, 2}; // Column pins connected to Arduino

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// Sensor pins
const int humidityPin = A0;  // Potentiometer for humidity
const int tempPin = A1;      // Potentiometer for temperature
const int pressurePin = A2;  // Potentiometer for pressure

// LED pins
const int redLED = 13;    // Red LED for temperature
const int greenLED = 12;  // Green LED for pressure
const int blueLED = 11;   // Blue LED for humidity

void setup() {
  Serial.begin(9600);
  
  // Initialize the LCD
  lcd.begin(16, 2);  // Set the LCD size (16 columns x 2 rows)
  lcd.setBacklight(LOW); // Turn off backlight initially
  lcd.clear();  // Clear any existing text
  
  // Set LED pins as OUTPUT
  pinMode(redLED, OUTPUT);
  pinMode(greenLED, OUTPUT);
  pinMode(blueLED, OUTPUT);
  
  // Display an initial message
  lcd.setCursor(0, 0);
  lcd.print("Sensor Project");
  lcd.setCursor(0, 1);
  lcd.print("Press 1, 2 or 3...");
  
  delay(2000); // Wait for a bit
  lcd.clear(); // Clear the display
}

void loop() {
  char key = keypad.getKey();  // Get keypress from the keypad

  if (key) {
    // Print the key to the Serial Monitor for debugging
    Serial.print("Key Pressed: ");
    Serial.println(key); // Print the key pressed to the Serial Monitor
    // Only process key '1', '2', or '3'
    if (key == '1' || key == '2' || key == '3') {
      lcd.clear();  // Clear display for new output

      // Process the keypress
      if (key == '1') {
        // Read humidity from potentiometer
        int humidityValue = analogRead(humidityPin);
        float humidity = map(humidityValue, 0, 1023, 0, 100);  // Map the value to 0-100%
        
        // Display humidity on the LCD
        lcd.setCursor(0, 0);
        lcd.print("Humidity: ");
        lcd.print(humidity);
        lcd.print(" %");

        // LED logic for humidity
        if (humidity > 80) {
          digitalWrite(blueLED, LOW);      // Turn off other LEDs
          digitalWrite(greenLED, LOW);
          digitalWrite(redLED, HIGH);      // Red LED for high humidity
        } else if (humidity > 40) {
          digitalWrite(blueLED, LOW);      // Turn off other LEDs
          digitalWrite(redLED, LOW);
          digitalWrite(greenLED, HIGH);    // Green LED for moderate humidity
        } else {
          digitalWrite(redLED, LOW);       // Turn off other LEDs
          digitalWrite(greenLED, LOW);
          digitalWrite(blueLED, HIGH);     // Blue LED for low humidity
        }
      }
      else if (key == '2') {
        // Read temperature from potentiometer
        int tempValue = analogRead(tempPin);
        float voltage = tempValue * (5.0 / 1023.0);  // Convert to voltage
        float temperature = (voltage - 0.5) * 100;   // Convert voltage to Celsius

        // Display temperature on the LCD
        lcd.setCursor(0, 0);
        lcd.print("Temperature: ");
        lcd.print(temperature);
        lcd.print(" C");

        // LED logic for temperature
        if (temperature > 30) {
          digitalWrite(blueLED, LOW);      // Turn off other LEDs
          digitalWrite(greenLED, LOW);
          digitalWrite(redLED, HIGH);      // Red LED for too high temperature
        } else if (temperature > 20) {
          digitalWrite(blueLED, LOW);      // Turn off other LEDs
          digitalWrite(redLED, LOW);
          digitalWrite(greenLED, HIGH);    // Green LED for warm temperature
        } else {
          digitalWrite(redLED, LOW);       // Turn off other LEDs
          digitalWrite(greenLED, LOW);
          digitalWrite(blueLED, HIGH);     // Blue LED for low temperature
        }
      }
      else if (key == '3') {
        // Read pressure from potentiometer
        int pressureValue = analogRead(pressurePin);
        float pressure = map(pressureValue, 0, 1023, 950, 1050);  // Map the value to 950-1050 hPa

        // Display pressure on the LCD
        lcd.setCursor(0, 0);
        lcd.print("Pressure: ");
        lcd.print(pressure);
        lcd.print(" hPa");

        // LED logic for pressure
        if (pressure > 1020) {
          digitalWrite(blueLED, LOW);      // Turn off other LEDs
          digitalWrite(redLED, HIGH);      // Red LED for high pressure
          digitalWrite(greenLED, LOW);
        } else if (pressure > 1000) {
          digitalWrite(blueLED, LOW);      // Turn off other LEDs
          digitalWrite(redLED, LOW);
          digitalWrite(greenLED, HIGH);    // Green LED for normal pressure
        } else {
          digitalWrite(redLED, LOW);       // Turn off other LEDs
          digitalWrite(greenLED, LOW);
          digitalWrite(blueLED, HIGH);     // Blue LED for low pressure
        }
      }

      delay(2000);  // Wait for 2 seconds to allow reading before clearing
      lcd.clear();  // Clear the LCD for the next input
      lcd.setCursor(0, 0);
      lcd.print("Press 1, 2 or 3...");
    } else {
      // If any other key is pressed, display "Invalid Key"
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Invalid Key");
      delay(1000);  // Wait for 1 second to show the message
      lcd.clear();  // Clear the LCD and wait for the next valid key press
      lcd.setCursor(0, 0);
      lcd.print("Press 1, 2 or 3...");
    }
  }
}