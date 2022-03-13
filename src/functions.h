#ifndef _FUNCTIONS_H_
#define _FUNCTIONS_H_

#include <Arduino.h>

/**
 * @brief - voltageMultiplier() - reverses the effect of a physical voltage divider.
 * 
 * @param - endVolts is the voltage read by the AnalogIn pin, which is obviously
 * AFTER the voltage divider circuit.
 * 
 * @param - R1 and R2 are the actual measured values of the two physical resistors
 * in the voltage divider circuit.
 */

float voltageMultiplier(float endVolts, int R1, int R2) {
  return endVolts * (R1 + R2) / R2;
}

/**
 * @brief - getBatteryVoltage() - reads the voltageMeasurementPin and converts
 * the average of 30 reads to voltage, then converts the voltage back into the original
 * measured voltage, prior to the physical voltage divider circuit.
 * 
 * @return - returns a float that is the original measured voltage.
*/


float getBatteryVoltage(uint8_t voltageReadPin, float calibrationValue) {
  double average_read_value = 0.0;
  float voltage_value = 0.0;
  
  // Calculate an avg of 30 analogRead values taken 30 ms apart
  for (int i = 0; i < 30; i++) {
    average_read_value += analogRead(voltageReadPin);
    delay(30);
  }
  average_read_value = average_read_value / 30;
  Serial.print("average_read_value: "); Serial.println(average_read_value);
  
  // Convert avg analogRead value (0 - 4095) to voltage
  voltage_value = (float)(average_read_value / 4096 * 3.3);
  Serial.print("voltage_value: "); Serial.println(voltage_value);
  
  // Convert average measured voltage to voltage before the voltage divider
  // IOW, reverse the effect of the voltage divider
  // BUT, it doesn't work, because I think there's a problem with the physical circuit,
  // based on my measurement of the actual voltage being measured and of the ohms value
  // of the physical resistors in the circuit.
  // voltage_value = voltageMultiplier(voltage_value, 9420, 2143);
  // For now, use the calculated voltageDividerCalibration (the value necessary to make the output accurate)
  voltage_value = voltage_value * calibrationValue;
  Serial.print("final voltage_value: "); Serial.println(voltage_value);
  return voltage_value;
}


/**
 * @brief - prepareHibernation() tells the ESP all the things to turn off when the
 * "deep sleep" command is issued. 
 */

void prepareHibernation() {
  esp_sleep_pd_config(ESP_PD_DOMAIN_MAX, ESP_PD_OPTION_OFF);
  esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_PERIPH, ESP_PD_OPTION_OFF);
  esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_SLOW_MEM, ESP_PD_OPTION_OFF);
  esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_FAST_MEM, ESP_PD_OPTION_OFF);  
}

#endif // _FUNCTIONS_H_
