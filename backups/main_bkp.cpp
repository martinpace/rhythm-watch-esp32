#include <Arduino.h>
#include <Arduino_GFX_Library.h>
#include <XPowersLib.h>
#include <Wire.h>

XPowersAXP2101 PMU;

// Bus and Driver setup based on Waveshare 2.06" Specs
Arduino_DataBus *bus = new Arduino_ESP32QSPI(
    7 /* cs */, 6 /* sck */, 8 /* d0 */, 9 /* d1 */, 10 /* d2 */, 11 /* d3 */);

Arduino_GFX *gfx = new Arduino_CO5300(
    bus, 1 /* rst */, 0 /* rotation */, true /* ips */,
    410 /* width */, 502 /* height */,
    12 /* col_offset1 */, 11 /* row_offset1 */, 12 /* col_offset2 */, 11 /* row_offset2 */);

void setup() {
    Serial.begin(115200);
    while(!Serial && millis() < 5000);
    Serial.println("\n--- DEEP RESET & SLOW-CLOCK BOOT ---");

    // 1. Force Power Rails (Hardware Gates)
    pinMode(15, OUTPUT); digitalWrite(15, HIGH); // AMOLED Power Gate
    pinMode(1, OUTPUT);  digitalWrite(1, LOW);  // Reset Pull Down
    delay(100);
    digitalWrite(1, HIGH); // Reset Pull Up
    delay(100);

    // 2. Initialize I2C - Even if it failed before, we try again with a slower clock
    Wire.begin(3, 2);
    Wire.setClock(100000); 

    // 3. Force the PMU to stay awake
    if (PMU.begin(Wire, AXP2101_SLAVE_ADDRESS, 3, 2)) {
        Serial.println("PMU Found. Forcing 3.3V and 1.8V Rails...");
        PMU.setALDO1Voltage(1800); PMU.enableALDO1();
        PMU.setBLDO1Voltage(3300); PMU.enableBLDO1();
        PMU.setALDO2Voltage(2800); PMU.enableALDO2();
        PMU.setALDO3Voltage(1800); PMU.enableALDO3();
        PMU.enableDC1();
    }

    // 4. Initialize Graphics with Manual Wake-Up
    if (gfx->begin(20000000)) { // Force 20MHz here as well
        Serial.println("GFX Driver Linked.");
        
        // Manual Command Sequence to "Ignite" AMOLED
        bus->writeCommand(0x11); // Exit Sleep
        delay(150);
        bus->writeCommand(0x29); // Display ON
        delay(50);
        bus->writeCommand(0x51); // Brightness
        bus->write(0xFF);        // 100%
        
        gfx->fillScreen(WHITE); // White uses the most power, good for testing "life"
        Serial.println("White fill command sent.");
    }
}

void loop() {
    delay(1000);
    Serial.println("System Heartbeat...");
}

// #include <Arduino.h>
// #include <Arduino_GFX_Library.h>
// #include <XPowersLib.h>
// #include <Wire.h>

// XPowersAXP2101 PMU;

// Arduino_DataBus *bus = new Arduino_ESP32QSPI(
//     7 /* cs */, 6 /* sck */, 8 /* d0 */, 9 /* d1 */, 10 /* d2 */, 11 /* d3 */);

// Arduino_GFX *gfx = new Arduino_CO5300(
//     bus, 1 /* rst */, 0 /* rotation */, true /* ips */,
//     410 /* width */, 502 /* height */,
//     12 /* col_offset1 */, 11 /* row_offset1 */, 12 /* col_offset2 */, 11 /* row_offset2 */);

// // void setup() {
// //     Serial.begin(115200);

// //     // Wait for you to open the Serial Monitor
// //     // The watch will not move forward until you see the message below
// //     while (!Serial) {
// //         delay(100); 
// //     }
    
// //     Serial.println("\n--- CONNECTED: STARTING DIAGNOSTICS ---");
// //     delay(1000); // Extra second for the buffer to clear

// //     // 1. Physical Reset
// //     pinMode(1, OUTPUT);
// //     digitalWrite(1, HIGH); delay(50);
// //     digitalWrite(1, LOW);  delay(100);
// //     digitalWrite(1, HIGH); delay(50);
// //     Serial.println("1. Hardware Reset: Done.");

