// Use board "ESP32 DEVKIT V1"
/* 
This is the version of the LoRa Transmitter that transmits data
from the Tower Garden, and also operates the Tower Garden.
Data sent: Water level, pH level, battery voltage, info about last auto-fill
Tower Garden functions: operates the circulation pump and fill pump
*/

#include <Arduino.h>
#include "functions.h"
#include "reyax_lora.h"
#include "analog_reader.h"

/**
 * Before building, look at all of the #define options in config.h. At the very least,
 * make sure you un-comment the correct #define for the name of the transmitter that this code is
 * going to be used for.
 * 
 * If you change the #defined transmitter or the NETWORK_ID (in config.h):
 * Un-comment "#define LORA_SETUP_REQUIRED" below, upload and run once, then comment out "#define LORA_SETUP_REQUIRED".
 * That will prevent writing the NETWORK_ID and NODE_ADDRESS to EEPROM every run.
 */
//#define LORA_SETUP_REQUIRED

// Used to power the LoRa radio on and off.
uint8_t lora_pin = 15; // BAS: verify all of these pins - the board has changed!
uint8_t voltage_measurement_pin = 13;

ReyaxLoRa lora(lora_pin);
ESP32VoltageSensor voltage_sensor(voltage_measurement_pin);

void setup() {  
  Serial.begin(115200);
  lora.initialize();

#ifdef LORA_SETUP_REQUIRED
  lora.one_time_setup();
#endif

  // Send the battery voltage
  lora.send_voltage_data(voltage_sensor.reported_voltage());

  // Send the pH level from the pH sensor
  lora.send_pH_data(9.9); // BAS: replace 9.9 with a function call

  // Don't turn it off too soon
  delay(2000);
 
  // Turn off the LoRa to save battery
  lora.turn_off();
  
  // Put the ESP32 into Deep Sleep
  hibernate();
  
} // setup()

void loop() {} // does nothing
