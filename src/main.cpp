#include <Arduino.h>
#include <Arduino_GFX_Library.h>
#include "Arduino_CO5300.h" // Include your local fix
#include <XPowersLib.h>

XPowersAXP2101 PMU;
Arduino_DataBus *bus = new Arduino_ESP32QSPI(7, 6, 8, 9, 10, 11);
// The compiler will now see your local Arduino_CO5300 class
Arduino_GFX *gfx = new Arduino_CO5300(bus, 1, 0, true, 410, 502, 12, 11, 12, 11);

// void setup() {
//     Serial.begin(115200);
//     while(!Serial);
//     delay(1000);
//     Serial.println("--- IDF-LOGIC POWER SEQUENCE ---");

//     // 1. HARDWARE ENABLE (GPIO 15)
//     // In the IDF code, this is often handled by the PMU, 
//     // but on the SD-slot board, it is a physical gate.
//     pinMode(15, OUTPUT);
//     digitalWrite(15, HIGH); 

//     // 2. I2C INIT (PINS 3 & 2)
//     // We match the IDF config: Internal pullups enabled by the hardware
//     Wire.begin(15, 14, 400000); 

//     // 3. PMU WAKE (The AXP2101 logic from the IDF demo)
//     if (!PMU.begin(Wire, AXP2101_SLAVE_ADDRESS, 15, 14)) {
//         Serial.println("PMU: Failed to initialize. Check I2C Pins.");
//     } else {
//         Serial.println("PMU: Initialized. Setting ALDO/BLDO Rails...");
//         // ALDO1: Logic Power (1.8V)
//         PMU.setALDO1Voltage(1800); PMU.enableALDO1();
//         // BLDO1: AMOLED Main Power (3.3V)
//         PMU.setBLDO1Voltage(3300); PMU.enableBLDO1();
//         // DC1: Internal Core Power
//         PMU.enableDC1();
//     }

//     // 4. DISPLAY RESET (Match the timing from IDF)
//     pinMode(1, OUTPUT);
//     digitalWrite(1, LOW); 
//     delay(200); // IDF uses a longer reset pulse
//     digitalWrite(1, HIGH);
//     delay(200);

//     // 5. GFX BOOT
//     if (gfx->begin()) {
//         Serial.println("GFX: Handshake Successful.");
        
//         // Direct Command Overrides (The 'Ignite' Sequence)
//         bus->writeCommand(0x11); // Exit Sleep
//         delay(120);
//         bus->writeCommand(0x29); // Display On
        
//         gfx->fillScreen(MAGENTA); // High contrast color
//         gfx->setCursor(100, 250);
//         gfx->setTextColor(WHITE);
//         gfx->setTextSize(3);
//         gfx->println("ALIVE");
//     }
// }

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