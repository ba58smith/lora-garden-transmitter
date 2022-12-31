#ifndef _ONE_WIRE_H_
#define _ONE_WIRE_H_

#include "Arduino.h"
#include "config.h"
#include <OneWire.h>
#include <DallasTemperature.h>

class TempSensor {
private:
    uint8_t data_pin_;
    DallasTemperature *sensor_ = NULL;

public:
    // Constructor for the 1-Wire temp sensor instance. pin is the GPIO that the "DQ" (data)
    // pin is connected to.
    TempSensor(uint8_t pin) : data_pin_{pin} {
        OneWire one_wire_ = OneWire(data_pin_);
        sensor_ = new DallasTemperature(&one_wire_);
    }

    /**
     * @brief Initializes the DallasTemperature sensor
     */

    void initialize() {
        sensor_->begin();
    }

    float reported_temperature() {
        sensor_->requestTemperatures();
        return sensor_->getTempFByIndex(0);
    }
};

#endif // _ONE_WIRE_H_
