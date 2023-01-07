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
#define TIME_TO_SLEEP 300 // 5 minutes
#define LORA_NODE_ADDRESS 2205UL // Bessie=2201, Boat=2202, Test=2203, Pool=2204, Garden=2205
// Measured values of the two voltage divider resistors BAS: calculate and measure these
#define R1_VALUE 9400.0 //BAS: change these to actual measured values
#define R2_VALUE 2144.0
// This will be different for each transmitter device, and must be calculated from actual
// measurements taken of the source voltage, to get the final voltage correct. Calibrate
// at normal battery voltage for known input voltage.
#define VOLTAGE_CALIBRATION 1.038  // BAS: to do

#define ETAPE_R2_OHMS 1595.0 // measured ohms value of black-white pins on eTape
#define ETAPE_VIN 3.3 // Voltage from ESP32 provided to the Vin pin on eTape
#define GALLONS_BELOW_ETAPE 5.0 // BAS: measure this
#define ETAPE_OHMS_PER_INCH 150 // Per the eTape datasheet
#define TUB_GALLONS_PER_INCH 0.90 // Measuring the TOP of the range, where the tub is wider //BAS: measure this
#define GALLONS_PER_MINUTE 1.30 // How many gallons per minute the fill_pump moves //BAS: measure this
#define AUTO_FILL_ALARM_CODE 1
#define AUTO_FILL_EMAIL_INTERVAL 1
#define AUTO_FILL_MAX_EMAILS 1

#define HIGH_VOLTAGE_ALARM_VALUE 14.65 //BAS: OK for a LiFePO4?
#define HIGH_VOLTAGE_ALARM_CODE 353
#define HIGH_VOLTAGE_EMAIL_INTERVAL 10 // In MINUTES
#define HIGH_VOLTAGE_MAX_EMAILS 5

#define LOW_VOLTAGE_ALARM_VALUE 13.0 //BAS: OK for a LiFePO4?
#define LOW_VOLTAGE_ALARM_CODE 1 // Not a very important alarm
#define LOW_VOLTAGE_EMAIL_INTERVAL 240 // In MINUTES (4 hours)
#define LOW_VOLTAGE_MAX_EMAILS 5

#define LOW_PH_ALARM_VALUE 5.5 // not urgent, per Fran
#define HIGH_PH_ALARM_VALUE 6.5
#define PH_ALARM_CODE 1
#define PH_ALARM_EMAIL_INTERVAL 360 // not urgent, per Fran (6 hours)
#define PH_MAX_EMAILS 5

#define LOW_WATER_ALARM_VALUE 14.0 // not urgent
#define LOW_WATER_ALARM_CODE 1
#define LOW_WATER_EMAIL_INTERVAL 360
#define LOW_WATER_MAX_EMAILS 5

#define HIGH_WATER_ALARM_VALUE 20.5 // tub c/b overflowing, stuck pump switch
#define HIGH_WATER_ALARM_CODE 333 
#define HIGH_WATER_EMAIL_INTERVAL 10
#define HIGH_WATER_MAX_EMAILS 5

#endif // #ifndef _CONFIG_H_
