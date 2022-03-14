#ifndef _FUNCTIONS_H_
#define _FUNCTIONS_H_

#include <Arduino.h>

/**
 * @brief - voltage_multiplier() - reverses the effect of a physical voltage divider.
 * 
 * @param - end_volts is the voltage read by the AnalogIn pin, which is obviously
 * AFTER the voltage divider circuit.
 * 
 * @param - R1 and R2 are the actual measured values of the two physical resistors
 * in the voltage divider circuit.
 */

float voltage_multiplier(float end_volts, int R1, int R2) {
  return end_volts * (R1 + R2) / R2;
}

/**
 * @brief - prepare_hibernation() tells the ESP all the things to turn off when the
 * "deep sleep" command is issued. 
 */

void prepare_hibernation() {
  esp_sleep_pd_config(ESP_PD_DOMAIN_MAX, ESP_PD_OPTION_OFF);
  esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_PERIPH, ESP_PD_OPTION_OFF);
  esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_SLOW_MEM, ESP_PD_OPTION_OFF);
  esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_FAST_MEM, ESP_PD_OPTION_OFF);
}

#endif // _FUNCTIONS_H_
