// Network must be the same (2) for all of my units to communicate w/ each other.
// After re-writing the receiver code, this will change to some other unique identifier.
#define NETWORK_ID 2

// Used to set the recipient address for all LoRa transmissions.
// My base station's address is 10.
#define BASE_STATION 10

// Configure each of the variables below for each transmitter

#ifdef BESSIE_1
#define TIME_TO_SLEEP  545 // 545 seconds == 9 minutes and 5 seconds
#define NODE_ADDRESS 1
// Measured values of the two voltage divider resistors
#define R1_VALUE 9400.0
#define R2_VALUE 2144.0
// This will be different for each transmitter device, and must be calculated from actual
// measurements taken of the source voltage, to get the final voltage correct. Calibrate
// at 12.60 for known input voltage.
#define VOLTAGE_CALIBRATION 1.038
#endif

#ifdef BESSIE_2
#define TIME_TO_SLEEP  560 // 560 seconds == 9 minutes and 20 seconds
#define NODE_ADDRESS 2
#define R1_VALUE 9500.0
#define R2_VALUE 2145.0
#define VOLTAGE_CALIBRATION 1.029
#endif

#ifdef BOAT
#define TIME_TO_SLEEP  575  // 575 seconds == 9 minutes and 35 seconds
#define NODE_ADDRESS 3
#define R1_VALUE ????.0
#define R2_VALUE ????.0
#define VOLTAGE_CALIBRATION 1.0??
#endif
