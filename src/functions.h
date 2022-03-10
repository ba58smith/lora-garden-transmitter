#ifndef _FUNCTIONS_H_
#define _FUNCTIONS_H_

#include <Arduino.h>

byte voltageMeasurementPin = 32;
float voltageDividerCalibration = 6.15;

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
 * @brief - readReply() - if delay_ms is > 0, does a delay(delay_ms), then reads the reply from an
 * AT command that was just sent, then displays it on Serial. Some of the AT commands seem to need
 * the delay before reading the reply. (AT+SEND is one of them.)
 */

void readReply(int delay_ms = 0) {
  if (delay_ms)
     delay(delay_ms);
  Serial.println(Serial2.readStringUntil('\n'));
}

/**
 * @brief - sendAndReadReply() - displays "Sending sendString", then sends
 * sendString to the LoRa. Then it delays (optionally), and reads the reply,
 * displaying it on the Serial Monitor.
 * 
 * @usage - sendString is "AT+", the AT Command, and the parameters, if any.
 * Do NOT include the "\r\n" at the end - it will be added to sendString before sending.
 * 
 * @return - void
 * 
 * @param sendString - the String you want to send to the LoRa.
 * 
 * @param delay_ms - OPTIONAL - number of ms to delay after the send and before the read. Any
 * non-zero value will cause the delay to occur.
 */

void sendAndReadReply(String sendString, int delay_ms = 0) {
  String command = sendString + String("\r\n");
  String sending = String("Sending: ") + command;
  Serial.println(sending);
  Serial2.print(command);
  readReply(delay_ms);
}

/**
 * @brief - getBattVolts() - reads the analogIn pin and converts the average of
 * 30 reads to voltage, then converts the voltage back into the original
 * measured voltage, before the physical voltage divider circuit.
 * 
 * @return - returns a float that is the original measured voltage
 * 
*/

float getBattVolts() {
  double average_read_value = 0.0;
  float voltage_value = 0.0;
  // calculate an avg of 30 analogRead values taken 30 ms apart
  for (int i = 0; i < 30; i++) {
    average_read_value += analogRead(voltageMeasurementPin);
    delay(30);
  }
  average_read_value = average_read_value / 30;
  Serial.print("average_read_value: "); Serial.println(average_read_value);
  // convert avg analogRead value (0 - 4095) to voltage
  voltage_value = (float)(average_read_value / 4096 * 3.3);
  Serial.print("voltage_value: "); Serial.println(voltage_value);
  // convert average measured voltage to voltage before the voltage divider
  // IOW, reverse the effect of the voltage divider
  // BUT, it doesn't work, because I think there's a problem with the physical circuit,
  // based on my measurement of the actual voltage being measured and of the ohms value
  // of the physical resistors in the circuit.
  // voltage_value = voltageMultiplier(voltage_value, 9420, 2143);
  // For now, use the calculated voltageDividerCalibration (the value necessary to make the output accurate)
  voltage_value = voltage_value * voltageDividerCalibration;
  Serial.print("final voltage_value: "); Serial.println(voltage_value);
  return voltage_value;
}

/**
 * @brief - Sends a value (the battery voltage) to the base station.
 * 
 * @param - voltage - the value that is to be sent to the base station.
 */

void sendBattStatus(float voltage) {
  // 10 is the address of the recipient (the base station)
  // 7 is the <Data> length.
  // 2 is Jim's network number, used by his code to "separate" my network from his.
  // The correct command below should be "AT+SEND=10,5,<voltage>", and the receiver code
  // needs to just drop the lines that deal with the so-called network ID.

  String payload = "AT+SEND=10,7,2," + String(voltage);
  sendAndReadReply(payload, 500);
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
