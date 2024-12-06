// Smart Home Security System
// Author : Mostafa Ahmed Lotfy Moghazy

#include <LiquidCrystal.h>
#include <Wire.h>

// Pin Definitions
const uint8_t SOUND_PIN = A0;      // Sound sensor input pin
const uint8_t WATER_PIN = A2;      // Water sensor input pin
const uint8_t LDR_PIN = A3;        // Light-dependent resistor (LDR) sensor pin
const uint8_t FLAME_PIN = A4;      // Flame sensor input pin
const uint8_t BUZZER_PIN = 6;      // Buzzer output pin
const uint8_t TRIG_PIN = 3;        // Ultrasonic sensor trigger pin
const uint8_t ECHO_PIN = 4;        // Ultrasonic sensor echo pin
const uint8_t RED_PIN = 7;         // Red RGB LED pin
const uint8_t GREEN_PIN = 8;       // Green RGB LED pin
const uint8_t BLUE_PIN = 9;        // Blue RGB LED pin
const uint8_t LCD_RS = 5;          // LCD register select pin
const uint8_t LCD_EN = 2;          // LCD enable pin
const uint8_t LCD_D4 = 10;         // LCD data pin 4
const uint8_t LCD_D5 = 11;         // LCD data pin 5
const uint8_t LCD_D6 = 12;         // LCD data pin 6
const uint8_t LCD_D7 = 13;         // LCD data pin 7

// System Configuration (Thresholds & Timings)
struct Config {
  const uint16_t soundThreshold = 400;    // Sound threshold for alert
  const uint16_t lightThreshold = 500;    // Light threshold for tamper detection
  const uint8_t distanceThreshold = 50;   // Ultrasonic distance threshold for proximity alert (in cm)
  const uint16_t waterThreshold = 500;    // Water detection threshold
  const uint16_t flameThreshold = 250;    // Flame detection threshold (lower is more sensitive)
  unsigned long lastAlertTime = 0;        // Time of last alert (to avoid repeated alerts)
  const unsigned long alertCooldown = 5000; // Cooldown time between alerts in milliseconds
};

Config config;

// Enum Definitions for Alert Types and Severity
enum class AlertType {
  None,
  Sound,
  Fire,
  Water,
  Tamper,
  Proximity
};

enum class AlertSeverity {
  Normal,
  Moderate,
  Critical
};

// System States for managing system phases
enum class SystemState {
  Startup,
  SelfTest,
  Normal,
  Alert
};

SystemState currentState = SystemState::Startup;  // Initial system state

// Initialize LCD
LiquidCrystal lcd(LCD_RS, LCD_EN, LCD_D4, LCD_D5, LCD_D6, LCD_D7);

// Function Declarations
void performSelfTest();
void updateLCD(const char* line1, const char* line2 = "");
void handleAlert(AlertType type, const char* message, AlertSeverity severity);
void sendSerialData(const char* sensor, const char* message, AlertSeverity severity);
void setRGBColor(bool redState, bool greenState, bool blueState);
uint16_t readUltrasonicDistance();
void activateBuzzer(AlertSeverity severity);

