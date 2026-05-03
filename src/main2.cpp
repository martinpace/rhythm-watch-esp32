#include <Arduino.h>
#include <Arduino_GFX_Library.h>
#include "Arduino_CO5300.h" // Include local implementation of CO5300 driver
#include <XPowersLib.h>
#include "HWCDC.h"

HWCDC USBSerial; // Replace standard Serial with Hardware CDC
XPowersAXP2101 PMU;
Arduino_DataBus *bus = new Arduino_ESP32QSPI(7, 6, 8, 9, 10, 11);
Arduino_GFX *gfx = new Arduino_CO5300(
    bus, 1 /* RST */, 0 /* rotation */, true /* IPS */, 
    410 /* width */, 502 /* height */, 
    22 /* col_offset1 */, 0 /* row_offset1 */, 
    0  /* col_offset2 */, 0 /* row_offset2 */
);

// Modular Power Initialization
void hardware_ignition() {
    // 1. I2C INIT
    Wire.begin(15, 14); 
    // 2. PMU WAKE
    if (PMU.begin(Wire, 0x34, 15, 14)) {
        USBSerial.println("PMU: Online. Enabling AMOLED Power Rails...");
        // These are the specific voltages for the 2.06" panel
        PMU.setALDO1Voltage(1800); PMU.enableALDO1(); // Logic
        PMU.setBLDO1Voltage(3300); PMU.enableBLDO1(); // Panel
        PMU.enableDC1(); // Main Core
        delay(200); // Give the power rails time to stabilize
    }
}

void setup() {
    USBSerial.begin(115200);
    // Note: while(!USBSerial) is optional, but helps catch early boot logs
    while(!USBSerial);
    delay(1000);
    USBSerial.println("--- PMU FOUND: COMMENCING IGNITION ---");

    hardware_ignition();

    // 3. HARDWARE RESET (Crucial for OLEDs)
    pinMode(1, OUTPUT);
    digitalWrite(1, LOW);  delay(200);
    digitalWrite(1, HIGH); delay(200);

    // 4. GFX INITIALIZATION
    gfx->begin();
    gfx->invertDisplay(true); //inversion couleur

    gfx->fillScreen(BLACK);
}

void loop() {
    USBSerial.printf("System Heartbeat... - Free PSRAM: %d bytes\n", ESP.getFreePsram());
    delay(5000);
}