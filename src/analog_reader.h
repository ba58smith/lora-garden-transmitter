#ifndef _analog_reader_H_
#define _analog_reader_H_

#include <Arduino.h>
#include "config.h"
#include "esp_adc_cal.h"

/**
 * @brief ESP32AnalogReader does all of the calibration of the ESP32's ADC, using the
 * Vref of the specific chip. If you don't do this, the values from analogRead() are
 * all over the place!
 * 
 * Use this class in any sensor that gets its value by reading analog input.
 * 
 * Instantiates a new ADC pin, calibrates it, reads a single value, or an avg value,
 * has a method to reverse the effect of a physical voltage divider circuit, and a method
 * to determine the value of R1 in a voltage divider, when R2 and the input and output
 * voltages are known.
 * 
 * Much of this was copied from SensESP - thanks, @mairas!
 */

class ESP32AnalogReader {
 private:
  uint8_t analog_read_pin_;
  adc_atten_t attenuation_ = ADC_ATTEN_DB_11;
  adc_bits_width_t bit_width_ = ADC_WIDTH_BIT_12;
  // maximum voltage readout for 3.3V VDDA when attenuation_ is set to 11 dB
  const float kVmax_ = 3300;
  int8_t adc_channel_;
  esp_adc_cal_characteristics_t adc_characteristics_;
  const int kVref_ = 1100;  // voltage reference, in mV

 public:
  ESP32AnalogReader(uint8_t pin) : analog_read_pin_{pin} {
    if (!(32 <= analog_read_pin_ && analog_read_pin_ <= 39)) {
      Serial.println("Only ADC1 is supported at the moment");
      adc_channel_ = -1;
      return;
    }
    adc_channel_ = digitalPinToAnalogChannel(analog_read_pin_);
    calibrate();
  }

  /**
   * @brief Calibrate the ADC for this specific ESP32
   * 
   * @return false for an invalid pin, otherwise true
   */

  bool calibrate() {
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
   * @brief Reads the voltage on the voltage_pin, in mV, one time only. 
   * 
   * @return float - voltage in mV.
   */
  
  float read_mV() {
    uint32_t voltage_in_mV;
    esp_adc_cal_get_voltage((adc_channel_t)adc_channel_, &adc_characteristics_,
                            &voltage_in_mV); 
    return voltage_in_mV;
  }

  /**
   * @brief Returns an average of read_analog_raw_value() measurements.
   * 
   * @param num_samples Number of samples to read and average.
   * 
   * @param ms_delay Milliseconds to delay between the samples.
   */

  float read_avg_mV(uint16_t num_samples = 30, uint16_t ms_delay = 50) {
    float avg_mV = 0;
    for (uint16_t x = 0; x < num_samples; x++) {
      avg_mV += read_mV();
      delay(ms_delay);
    }
    return avg_mV / num_samples;
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
   * @brief Returns the value of R1 (the variable resistor in the eTape liquid level sensor),
   * with the known input voltage (3.3), the known value of R2 (ETAPE_R2_OHMS),
   * and the measured voltage from the sensor.
   * 
   * @param v_out - The voltage measured on the sensor's data pin.
   */

  float calculated_R1_value(float v_out) {
    return ETAPE_R2_OHMS * ((ETAPE_VIN / v_out) - 1.0);
  }

  };


class VoltageSensor {
 private:
  ESP32AnalogReader analog_reader_;
  

 public:
  VoltageSensor(uint8_t pin) : analog_reader_(pin) {
    //analog_reader_ = ESP32AnalogReader(pin);
    //analog_reader_(pin);
  }

  /**
   * @brief returns the voltage to be reported to the base station, after any and
   * all manipulations necessary. R1_VALUE, R2_VALUE, AND VOLTAGE_CALIBRATION all
   * come from config.h. 
   */

  float reported_voltage() {
    float final_voltage = analog_reader_.read_avg_mV(); // a value from 0 - 3300 (representing 0.0V - 3.3V)
    final_voltage = final_voltage / 1000; // convert mV to volts (0.0V to 3.3V)
    final_voltage = final_voltage * VOLTAGE_CALIBRATION; // calibrate the value
    // reverse effect of physical voltage divider, to output the actual voltage of the battery
    return analog_reader_.voltage_multiplier(final_voltage, R1_VALUE, R2_VALUE);
  }

};

#endif // _analog_reader_H_