// Use board "wemos lolin32"

#include <Arduino.h>
#include "functions.h"

#define uS_TO_S_FACTOR 1000000ULL  // Conversion factor for micro seconds to seconds
#define TIME_TO_SLEEP  565        // 565 seconds == 9 minutes and 25 seconds

// If you change the NETWORK_ID or NODE_ADDRESS, below:
// Un-comment "#define LORA_SETUP_REQUIRED", upload and run once, then comment out "#define LORA_SETUP_REQUIRED".
// That will prevent writing the NETWORK_ID and NODE_ADDRESS to EEPROM every run.
// #define LORA_SETUP_REQUIRED

// Network must be the same (2) for all of my units to communicate w/ each other
#define NETWORK_ID 2

// Address must be 1 for the base station to recognize it as Bessie1. Stored in EEPROM.
// Address must be 2 for Bessie2, and 3 for Boat.
#define NODE_ADDRESS 2

byte LoRaPin = 15;
byte voltageMeasurementPin = 32;

// This will be different for each hardware device, and must be calculated from actual
// measurements taken of the source voltage, to get the final voltage correct.
float voltageDividerCalibration = 6.15;

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(LoRaPin, OUTPUT);
  
  // For Serial Monitor display of debug messages
  Serial.begin(115200);
  
  // Establish the serial connection to the LoRa:
  // Serial2 is defined in HardwareSerial.cpp as txPin = 17 and rxPin = 16
  Serial2.begin(115200);
  readReply();
  delay(500);
  
  // Turn on the LoRa radio via transistor
  digitalWrite(LoRaPin, HIGH);
  delay(500);
  
  // Wake up the LoRa
  sendAndReadReply("AT");
  sendAndReadReply("AT+VER?");

#ifdef LORA_SETUP_REQUIRED
  String networkString = "AT+NETWORKID=" + String(NETWORK_ID);
  sendAndReadReply(networkString);
  String addressString = "AT+ADDRESS=" + String(NODE_ADDRESS);
  sendAndReadReply(addressString);
#endif

  // Read the battery voltage and send it to the base station
  sendLoRaData(getBatteryVoltage(voltageMeasurementPin, voltageDividerCalibration));
  delay(2000);
 
  // Turn off the LoRa to save battery
  digitalWrite(LoRaPin, LOW); 
  
  // Get ready to "hibernate"
  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
  prepareHibernation();
  esp_deep_sleep_start();
  
} // setup()

void loop() {} // does nothing
