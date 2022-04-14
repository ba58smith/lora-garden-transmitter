#ifndef _CONFIG_H_
#define _CONFIG_H_

// Un-comment only one of the following units, to set all the configuration variables
// for that unit (defined below).
//#define BESSIE
#define TEST
//#define BOAT

// Network must be the same (2) for all of my units to communicate w/ each other.
// After re-writing the receiver code, this will change to some other unique identifier.
#define LORA_NETWORK_ID 14

// Used to set the recipient address for all LoRa transmissions.
// My base station's address is 2200. Valid transmitter addresses are 2201-2240.
#define LORA_BASE_STATION_ADDRESS 2200UL

// Un-comment and change the baud rate below to change it.
// #define LORA_BAUD_RATE 115200ULL     // default 115200

// Configure each of the variables below for each transmitter

#ifdef BESSIE
String TRANSMITTER_NAME = "Bessie";
#define TIME_TO_SLEEP 545 // 545 seconds == 9 minutes and 5 seconds
#define LORA_NODE_ADDRESS 2201UL
// Measured values of the two voltage divider resistors
#define R1_VALUE 9400.0
#define R2_VALUE 2144.0
// This will be different for each transmitter device, and must be calculated from actual
// measurements taken of the source voltage, to get the final voltage correct. Calibrate
// at 12.60 for known input voltage.
#define VOLTAGE_CALIBRATION 1.038
#define VOLTAGE_ALARM_RANGE_LOWER 12.20
#define VOLTAGE_ALARM_RANGE_UPPER 14.00
#define VOLTAGE_ALARM_CODE 11
#endif

#ifdef TEST
String TRANSMITTER_NAME = "Test";
#define TIME_TO_SLEEP 560 // 560 seconds == 9 minutes and 20 seconds
#define LORA_NODE_ADDRESS 2202UL
#define R1_VALUE 9500.0
#define R2_VALUE 2145.0
#define VOLTAGE_CALIBRATION 1.029
#define VOLTAGE_ALARM_RANGE_LOWER 5.00
#define VOLTAGE_ALARM_RANGE_UPPER 8.00
#define VOLTAGE_ALARM_CODE 21
#endif

#ifdef BOAT
String TRANSMITTER_NAME = "Boat";
#define TIME_TO_SLEEP 575 // 575 seconds == 9 minutes and 35 seconds
#define LORA_NODE_ADDRESS 2203UL
#define R1_VALUE 9480.0
#define R2_VALUE 2158.0
#define VOLTAGE_CALIBRATION 1.020
#define VOLTAGE_ALARM_RANGE_LOWER 12.20
#define VOLTAGE_ALARM_RANGE_UPPER 14.00
#define VOLTAGE_ALARM_CODE 31
#endif

#endif // #ifndef _CONFIG_H_
