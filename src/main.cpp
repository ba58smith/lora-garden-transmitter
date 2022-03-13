// Use board "wemos lolin32"

#include <Arduino.h>
#include "functions.h"
#include "reyax_lora.h"

#define uS_TO_S_FACTOR 1000000ULL  // Conversion factor for micro seconds to seconds
#define TIME_TO_SLEEP  565         // 565 seconds == 9 minutes and 25 seconds

// If you change the NETWORK_ID or NODE_ADDRESS, below:
// Un-comment "#define LORA_SETUP_REQUIRED", upload and run once, then comment out "#define LORA_SETUP_REQUIRED".
// That will prevent writing the NETWORK_ID and NODE_ADDRESS to EEPROM every run.
// #define LORA_SETUP_REQUIRED

// Network must be the same (2) for all of my units to communicate w/ each other
#define NETWORK_ID 2

// Address must be 1 for the base station to recognize it as Bessie1. Stored in EEPROM.
// Address must be 2 for Bessie2, and 3 for Boat.
#define NODE_ADDRESS 2

// Used to set the recipient address for all LoRa transmissions.
// My base station's address is 10.
#define BASE_STATION 10

// Used to power the LoRa radio on or off.
uint8_t LoRaPin = 15;

uint8_t voltageMeasurementPin = 32;

// This will be different for each transmitter device, and must be calculated from actual
// measurements taken of the source voltage, to get the final voltage correct. Calibrate
// at 12.60 known input voltage.
float voltageDividerCalibration = 6.15;

ReyaxLoRa lora(LoRaPin, NETWORK_ID, NODE_ADDRESS, BASE_STATION);

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  
  // For Serial Monitor display of debug messages
  Serial.begin(115200);

  lora.initialize();

#ifdef LORA_SETUP_REQUIRED
  lora.one_time_setup();
#endif

  // Read the battery voltage and send it to the base station
  lora.sendData(getBatteryVoltage(voltageMeasurementPin, voltageDividerCalibration));
  delay(2000);
 
  // Turn off the LoRa to save battery
  lora.turnOff(); 
  
  // Get ready to "hibernate"
  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
  prepareHibernation();
  esp_deep_sleep_start();
  
} // setup()

void loop() {} // does nothing
