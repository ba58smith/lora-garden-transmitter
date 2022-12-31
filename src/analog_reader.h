#ifndef _analog_reader_H_
#define _analog_reader_H_

#include <Arduino.h>
#include "config.h"
#include "esp_adc_cal.h"

/**
 * @brief ESP32VoltageSensor does all of the calibration of the ESP32's ADC, using the
 * Vref of the specific chip. If you don't do this, the values from analogRead() are
 * all over the place!
 * 
 * Also does the reading of the value, in mV, and all the manipulations necessary to
 * convert the raw voltage read into the voltage to be reported to the base station.
 * 
 * Copied from SensESP - thanks, @mairas!
 */

class ESP32VoltageSensor {
 private:
  uint8_t voltage_pin_;
  uint16_t voltage_samples_ = 30;
  uint16_t sample_delay_ms_ = 50;
  adc_atten_t attenuation_ = ADC_ATTEN_DB_11;
  adc_bits_width_t bit_width_ = ADC_WIDTH_BIT_12;
  // maximum voltage readout for 3.3V VDDA when attenuation_ is set to 11 dB
  const float kVmax_ = 3300;
  int output_scale_;
  int8_t adc_channel_;
  esp_adc_cal_characteristics_t adc_characteristics_;
  const int kVref_ = 1100;  // voltage reference, in mV

 public:
  ESP32VoltageSensor(uint8_t pin) : voltage_pin_{pin} {
    if (!(32 <= voltage_pin_ && voltage_pin_ <= 39)) {
      Serial.println("Only ADC1 is supported at the moment");
      adc_channel_ = -1;
      return;
    }
    adc_channel_ = digitalPinToAnalogChannel(voltage_pin_);
    configure();
  }

  /**
   * @brief Calibrate the ADC for this specific ESP32
   * 
   * @return false for an invalid pin, otherwise true
   */

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

  /**
   * @brief Reads the voltage on the voltage_pin one time only. Average
   *        some number of this reading with read_avg_voltage() before sending
   *        voltage to the base station.
   * 
   * @return float 
   */
  
  float read_voltage() {
    uint32_t voltage_in_mV;
    esp_adc_cal_get_voltage((adc_channel_t)adc_channel_, &adc_characteristics_,
                            &voltage_in_mV);
    return voltage_in_mV / 1000.0; 
  }

  /**
   * @brief Returns an average of read_voltage() measurements.
   * 
   * @param num_samples Number of samples to read and average.
   * 
   * @param ms_delay Milliseconds to delay between the samples.
   */

  float read_avg_voltage(uint16_t num_samples, uint16_t ms_delay) {
    float voltage = 0;
    for (uint16_t x = 0; x < num_samples; x++) {
      voltage += read_voltage();
      delay(ms_delay);
    }
    voltage = voltage / num_samples;
    return voltage;
  }

  /**
 * @brief - voltage_multiplier() - reverses the effect of a physical voltage divider.
 * 
 * @param - measured_volts is the voltage read by the AnalogIn pin, which is obviously
 * AFTER the voltage divider circuit.
 * 
 * @param - R1 and R2 are the actual measured values of the two physical resistors
 * in the voltage divider circuit.
 */

  float voltage_multiplier(float measured_volts, int R1, int R2) {
    return measured_volts * (R1 + R2) / R2;
  }

  /**
   * @brief returns the voltage to be reported to the base station, after any and
   * all manipulations necessary. R1_VALUE, R2_VALUE, AND VOLTAGE_CALIBRATION all
   * come from config.h. 
   */

  float reported_voltage() {
    float final_voltage = read_avg_voltage(voltage_samples_, sample_delay_ms_);
    final_voltage = voltage_multiplier(final_voltage, R1_VALUE, R2_VALUE);
    final_voltage = final_voltage * VOLTAGE_CALIBRATION;
    return final_voltage;
  }

};

#endif // _analog_reader_H_