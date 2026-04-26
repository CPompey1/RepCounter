#include <Arduino.h>
#include "SerialUx.h"

UXState uxState;

void setup() {
    Serial.begin(9600);
    uxState = {UX_SERIAL,NULL,NULL}; // Initialize the UXState structure to zero
    initializeUXState(&uxState, UX_SERIAL);
    displayMenu(&uxState);
}

void loop() {
    handle_input(&uxState);
    delay(25);
}
