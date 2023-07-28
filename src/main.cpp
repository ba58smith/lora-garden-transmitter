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
bool float_sw_activated = false;

// To stop the auto-fill with the float switch, as a fail-safe
// Stored in IRAM (Internal RAM) for maximum speed of loading and execution
void IRAM_ATTR float_switch_isr() {
  float_sw_activated = true;
}

/* Variable to determine what to do / not do during this run.
 * It's stored in RTC memory using RTC_DATA_ATTR and
 * maintains its value when ESP32 goes into deep sleep.
 */
RTC_DATA_ATTR static bool measure_things_this_run = false;

/** Variable to record the fact that the auto-fill pump ran for
 * more than AUTO_FILL_CUT_OFF_TIME, so we don't keep running the
 * auto-fill every time. 
 */
RTC_DATA_ATTR static bool auto_fill_timed_out = false;

ReyaxLoRa lora(0);
VoltageSensor voltage_sensor(voltage_measurement_pin);
pHSensor pH_sensor(pH_pin);
WaterVolumeSensor water_volume_sensor(water_volume_pin);

void setup() {  
  Serial.begin(115200);
  lora.initialize();
  delay(1000); // cuts off Serial Monitor output w/o this
  pinMode(voltage_measurement_pin, INPUT);
  pinMode(fill_pump_pin, OUTPUT);
  pinMode(circ_pump_pin, OUTPUT);
  pinMode(water_volume_pin, INPUT);
  pinMode(pH_pin, INPUT);
  pinMode(hi_water_float_pin, INPUT);
  attachInterrupt(hi_water_float_pin, float_switch_isr, RISING);

#ifdef LORA_SETUP_REQUIRED
  lora.one_time_setup();
#endif

  // Add the appropriate "set" method(s) here to change most of
  // the LoRa parameters, if desired. If you do, use the appropriate 
  // AT command to display the result of the change, to make sure it changed. 
  // EXAMPLE: lora->set_output_power(10);
  //          lora->send_and_reply("AT+CRFOP?");;

  measure_things_this_run = !measure_things_this_run; // to make it different each time it wakes up
  delay(1000); // Serial.monitor needs a few seconds to get ready
  Serial.println("measure_things_this_run = " + (String)measure_things_this_run);

  if (digitalRead(hi_water_float_pin) == HIGH) { // water is getting into the tub w/o the fill pump running
    float_sw_activated = true; // the pin is HIGH, so the variable s/b true
    lora.send_auto_fill_data(0.00, "FL-SW");     // or the last auto-fill stopped w/ the float switch, and it has not been investigated
  }
  else {
    float_sw_activated = false; // the pin is LOW, so the variable s/b false
  }

  elapsedMillis timer_ms = 0;
  Serial.println("Circ pump starting");
  digitalWrite(circ_pump_pin, HIGH);
  //while (timer_ms < (5 * 1000)) {} // BAS: testing only
  while (timer_ms < (3 * 60 * 1000)) {} // run the circulation pump for 3 minutes
  Serial.println("Circ pump stopping");
  digitalWrite(circ_pump_pin, LOW);

  if (measure_things_this_run) { // measure all the things

    // Send the battery voltage
    float voltage = voltage_sensor.reported_voltage();
    Serial.println("Reported_voltage:" + (String)voltage);
    lora.send_voltage_data(voltage);
    delay(1000); // may not be necessary, but it can't hurt

    // Send the pH level from the pH sensor
    // pH_sensor.pH_calibration(); // BAS: run only when you need to calibrate the pH sensor
    float pH = pH_sensor.reported_pH();
    Serial.println("Reported_pH: " + String(pH, 1));
    lora.send_pH_data(pH);
    delay(500);

    // Send the water level
    float water_volume = water_volume_sensor.reported_water_volume();
    Serial.println("Reported_water_volume:" + (String)water_volume);
    lora.send_water_volume_data(water_volume);

    // fill tub if necessary, then send a packet about that
    if (!auto_fill_timed_out) {
      if (water_volume <= REFILL_START_VOLUME && !float_sw_activated) {
        float stop_time_secs = 0.0;
        String stop_reason;
        timer_ms = 0;
        Serial.println("Fill pump starting");
        digitalWrite(fill_pump_pin, HIGH);
        while (!float_sw_activated && water_volume_sensor.reported_water_volume() < REFILL_STOP_VOLUME
               && timer_ms < (AUTO_FILL_CUT_OFF_SECONDS * 1000.0)) {
              delay(1000); // fill a second, then check again
        }
        digitalWrite(fill_pump_pin, LOW);
        stop_time_secs = (float)timer_ms / 1000.0;
        if (stop_time_secs >= AUTO_FILL_CUT_OFF_SECONDS) {
          auto_fill_timed_out = true;
          stop_reason = "TIMER";
        }
        else {
          if (float_sw_activated) {
            stop_reason = "FL-SW";
          }
          else {
            stop_reason = "Fill";
          }
        }
        Serial.println("Fill pump stopped: " + stop_reason);
        Serial.println("Auto-fill timer (sec): " + (String)stop_time_secs);
        float fill_volume = water_volume_sensor.reported_water_volume() - water_volume;
        Serial.println("Auto-fill volume: " + (String)fill_volume);
        lora.send_auto_fill_data(fill_volume, stop_reason);
      }
    }
  }
    // Give the last packet a couple seconds to go, then go to deep sleep for 5 minutes
    delay(2000);
    Serial.println("Going to sleep now");
    esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
    esp_deep_sleep_start();

} // setup()

void loop() {} // does nothing
