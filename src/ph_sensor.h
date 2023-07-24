#ifndef _PH_SENSOR_H_
#define _PH_SENSOR_H_

#include "Arduino.h"
#include "config.h"
#include "analog_reader.h"

class pHSensor {
private:
    uint8_t data_pin_;
    ESP32AnalogReader analog_reader_;

public:
    // Constructor for the pH sensor instance. 
    pHSensor(uint8_t pin) : data_pin_{pin}, analog_reader_(data_pin_) {}


    /**
     * @brief Takes the measured voltage from the probe (in mV) and applies
     * the calibration values, to output the final value. This is from Atlas Scientific's
     * calibration software for Arduino, in the ph_gravity.cpp file.
     */

    float reported_pH() {
      float measured_mV = analog_reader_.read_avg_mV(200, 5);
      if (measured_mV > PH_MID_CAL_VOLTAGE_MV) {  // high voltage == low ph
        return 7.0 - 3.0 / (PH_LOW_CAL_VOLTAGE_MV - PH_MID_CAL_VOLTAGE_MV) * (measured_mV - PH_MID_CAL_VOLTAGE_MV);
      }
      else {
        return 7.0 - 3.0 / (PH_MID_CAL_VOLTAGE_MV - PH_HI_CAL_VOLTAGE_MV) * (measured_mV - PH_MID_CAL_VOLTAGE_MV);
      }
    }


    /**
     * @brief Measures the millivolts from the pH sensor 20 times and outputs the values to the serial monitor.
     * Used only to calibrate the pH sensor. (It's normally commented out in main.cpp.)
    */

   void pH_calibration() {
      for(uint8_t n = 0; n < 20; n++) {
        Serial.println(analog_reader_.read_avg_mV(100, 10));
      }  
   }   
  
};

#endif // _PH_SENSOR_H_
