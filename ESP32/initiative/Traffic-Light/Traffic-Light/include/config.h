#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

// ===========================
// SERIAL
// ===========================

constexpr uint32_t SERIAL_BAUDRATE = 115200;

// ===========================
// GPIO
// ===========================

constexpr uint8_t PIN_RED = 15;
constexpr uint8_t PIN_YELLOW = 5;
constexpr uint8_t PIN_GREEN = 4;

#endif