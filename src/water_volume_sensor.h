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
    // Constructor for the pH sensor instance. 
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
     * then inches, then gallons - which is what I did.
     */
    float reported_water_volume() {
        float measured_voltage = analog_reader_.read_avg_mV(100, 25) / 1000;
        //Serial.println("Water volume voltage: " + (String)measured_voltage);
        float calculated_gallons = ((measured_voltage - LOWEST_MEASURED_VOLTAGE) / VOLTS_PER_GALLON) + LOWEST_MEASURED_GALLONS;
        Serial.println("Calculated volume: " + String(calculated_gallons, 1));
        return calculated_gallons;
    }

    /**
     * @brief Converts the time (in ms) of the running of the fill pump
     * into gallons, to be reported to the base station.
     * 
     * @param fill_timer_ms - how long the fill pump ran 
     */

    float reported_auto_fill_volume(uint64_t fill_timer_ms) {
        return fill_timer_ms / 60000 * GALLONS_PER_MINUTE;
    }
};

#endif // _WATER_LEVEL_SENSOR_H_
