#ifndef _FUNCTIONS_H_
#define _FUNCTIONS_H_

#include <Arduino.h>
#include <esp_sleep.h>
#include <driver/rtc_io.h> // for the rtc_gpio_etc. functions
#include <esp_wifi.h>
#include <driver/adc.h>
#include "config.h"

/**
 *  BAS: put this into a new Class called Hardware? (and break it up
 *       into more methods?)
 * 
 *  @brief - hibernate() tells the ESP how to wake up from deep sleep, and
 *  all the things to turn off when the "deep sleep" command is issued,
 *  then it issues that command.
 */

void hibernate() {
  // esp_deep_sleep_start() powers down everything not needed for whatever wakeup sources are enabled. But you can
  // override that with esp_sleep_pd_config().
  esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_SLOW_MEM, ESP_PD_OPTION_OFF); //_OFF causes SLOW_MEM to be powered down. Won't they power down w/o this, since my wakeup methods don't need SLOW_MEM?
  esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_FAST_MEM, ESP_PD_OPTION_OFF); //_OFF causes FAST_MEM to be powered down. Needed?
  // esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_PERIPH, ESP_PD_OPTION_ON); // to use internal pull-down resistors during deep sleep. (Not needed once I add PD resistors to the PCB.)
  
  esp_wifi_stop(); // Espressif docs say to do this to ensure the lowest power consumption during deep sleep
  // may need to put esp_wifi_start() in startup(), or it may cause a crash (only if I ever start using wifi to get the date and time)
  adc_power_release(); // Ditto

  // Enable EXT1 wakeup only if both float pins are LOW - or the ESP will immediately wake up again
  // Enable timer_wakeup for the normal time (TIME_TO_SLEEP)
  if (digitalRead(HIGH_WATER_FLOAT_PIN == LOW) && digitalRead(LOW_WATER_FLOAT_PIN == LOW)) {
    esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
    const uint64_t high_water_float_pin_bit_mask = 1ULL << HIGH_WATER_FLOAT_PIN;
    const uint64_t low_water_float_pin_bit_mask = 1ULL << LOW_WATER_FLOAT_PIN;
    esp_sleep_enable_ext1_wakeup(high_water_float_pin_bit_mask | low_water_float_pin_bit_mask, ESP_EXT1_WAKEUP_ANY_HIGH);
    // https://github.com/espressif/esp-idf/blob/1657db56c9a599403595fbf98e3eef6ad627a5d4/examples/system/deep_sleep/main/deep_sleep_example_main.c
  }
  else { // One of the float pins is HIGH, so wakeup in 3 minutes to check the status, and send another alarm/email if still HIGH
    esp_sleep_disable_wakeup_source(ESP_SLEEP_WAKEUP_EXT1);
    esp_sleep_enable_timer_wakeup(180 * uS_TO_S_FACTOR);
  }
  
  rtc_gpio_pullup_dis(GPIO_NUM_34); // hi water float switch: RTC_GPIO4 is GPIO_NUM_34rtc_gpio_pulldown_en(GPIO_NUM_34); // hi water float switch: RTC_GPIO4 is GPIO_NUM_34
  rtc_gpio_pulldown_en(GPIO_NUM_34);
  // rtc_gpio_isolate(GPIO_NUM_34); BAS: enable this when there is a physical pulldown resistor, to save battery?
  rtc_gpio_pullup_dis(GPIO_NUM_35); // low water float switch: RTC_GPIO5 is GPIO_NUM_35
  rtc_gpio_pulldown_en(GPIO_NUM_35);
  // rtc_gpio_isolate(GPIO_NUM_35); BAS: enable this when there is a physical pulldown resistor, to save battery?
  esp_deep_sleep_start();
}

#endif // _FUNCTIONS_H_
