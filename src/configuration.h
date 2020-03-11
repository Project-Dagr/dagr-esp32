#pragma once

#include <Arduino.h>
// -----------------------------------------------------------------------------
// Version
// -----------------------------------------------------------------------------

// If app version is not specified we assume we are not being invoked by the build script
#ifndef APP_VERSION
#define APP_VERSION "0.0.0"   // this def normally comes from build-all.sh
#define HW_VERSION "1.0 - US" // normally comes from build-all.sh and contains the region code
#endif


// -----------------------------------------------------------------------------
// Configuration
// -----------------------------------------------------------------------------
#define DEBUG_PORT Serial  // Serial debug port
#define SERIAL_BAUD 115200 // Serial debug baud rate

#define REQUIRE_RADIO true // If true, we will fail to start if the radio is not found

#define xstr(s) str(s)
#define str(s) #s

#ifdef DEBUG_PORT
#define DEBUG_MSG(...) DEBUG_PORT.printf(__VA_ARGS__)
#else
#define DEBUG_MSG(...)
#endif


#define LED_PIN 13

#define RFM95_CS 14  // "E"
#define RFM95_RST 32 // "D"
#define RFM95_INT 33 // "B"

#define RF95_FREQ 915.0