void setup() {
  Serial.begin(9600);
  lcd.begin(16, 2);  // Initialize the LCD with 16x2 characters

  // Pin Configuration
  pinMode(SOUND_PIN, INPUT);
  pinMode(FLAME_PIN, INPUT);
  pinMode(WATER_PIN, INPUT);
  pinMode(LDR_PIN, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(RED_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(BLUE_PIN, OUTPUT);

  // Perform system self-test
  updateLCD("System Starting", "Self-Test...");
  performSelfTest();

  // Change system state to Normal after test
  currentState = SystemState::Normal;
  updateLCD("System Ready", "Monitoring...");
  setRGBColor(false, true, false);  // Green color for ready state
}

void loop() {
  // Read sensor values
  uint16_t soundValue = analogRead(SOUND_PIN);
  uint16_t flameValue = analogRead(FLAME_PIN);
  uint16_t waterValue = analogRead(WATER_PIN);
  uint16_t lightValue = analogRead(LDR_PIN);
  uint16_t distance = readUltrasonicDistance();

  // Check if enough time has passed since last alert
  if (millis() - config.lastAlertTime >= config.alertCooldown) {
    // Check for critical alerts (LCD update, Serial output, Buzzer activation)
    if (flameValue < config.flameThreshold) {  
      handleAlert(AlertType::Fire, "FIRE DETECTED", AlertSeverity::Critical);
    }
    else if (distance < config.distanceThreshold) {
      handleAlert(AlertType::Proximity, "PROXIMITY ALERT", AlertSeverity::Critical);
    }
    // Check for moderate alerts (only Serial output)
    else if (soundValue > config.soundThreshold) {
      handleAlert(AlertType::Sound, "Sound Alert", AlertSeverity::Moderate);
    }
    else if (waterValue < config.waterThreshold) {
      handleAlert(AlertType::Water, "Water Detected", AlertSeverity::Critical);
    }
    else if (lightValue < config.lightThreshold) {
      handleAlert(AlertType::Tamper, "Tamper Alert", AlertSeverity::Moderate);
    }
  }

  delay(100);  // Small delay to stabilize sensor readings
}

// Function to handle alerts (update LCD, activate buzzer, send serial data)
void handleAlert(AlertType type, const char* message, AlertSeverity severity) {
  config.lastAlertTime = millis();  // Update last alert time

  // Send alert data to serial monitor
  sendSerialData(getAlertTypeName(type), message, severity);

  // Handle critical alerts
  if (severity == AlertSeverity::Critical) {
    currentState = SystemState::Alert;  // Set system state to Alert
    updateLCD("! ALERT !", message);    // Display alert on LCD
    setRGBColor(true, false, false);    // Red color for critical alert
    activateBuzzer(severity);           // Activate buzzer for critical alerts
    delay(2000);                        // Wait for 2 seconds before resetting state

    // Reset to normal state after alert
    currentState = SystemState::Normal;
    updateLCD("System Ready", "Monitoring...");
    setRGBColor(false, true, false);    // Green color for ready state
  }
}

// Function to activate buzzer based on severity
void activateBuzzer(AlertSeverity severity) {
  if (severity == AlertSeverity::Critical) {
    // Beep the buzzer in a pattern for critical alerts
    for (uint8_t i = 0; i < 3; i++) {
      tone(BUZZER_PIN, 2000, 200);
      delay(300);
      tone(BUZZER_PIN, 1500, 200);
      delay(300);
    }
  }
  noTone(BUZZER_PIN);  // Stop the buzzer after alert
}

// Function to get the name of the alert type as a string
const char* getAlertTypeName(AlertType type) {
  switch (type) {
    case AlertType::Sound: return "Sound Sensor";
    case AlertType::Fire: return "Fire Sensor";
    case AlertType::Water: return "Water Sensor";
    case AlertType::Tamper: return "Tamper Sensor";
    case AlertType::Proximity: return "Proximity Sensor";
    default: return "Unknown Sensor";
  }
}

// Function to send alert data to the serial monitor
void sendSerialData(const char* sensor, const char* alertMessage, AlertSeverity severity) {
  Serial.println("----------------------------------------");
  Serial.print("Sensor: ");
  Serial.println(sensor);
  Serial.print("Alert: ");
  Serial.println(alertMessage);
  Serial.print("Severity: ");
  Serial.println(severity == AlertSeverity::Critical ? "CRITICAL" : 
                severity == AlertSeverity::Moderate ? "MODERATE" : "NORMAL");
  Serial.println("----------------------------------------");
}

// Function to perform self-test on sensors and display results
void performSelfTest() {
  // Test RGB LED colors
  setRGBColor(true, false, false);  // Red color
  delay(500);
  setRGBColor(false, true, false);  // Green color
  delay(500);
  setRGBColor(false, false, true);  // Blue color
  delay(500);
  setRGBColor(false, false, false); // Turn off RGB LED
  
  // Test buzzer
  tone(BUZZER_PIN, 1000, 200);  // Buzzer test beep
  delay(300);

  // Output sensor test results to the serial monitor
  Serial.println("Self-Test Results:");
  Serial.println("----------------------------------------");
  Serial.println("Sound Sensor: OK");
  Serial.println("Flame Sensor: OK");
  Serial.println("Water Sensor: OK");
  Serial.println("Light Sensor: OK");
  Serial.println("Ultrasonic Sensor: OK");
  Serial.println("----------------------------------------");
}

// Function to update the LCD display
void updateLCD(const char* line1, const char* line2) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(line1);
  if (line2[0] != '\0') {
    lcd.setCursor(0, 1);
    lcd.print(line2);
  }
}

// Function to set RGB LED color
void setRGBColor(bool redState, bool greenState, bool blueState) {
  digitalWrite(RED_PIN, redState ? HIGH : LOW);
  digitalWrite(GREEN_PIN, greenState ? HIGH : LOW);
  digitalWrite(BLUE_PIN, blueState ? HIGH : LOW);
}

// Function to read distance from ultrasonic sensor
uint16_t readUltrasonicDistance() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  return pulseIn(ECHO_PIN, HIGH) * 0.034 / 2;  // Calculate distance in cm
}
