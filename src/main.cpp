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
#include "ph_sensor.h"
#include "water_volume_sensor.h"
#include "elapsedMillis.h"

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

uint8_t voltage_measurement_pin = 13;
uint8_t fill_pump_pin = 22;
uint8_t circ_pump_pin = 23;
uint8_t water_volume_pin = 32;
uint8_t pH_pin = 33;
uint8_t hi_water_float_pin = 34;
//uint8_t low_water_float_pin = 35; // c/b used to monitor a physical button that would wake up ESP32, and start an auto-fill (for Fran)
bool cancel_auto_fill = false;

// To stop the auto-fill with the float switch, as a fail-safe
// Stored in IRAM (Internal RAM) for maximum speed of loading and execution
void IRAM_ATTR cancel_auto_fill_isr() {
  cancel_auto_fill = true;
}

/* Variable to determine what to do / not do during this run.
 * It's stored in RTC memory using RTC_DATA_ATTR and
 * maintains its value when ESP32 goes into deep sleep.
 */
RTC_DATA_ATTR static bool measure_things_this_run = false;

ReyaxLoRa lora(0);
VoltageSensor voltage_sensor(voltage_measurement_pin);
pHSensor pH_sensor(pH_pin);
WaterVolumeSensor water_volume_sensor(water_volume_pin);

void setup() {  
  Serial.begin(115200);
  lora.initialize();
  pinMode(voltage_measurement_pin, INPUT);
  pinMode(fill_pump_pin, OUTPUT);
  pinMode(circ_pump_pin, OUTPUT);
  pinMode(water_volume_pin, INPUT);
  pinMode(pH_pin, INPUT);
  pinMode(hi_water_float_pin, INPUT);
  attachInterrupt(hi_water_float_pin, cancel_auto_fill_isr, RISING);

#ifdef LORA_SETUP_REQUIRED
  lora.one_time_setup();
#endif

  // Add the appropriate "set" method(s) here to change most of
  // the LoRa parameters, if desired. If you do, use the appropriate 
  // AT command to display the result of the change, to make sure it changed. 
  // EXAMPLE: lora->set_output_power(10);
  //          lora->send_and_reply("AT+CRFOP?");;

  measure_things_this_run = !measure_things_this_run; // to make it different each time it wakes up
  elapsedMillis timer = 0;
  digitalWrite(circ_pump_pin, HIGH);
  while (timer < (3 * 60 * 1000)) {} // run the circulation pump for 3 minutes
  digitalWrite(circ_pump_pin, LOW);

  if (measure_things_this_run) { // measure all the things

    // Send the battery voltage
    lora.send_voltage_data(voltage_sensor.reported_voltage()); // BAS: s/ there be a short delay btwn lora.sends, maybe just 500 ms?

    // Send the pH level from the pH sensor
    lora.send_pH_data(pH_sensor.reported_pH());

    // Send the water level
    lora.send_water_volume_data(water_volume_sensor.reported_water_volume());

    // fill tub if necessary, then send a packet about that
    if (water_volume_sensor.reported_water_volume() <= 16.0) {
      timer = 0;
      digitalWrite(fill_pump_pin, HIGH);
      while (water_volume_sensor.reported_water_volume() < 20.0 && !cancel_auto_fill) {
        delay(5000);
      }
      digitalWrite(fill_pump_pin, LOW);
      lora.send_auto_fill_data(water_volume_sensor.reported_auto_fill_volume(timer));
    }
  }
    // Give the last packet a couple seconds to go, then go to deep sleep for 5 minutes
    delay(2000);
    esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
    esp_deep_sleep_start();

} // setup()

void loop() {} // does nothing
