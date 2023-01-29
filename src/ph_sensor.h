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
     * @brief Gets an averaged mV reading from the sensor, then converts
     * it to a pH value, expressed as a float. 
     */
    float reported_pH() {
        float pH = analog_reader_.read_avg_mV(4, 500); //BAS: is there any limit on how fast you can, or should, read the sensor?
        pH = pH / 1000; // convert mV to V
        return (pH * -5.6548) + 15.509; // per https://files.atlas-scientific.com/Gravity-pH-datasheet.pdf
    }
};

#endif // _PH_SENSOR_H_
