#include <Arduino.h>

#include "config.h"

void setup()
{
    Serial.begin(SERIAL_BAUDRATE);

    pinMode(PIN_RED, OUTPUT);
    pinMode(PIN_YELLOW, OUTPUT);
    pinMode(PIN_GREEN, OUTPUT);

    Serial.println("Traffic Light Test");
}

void loop()
{
    // =====================
    // RED
    // =====================

    digitalWrite(PIN_RED, HIGH);
    digitalWrite(PIN_YELLOW, LOW);
    digitalWrite(PIN_GREEN, LOW);

    Serial.println("RED");

    delay(5000);

    // =====================
    // YELLOW
    // =====================

    digitalWrite(PIN_RED, LOW);
    digitalWrite(PIN_YELLOW, HIGH);
    digitalWrite(PIN_GREEN, LOW);

    Serial.println("YELLOW");

    delay(1000);

    // =====================
    // GREEN
    // =====================

    digitalWrite(PIN_RED, LOW);
    digitalWrite(PIN_YELLOW, LOW);
    digitalWrite(PIN_GREEN, HIGH);

    Serial.println("GREEN");

    delay(5000);
}