// //     // 2. PMU Wakeup
// //     Wire.begin(3, 2);
// //     if (!PMU.begin(Wire, AXP2101_SLAVE_ADDRESS, 3, 2)) {
// //         Serial.println("2. PMU: NOT FOUND! (Check battery/I2C)");
// //     } else {
// //         Serial.println("2. PMU: Found. Powering display rails...");
// //         PMU.setALDO1Voltage(1800); PMU.enableALDO1();
// //         PMU.setBLDO1Voltage(3300); PMU.enableBLDO1();
// //         PMU.setALDO2Voltage(2800); PMU.enableALDO2();
// //         PMU.setALDO3Voltage(1800); PMU.enableALDO3();
// //         PMU.enableDC1();
// //     }

// //     // 3. Display Wakeup
// //     if (!gfx->begin()) {
// //         Serial.println("3. GFX: FAILED to initialize driver.");
// //     } else {
// //         Serial.println("3. GFX: Driver Online.");
        
// //         // GPIO 38 is the "Power Enable" for the 2.06" AMOLED Wearable
// //         pinMode(38, OUTPUT);
// //         digitalWrite(38, HIGH); 
// //         Serial.println("GPIO 38 set HIGH (Power Enable).");

// //         // MANUALLY SET BRIGHTNESS (Some boards need this)
// //         // We use the driver-specific command for the CO5300
// //         bus->writeCommand(0x51); // Write Display Brightness command
// //         bus->write(0xFF);    // Set to 255 (100% Brightness)
        
// //         gfx->fillScreen(WHITE); // Pure white should be blindingly bright
// //         delay(1000);
// //         gfx->fillScreen(BLACK);
        
// //         gfx->setTextColor(GREEN);
// //         gfx->setTextSize(4);
// //         gfx->setCursor(60, 200);
// //         gfx->println("READY!");
// //     }
    
// //     Serial.println("--- SETUP COMPLETE: ENTERING LOOP ---");
// // }

// void setup() {
//     Serial.begin(115200);
//     while (!Serial); 
//     delay(1000);
//     Serial.println("\n--- SD-SLOT REVISION: POWERING BUS ---");

//     // 1. Enable the Peripheral Power Gates
//     // On the SD version, 15 and 48 are the master switches
//     pinMode(15, OUTPUT); digitalWrite(15, HIGH); 
//     pinMode(48, OUTPUT); digitalWrite(48, HIGH);
//     pinMode(38, OUTPUT); digitalWrite(38, HIGH);
//     pinMode(1, OUTPUT);  digitalWrite(1, HIGH); // Reset High
    
//     Serial.println("Power gates 15, 38, 48 set HIGH. Waiting for stabilization...");
//     delay(500); 

//     // 2. The most common I2C pairs for the SD-Card revision
//     int sdPairs[2][2] = {
//         {12, 11}, // Most likely for the wearable with SD
//         {3, 2}    // Standard dev board
//     };

//     bool found = false;
//     for (int i = 0; i < 2; i++) {
//         int sda = sdPairs[i][0];
//         int scl = sdPairs[i][1];
//         Serial.printf("Scanning SDA:%d SCL:%d... ", sda, scl);

//         Wire.begin(sda, scl);
//         Wire.beginTransmission(AXP2101_SLAVE_ADDRESS);
//         if (Wire.endTransmission() == 0) {
//             Serial.printf("SUCCESS! PMU found on %d/%d\n", sda, scl);
//             if (PMU.begin(Wire, AXP2101_SLAVE_ADDRESS, sda, scl)) {
//                 PMU.setALDO1Voltage(1800); PMU.enableALDO1();
//                 PMU.setBLDO1Voltage(3300); PMU.enableBLDO1();
//                 PMU.setALDO2Voltage(2800); PMU.enableALDO2();
//                 PMU.setALDO3Voltage(1800); PMU.enableALDO3();
//                 PMU.enableDC1();
//                 found = true;
//                 break;
//             }
//         }
//         Serial.println("No response.");
//     }

