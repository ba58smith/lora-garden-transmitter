#ifndef _PH_SENSOR_H_
#define _PH_SENSOR_H_

#include "Arduino.h"
#include "config.h"

class TempSensor {
private:
    uint8_t data_pin_;

public:
    // Constructor for the pH sensor instance. 
    TempSensor(uint8_t pin) : data_pin_{pin} {
        // BAS: do stuff here
    }

    /**
     * @brief Initializes the DallasTemperature sensor
     */

    void initialize() {
        // BAS: do stuff here? May not need to.
    }

    float reported_pH() {
        // BAS: do stuff here
    }
};

#endif // _PH_SENSOR_H_
