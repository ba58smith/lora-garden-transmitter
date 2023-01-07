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
    WaterVolumeSensor(uint8_t pin) : analog_reader_ (data_pin_), data_pin_{pin} 
    {}

    /**
     * @brief Gets an averaged mV reading from the sensor, then converts
     * it to a water volume value (a float indicating gallons in the tub).
     * 
     * The Milone eTape liquid level sensor is a variable resistor, so by reading
     * the voltage out, and knowing the value of the other resistor, you know the
     * resistance of the sensor, and that can be converted to the water level, which
     * can be converted to the water volume.
     */
    float reported_water_volume() {
        float measured_voltage = analog_reader_.read_avg_mV() * 1000;
        float measured_R1_ohms = analog_reader_.calculated_R1_value(measured_voltage);
        float measured_inches = measured_R1_ohms / ETAPE_OHMS_PER_INCH;
        return (measured_inches * TUB_GALLONS_PER_INCH) + GALLONS_BELOW_ETAPE;
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