//     // 3. Start Graphics
//     if (gfx->begin()) {
//         Serial.println("GFX: Driver Online.");
//         gfx->fillScreen(WHITE); // Pure white test
        
//         gfx->setTextColor(BLACK);
//         gfx->setTextSize(3);
//         gfx->setCursor(50, 220);
//         gfx->println("POWER ENABLED");
//     }
// }

// void loop() {
//     Serial.println("Heartbeat...");
//     delay(2000);
// }



// #include <Arduino.h>
// #include <Arduino_GFX_Library.h>
// #include <XPowersLib.h>

// // 1. Setup Power Management (AXP2101)
// XPowersAXP2101 PMU;

// // 2. Setup Display Driver for the 2.06" AMOLED
// // These pins are specific to the Waveshare ESP32-S3-Touch-AMOLED-2.06
// Arduino_DataBus *bus = new Arduino_ESP32QSPI(
//     7 /* cs */, 6 /* sck */, 8 /* d0 */, 9 /* d1 */, 10 /* d2 */, 11 /* d3 */);

// Arduino_GFX *gfx = new Arduino_CO5300(
//     bus, 1 /* rst */, 0 /* rotation */, true /* ips */,
//     410 /* width */, 502 /* height */,
//     12 /* col_offset1 */, 11 /* row_offset1 */, 12 /* col_offset2 */, 11 /* row_offset2 */);

// void setup() {
//     // Start Serial with a massive delay to ensure Windows sees it
//     Serial.begin(115200);
//     while (!Serial && millis() < 5000); 
//     Serial.println("--- SYSTEM START ---");
//     Serial.println("Attempting I2C/PMU wake up...");

//     // Initialize Power Management - THIS IS CRITICAL
//     Wire.begin(3 /* SDA */, 2 /* SCL */);
//     if (!PMU.begin(Wire, AXP2101_SLAVE_ADDRESS, 3, 2)) {
//         Serial.println("Failed to find AXP2101 PMU");
//     } else {
//         // Turn on the power rails for the AMOLED screen
//         PMU.setALDO1Voltage(1800); // 1.8V
//         PMU.enableALDO1();
//         PMU.setBLDO1Voltage(3300); // 3.3V
//         PMU.enableBLDO1();

//         // These power the specific AMOLED circuitry inside the watch
//         PMU.setALDO2Voltage(1800); 
//         PMU.enableALDO2();
//         PMU.setALDO3Voltage(3300); 
//         PMU.enableALDO3();

//         Serial.println("PMU Initialized: Display Power ON");
//     }

//     // Initialize Graphics
//     if (!gfx->begin()) {
//         Serial.println("gfx->begin() failed!");
//     }

//     gfx->fillScreen(BLACK);
    
//     // Draw a "Rhythm Watch" welcome message
//     gfx->setCursor(60, 200);
//     gfx->setTextColor(GREEN);
//     gfx->setTextSize(4);
//     gfx->println("RHYTHM");
    
//     gfx->setCursor(60, 250);
//     gfx->setTextColor(WHITE);
//     gfx->println("WATCH");

//     Serial.println("Setup Complete!");
// }

// void loop() {
//     // Make the text blink slightly to show it's alive
//     delay(1000);
//     gfx->setCursor(60, 320);
//     gfx->setTextColor(RED);
//     gfx->setTextSize(2);
//     gfx->println("System Active");
    
//     delay(1000);
//     gfx->fillRect(60, 320, 200, 30, BLACK); // Clear the "Active" text
// }










// #include <Arduino.h>

// void setup() {
//   Serial.begin(115200);
// }

// void loop() {
//   // This sends a message every half second
//   Serial.println("HEARTBEAT - CPU IS ALIVE");
//   delay(500);
// }


//// First test app
// #include <Arduino.h>

// // put function declarations here:
// int myFunction(int, int);

// void setup() {
//   // put your setup code here, to run once:
//   //int result = myFunction(2, 3);
//   Serial.begin(115200);
//   Serial.println("Waveshare ESP32-S3 Initialized!");
// }

// void loop() {
//   // put your main code here, to run repeatedly:
//   Serial.println("Tick...");
//   delay(1000);
// }

// // put function definitions here:
// // int myFunction(int x, int y) {
// //   return x + y;
// // }