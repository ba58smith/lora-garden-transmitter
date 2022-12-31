// Use board "wemos lolin32"
/* 
This is the version of the LoRa Transmitter that transmits data
from the swimming pool: water temp, alarms for high water and low
water, and the voltage of the battery powering the unit.

- Keep the battery voltage sensing / sending (modify if necessary for it to be
  a 3.7v battery, not 12v). (See all BAS's in config.h)
*/

#include <Arduino.h>
#include "functions.h"
#include "reyax_lora.h"
#include "analog_reader.h"
#include "one_wire.h"

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
uint8_t lora_pin = 15;
uint8_t voltage_measurement_pin = 32;
uint8_t one_wire_pin = 18;

/* Variables holding alarm status of float pins just before putting the
   ESP32 into deep sleep. If either pin is HIGH at that time, need to
   set this variable to TRUE, so when the ESP32 wakes up, it can cancel
   the alarm condition if neither float is still HIGH.
 * They are stored in RTC memory using RTC_DATA_ATTR, which
 * maintains their value when ESP32 goes into deep sleep.
 */
RTC_DATA_ATTR static bool high_water_before_sleep = false;
RTC_DATA_ATTR static bool low_water_before_sleep = false;

ReyaxLoRa lora(lora_pin);
ESP32VoltageSensor voltage_sensor(voltage_measurement_pin);
TempSensor temp_sensor(one_wire_pin);

void setup() {  
  Serial.begin(115200);
  lora.initialize();
  temp_sensor.initialize();

#ifdef LORA_SETUP_REQUIRED
  lora.one_time_setup();
#endif

  // Send the battery voltage
  lora.send_voltage_data(voltage_sensor.reported_voltage());

  // Send the water temperature from the 1-Wire sensor
  lora.send_temperature_data(temp_sensor.reported_temperature());

  // Check for high water and send the appropriate alarm packet
  if (LOW_WATER_FLOAT_PIN == HIGH) {
    lora.send_float_data(ReyaxLoRa::LOW_WATER_FLOAT, HIGH); // send active alarm
    }
  else { // LOW_WATER_FLOAT_PIN is LOW
    if (low_water_before_sleep) // alarm was active prior to last hibernate()
    {
      lora.send_float_data(ReyaxLoRa::LOW_WATER_FLOAT, LOW); // deactivate alarm
    }
  }

  // Check for low water and send appropriate alarm packet
  if (HIGH_WATER_FLOAT_PIN == HIGH) {
    lora.send_float_data(ReyaxLoRa::HIGH_WATER_FLOAT, HIGH); // send active alarm
    }
  else { // LOW_WATER_FLOAT_PIN is LOW
    if (high_water_before_sleep) // alarm was active prior to last hibernate()
    {
      lora.send_float_data(ReyaxLoRa::HIGH_WATER_FLOAT, LOW); // deactivate alarm
    }
  }  
  
  // Don't turn it off too soon
  delay(2000);
 
  // Turn off the LoRa to save battery
  lora.turn_off();
  
  // Put the ESP32 into Deep Sleep
  hibernate();
  
} // setup()

void loop() {} // does nothing
