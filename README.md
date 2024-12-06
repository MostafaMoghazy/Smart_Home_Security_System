# Smart Home Security System

## Description:
This project is a smart home security system built using an Arduino Uno and several sensors to monitor for different environmental hazards. The system can detect fire, water leakage, sound levels, proximity, and tampering events. It provides alerts using an LCD display, RGB LED, buzzer, and serial communication.

## Features:
- **Sound Detection:** Alerts when a noise threshold is exceeded.
- **Fire Detection:** Monitors for flame presence with a flame sensor.
- **Water Detection:** Monitors for water leakage with a water sensor.
- **Proximity Detection:** Monitors for nearby objects using an ultrasonic sensor.
- **Tamper Detection:** Uses light level sensing to detect tampering.
- **Alerts:** Visual (LCD and RGB LED), audio (buzzer), and serial communication.
- **Self-Test:** A self-test function to verify system operation on startup.

## Components Used:
- Arduino Uno
- Sound sensor
- Flame sensor
- Water sensor
- Light-dependent resistor (LDR) sensor
- Ultrasonic distance sensor
- RGB LED
- Buzzer
- LCD display (16x2)
- Wires and resistors

## Pin Configuration:
- Sound Sensor: A0
- Flame Sensor: A4
- Water Sensor: A2
- Light Sensor (LDR): A3
- Ultrasonic Trigger Pin: 3
- Ultrasonic Echo Pin: 4
- RGB LED: Red (7), Green (8), Blue (9)
- Buzzer: 6
- LCD: Pins 5, 2, 10, 11, 12, 13

## Setup Instructions:
1. Connect all components to the corresponding pins on the Arduino Uno as described above.
2. Upload the code to the Arduino using the Arduino IDE.
3. Open the Serial Monitor to view real-time alerts and system statuses.
4. Upon startup, the system will perform a self-test to verify that all sensors and outputs are working correctly.

## Alerts:
- **Critical Alerts** (Fire, Proximity, Water): Trigger LCD, RGB LED, and buzzer alerts.
- **Moderate Alerts** (Sound, Tamper): Display messages on the LCD and log to Serial Monitor.
