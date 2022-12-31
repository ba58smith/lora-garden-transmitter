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


  // Enable timer_wakeup for the normal time (TIME_TO_SLEEP)
  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
  esp_deep_sleep_start();
}

#endif // _FUNCTIONS_H_
