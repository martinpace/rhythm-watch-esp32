#include <Arduino.h>
#include <Arduino_GFX_Library.h>
#include "Arduino_CO5300.h" // Include local implementation of CO5300 driver
#include <XPowersLib.h>

XPowersAXP2101 PMU;
Arduino_DataBus *bus = new Arduino_ESP32QSPI(7, 6, 8, 9, 10, 11);
Arduino_GFX *gfx = new Arduino_CO5300(bus, 1, 0, true, 410, 502, 12, 11, 12, 11);

void setup() {
    Serial.begin(115200);
    while(!Serial);
    delay(1000);
    Serial.println("--- PMU FOUND: COMMENCING IGNITION ---");

    // 1. I2C INIT on your newly discovered pins
    Wire.begin(15, 14); 

    // 2. PMU WAKE
    if (PMU.begin(Wire, 0x34, 15, 14)) {
        Serial.println("PMU: Online. Enabling AMOLED Power Rails...");
        
        // These are the specific voltages for the 2.06" panel
        PMU.setALDO1Voltage(1800); PMU.enableALDO1(); // Logic
        PMU.setBLDO1Voltage(3300); PMU.enableBLDO1(); // Panel
        PMU.enableDC1(); // Main Core
        delay(200); // Give the power rails time to stabilize
    }

    // 3. HARDWARE RESET (Crucial for OLEDs)
    pinMode(1, OUTPUT);
    digitalWrite(1, LOW);  delay(200);
    digitalWrite(1, HIGH); delay(200);

    // 4. GFX INITIALIZATION
    if (gfx->begin()) {
        Serial.println("GFX: Driver Handshake Successful.");
        
        // 5. THE "WAKE UP" COMMANDS (Writing directly to the bus)
        // Many OLEDs stay black until they receive 'Sleep Out' and 'Display On'
        bus->writeCommand(0x11); // Sleep Out
        delay(120);              // Required delay after sleep out
        bus->writeCommand(0x29); // Display On
        
        // 6. Test Fill
        gfx->fillScreen(MAGENTA); 
        Serial.println("Action: MAGENTA fill sent to buffer.");
    }
}

void loop() {
    Serial.println("System Heartbeat...");
    delay(5000);
}