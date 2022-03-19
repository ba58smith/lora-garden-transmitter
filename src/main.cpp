// Use board "wemos lolin32"

#include <Arduino.h>
#include "functions.h"
#include "reyax_lora.h"
#include "analog_reader.h"

#define uS_TO_S_FACTOR 1000000ULL  // Conversion factor for micro seconds to seconds

// If you change the NETWORK_ID (below) or NODE_ADDRESS (in config.h):
// Un-comment "#define LORA_SETUP_REQUIRED", upload and run once, then comment out "#define LORA_SETUP_REQUIRED".
// That will prevent writing the NETWORK_ID and NODE_ADDRESS to EEPROM every run.
//#define LORA_SETUP_REQUIRED

// Un-comment only one of the following transmitters, to set all the configuration variables
// for that transmitter (defined in config.h).
#define BESSIE_1
//#define BESSIE_2
//#define BOAT
#include "config.h"

// Used to power the LoRa radio on or off.
uint8_t lora_pin = 15;

uint8_t voltage_measurement_pin = 32;

ReyaxLoRa lora(lora_pin, NETWORK_ID, NODE_ADDRESS, BASE_STATION);

ESP32AnalogReader voltage_sensor(voltage_measurement_pin);

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  
  // For Serial Monitor display of debug messages
  Serial.begin(115200);

  lora.initialize();

#ifdef LORA_SETUP_REQUIRED
  lora.one_time_setup();
#endif

  // Read the battery voltage and send it to the base station
  // But first, calibrate the ADC
  voltage_sensor.configure();
  float voltage = voltage_sensor.read();
  
  // Reverse the effect of the voltage divider circuit
  voltage = voltage_multiplier(voltage, R1_VALUE, R2_VALUE);
  
  // Apply the final calibration adjustment
  voltage = voltage * VOLTAGE_CALIBRATION;
  
  // And send the value to the LoRa to transmit
  lora.send_data(voltage);
  
  // Don't turn it off too soon
  delay(2000);
 
  // Turn off the LoRa to save battery
  lora.turn_off(); 
  
  // Get ready to "hibernate"
  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
  prepare_hibernation();
  esp_deep_sleep_start();
  
} // setup()

void loop() {} // does nothing
