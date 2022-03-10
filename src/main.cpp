// use board wemos lolin32

#include <Arduino.h>
#include "functions.h"

#define uS_TO_S_FACTOR 1000000ULL  // Conversion factor for micro seconds to seconds
#define TIME_TO_SLEEP  565        // 565 seconds == 9 minutes and 25 seconds
// Comment out the following after the program has been run one time
// That will prevent writing the networkID and node address to EEPROM every run
// #define LORA_SETUP_REQUIRED
// Network must be the same (2) for all of my units to communicate w/ each other
#define NETWORK_ID 2
// Address must be 1 for the base station to recognize it as Bessie1. Stored in EEPROM.
// Address must be 2 for Bessie2, and 3 for Boat.
#define NODE_ADDRESS 2

byte LoRaPin = 15;

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(LoRaPin, OUTPUT);
  Serial.begin(115200); // for Serial Monitor display of debug messages
  // Establish the serial connection to the LoRa
  // tx = 17 rx 16
  Serial2.begin(115200);
  readReply();
  delay(500);
  digitalWrite(LoRaPin, HIGH); // turn on the LoRa radio via transistor
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

  // read the battery voltage and send it to the base station
  sendBattStatus(getBattVolts());
  delay(2000);
 
  // turn off the LoRa to save battery
  digitalWrite(LoRaPin, LOW); 
  // Get ready to "hibernate"
  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
  prepareHibernation();
  esp_deep_sleep_start();
  
} // setup()

void loop() {} // does nothing
