#include <Arduino.h>
#include <Arduino_GFX_Library.h>
#include <XPowersLib.h>
#include <Wire.h>

XPowersAXP2101 PMU;
Arduino_DataBus *bus = new Arduino_ESP32QSPI(7, 6, 8, 9, 10, 11);
Arduino_GFX *gfx = new Arduino_CO5300(bus, 1, 0, true, 410, 502, 12, 11, 12, 11);

void setup() {
    Serial.begin(115200);
    while(!Serial); 
    delay(1000);
    Serial.println("--- THE FINAL STAND ---");

    // 1. Force the hardware 'Master' enables
    // On this specific board, these pins act as the gate for the I2C bus itself
    pinMode(15, OUTPUT); digitalWrite(15, HIGH); 
    pinMode(38, OUTPUT); digitalWrite(38, HIGH);
    pinMode(48, OUTPUT); digitalWrite(48, HIGH);
    delay(500);

    // 2. Hardware Reset the AMOLED
    pinMode(1, OUTPUT);
    digitalWrite(1, LOW); delay(100);
    digitalWrite(1, HIGH); delay(100);

    // 3. I2C Initialization on the Wiki-specified pins
    Wire.begin(3, 2);
    
    // 4. Force power to the rails WITHOUT waiting for a 'Begin' success
    // Sometimes the library fails the 'Begin' check but the commands still work
    Serial.println("Sending Force-Power commands...");
    PMU.begin(Wire, 0x34, 3, 2);
    PMU.setALDO1Voltage(1800); PMU.enableALDO1();
    PMU.setBLDO1Voltage(3300); PMU.enableBLDO1();
    PMU.enableDC1();

    // 5. Initialize Graphics
    if (gfx->begin()) {
        Serial.println("GFX: Online. Sending 'Ignite' commands.");
        bus->writeCommand(0x11); // Sleep Out
        delay(120);
        bus->writeCommand(0x29); // Display On
        
        // Fill Red - uses the least power to test the panel
        gfx->fillScreen(RED); 
    }
}

void loop() {
    Serial.printf("Total PSRAM: %d bytes\n", ESP.getPsramSize());
    Serial.printf("Free PSRAM: %d bytes\n", ESP.getFreePsram());
    delay(2000);
}