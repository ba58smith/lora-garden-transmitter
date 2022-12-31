#ifndef _CONFIG_H_
#define _CONFIG_H_

#define uS_TO_S_FACTOR 1000000ULL  // Used to determine how long to deep sleep the ESP32


// Network must be the same (14) for all of my units to communicate w/ each other.
#define LORA_NETWORK_ID 14

// Used to set the recipient address for all LoRa transmissions.
// My base station's address is 2200. Valid transmitter addresses are 2201-2240.
#define LORA_BASE_STATION_ADDRESS 2200UL

// Un-comment and change the baud rate below to change it.
// #define LORA_BAUD_RATE 115200ULL     // default 115200

// Configure each of the variables below for each transmitter

String TRANSMITTER_NAME = "Garden";
#define TIME_TO_SLEEP 890 // 1800 seconds == 30 minutes
#define LORA_NODE_ADDRESS 2205UL // Bessie=2201, Boat=2202, Test=2203, Pool=2204, Garden=2205
// Measured values of the two voltage divider resistors BAS: calculate and measure these
#define R1_VALUE 9400.0 //BAS: change these to actual measured values
#define R2_VALUE 2144.0
// This will be different for each transmitter device, and must be calculated from actual
// measurements taken of the source voltage, to get the final voltage correct. Calibrate
// at normal battery voltage for known input voltage.
#define VOLTAGE_CALIBRATION 1.038  // BAS: to do
#define HIGH_WATER_FLOAT_PIN 34
#define VOLTAGE_ALARM_RANGE_LOWER 3.60 //BAS: change this, and RANGE_UPPER, for a LiFePO4
#define VOLTAGE_ALARM_RANGE_UPPER 4.20
#define VOLTAGE_ALARM_CODE 11 // Not a very important alarm
#define VOLTAGE_ALARM_EMAIL_THRESHOLD 240 // In MINUTES
#define PH_ALARM_RANGE_LOWER 1.0 // BAS: change these
#define PH_ALARM_RANGE_UPPER 9.0
#define PH_ALARM_CODE 11
#define PH_ALARM_EMAIL_THRESHOLD 240 // BAS: change this

#endif // #ifndef _CONFIG_H_
