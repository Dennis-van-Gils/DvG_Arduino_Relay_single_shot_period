/*------------------------------------------------------------------------------
Single-shot relay energizer for a programmable time period

Hardware:
  Arduino M0 Pro
  Arduino 4 relays shield

https://github.com/Dennis-van-Gils/DvG_Arduino_Relay_single_shot_period

Dennis van Gils
27-11-2020
------------------------------------------------------------------------------*/

#include <Arduino.h>
#include "DvG_SerialCommand.h"

// Instantiate serial command listener
#define Ser Serial
DvG_SerialCommand sc(Ser);

const int PIN_RELAY_1 = 4;
uint32_t energize_period = 1000; // [msec]

void setup() {
  Ser.begin(9600);

  pinMode(PIN_RELAY_1, OUTPUT);
  digitalWrite(PIN_RELAY_1, LOW);

  pinMode(PIN_LED, OUTPUT);
  digitalWrite(PIN_LED, LOW);
}

void loop() {
  char* strCmd; // Incoming serial command string
  static uint32_t tick = 0;
  static bool state_relay_1 = false;
  static bool prev_state_relay_1 = false;

  if (sc.available()) {
    strCmd = sc.getCmd();

    if (strcmp(strCmd, "?") == 0) {
      Ser.println("Single-shot relay energizer for a programmable time period");
      Ser.println("https://github.com/Dennis-van-Gils/DvG_Arduino_Relay_single_shot_period\n");
      Ser.println("Commands:");
      Ser.println("  ?     : Show this help menu");
      Ser.println("  p?    : Get the period of energizing the relay in ms");
      Ser.println("  p#### : Set the period of energizing the relay in ms");
      Ser.println("  {else}: energize the relay for the set period");

    } else if (strcmp(strCmd, "p?") == 0) {
      Ser.print("energize period [ms]: ");
      Ser.println(energize_period);

    } else if (strncmp(strCmd, "p", 1) == 0) {
      energize_period = floor(parseFloatInString(strCmd, 1));
      energize_period = constrain(energize_period, 0, 60000);

      Ser.print("energize period [ms]: ");
      Ser.println(energize_period);

    } else {
      state_relay_1 = true;
    }
  }

  if (prev_state_relay_1 != state_relay_1) {
    if (state_relay_1) {
      // Start of energizing period
      Ser.println("Energize!");
      tick = millis();
      digitalWrite(PIN_RELAY_1, HIGH);
      digitalWrite(PIN_LED, HIGH);
    } else {
      uint32_t period = millis() - tick;
      digitalWrite(PIN_RELAY_1, LOW);
      digitalWrite(PIN_LED, LOW);
      Ser.print("De-energized in ");
      Ser.print(period);
      Ser.println(" ms");
    }

    prev_state_relay_1 = state_relay_1;
  }

  if (state_relay_1 & (millis() - tick > energize_period)) {
    state_relay_1 = false;
  }
}