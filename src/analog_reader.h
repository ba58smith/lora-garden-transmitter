#ifndef _analog_reader_H_
#define _analog_reader_H_

#include <Arduino.h>
#if defined(ESP32)
#include "esp_adc_cal.h"
#endif

/**
 * @brief ESP32AnalogReader does all of the calibration of the ESP32's ADC, using the
 * Vref of the specific chip. If you don't do this, the values from analogRead() are
 * all over the place!
 * 
 * Also does the reading of the value, in mV.
 * 
 * Copied from SensESP - thanks, @mairas!
 */

class ESP32AnalogReader {
 private:
  uint8_t pin_;
  adc_atten_t attenuation_ = ADC_ATTEN_DB_11;
  adc_bits_width_t bit_width_ = ADC_WIDTH_BIT_12;
  // maximum voltage readout for 3.3V VDDA when attenuation_ is set to 11 dB
  const float kVmax_ = 3300;
  int output_scale_;
  int8_t adc_channel_;
  esp_adc_cal_characteristics_t adc_characteristics_;
  const int kVref_ = 1100;  // voltage reference, in mV

 public:
  ESP32AnalogReader(uint8_t pin) : pin_{pin} {
    if (!(32 <= pin_ && pin_ <= 39)) {
      Serial.println("Only ADC1 is supported at the moment");
      adc_channel_ = -1;
      return;
    }
    adc_channel_ = digitalPinToAnalogChannel(pin_);
  }

  // This is all the calibration stuff
  bool configure() {
    if (adc_channel_ == -1) {
      return false;
    }
    adc1_config_width(bit_width_);
    adc1_config_channel_atten((adc1_channel_t)adc_channel_, attenuation_);
    esp_adc_cal_characterize(ADC_UNIT_1, attenuation_, bit_width_, kVref_,
                             &adc_characteristics_);
    return true;
  }

  float read() {
    uint32_t voltage_in_mV;
    esp_adc_cal_get_voltage((adc_channel_t)adc_channel_, &adc_characteristics_,
                            &voltage_in_mV);
    return voltage_in_mV / 1000.0; 
  }
};

#endif // _analog_reader_H_