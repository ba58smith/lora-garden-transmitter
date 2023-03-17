#ifndef _WATER_LEVEL_SENSOR_H_
#define _WATER_LEVEL_SENSOR_H_

#include "Arduino.h"
#include "config.h"
#include "analog_reader.h"

class WaterVolumeSensor {
private:
    uint8_t data_pin_;
    ESP32AnalogReader analog_reader_;

public:
    // Constructor for the water volume sensor instance. 
    WaterVolumeSensor(uint8_t pin) : data_pin_{pin}, analog_reader_(data_pin_) 
    {}

    /**
     * @brief Gets an averaged mV reading from the sensor, then converts
     * it to a water volume value (a float indicating gallons in the tub).
     * 
     * The Milone eTape liquid level sensor is a variable resistor, so by reading
     * the voltage out, and knowing the value of the other resistor, you know the
     * resistance of the sensor, and that can be converted to the water level, which
     * can be converted to the water volume.
     * 
     * However, it's just as simple to convert the measured voltage to gallons, w/o converting to ohms,
     * then inches, then gallons, so this function takes the simpler approach.
     */
    float reported_water_volume() {
        float measured_voltage = analog_reader_.read_avg_mV(20, 50) / 1000;
        float calculated_gallons = ((measured_voltage - LOWEST_MEASURED_VOLTAGE) / VOLTS_PER_GALLON) + LOWEST_MEASURED_GALLONS;
        return calculated_gallons;
    }
};

#endif // _WATER_LEVEL_SENSOR_H_
