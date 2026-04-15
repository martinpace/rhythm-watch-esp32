# rhythm-watch-esp32

A custom application developed for the Waveshare ESP32-S3 AMOLED Touch watch development board.

## 🛠 Development Environment Setup

### A. IDE & Extensions
1. **Install Cursor or VS Code:** [Cursor.sh](https://cursor.sh/) is recommended for AI-native development.
2. **PlatformIO IDE Extension:**
   - Open Cursor/VS Code.
   - Go to the **Extensions** view (Ctrl+Shift+X).
   - Search for `platformio` and install **PlatformIO IDE**.
   - *Note: This may take several minutes to initialize the Python core and compiler toolchains.*

### B. Hardware Drivers (Crucial)
Windows often fails to recognize the ESP32-S3 over USB without the correct VCP (Virtual COM Port) drivers.
1. Download the **CP210x USB to UART Bridge VCP Drivers** from [Silicon Labs](https://www.silabs.com/developers/usb-to-uart-bridge-vcp-drivers).
2. Install the driver and restart your machine.

---

## 2. Project Configuration (`platformio.ini`)

To ensure the high-speed AMOLED and 8MB PSRAM are utilized, the project must use the following base configuration. 

```ini
[env:esp32-s3-touch-amoled]
platform = espressif32@6.6.0
board = esp32-s3-devkitc-1
framework = arduino
board_build.arduino.memory_type = qio_opi ; Critical for the AMOLED version
board_build.f_flash = 80000000L
board_build.flash_mode = qio

build_flags = 
    -DBOARD_HAS_PSRAM
    -DARDUINO_USB_MODE=1
    -DARDUINO_USB_CDC_ON_BOOT=1
    -DARDUINO_RUNNING_CORE=1
    -DARDUINO_EVENT_RUNNING_CORE=1

lib_deps =
    https://github.com/moononournation/Arduino_GFX.git#v1.4.9
    lewisxhe/XPowersLib @ ^0.2.6
    lewisxhe/SensorLib @ ^0.3.1
```

## 3. Build & Upload Process
The PlatformIO icons are located in the bottom status bar of your editor or in the PlatformIO sidebar (Alien icon).

### Step 1: Build (Compile)
* **Action:** Click the Checkmark icon (✓) in the bottom status bar.
* **What happens:** PlatformIO downloads required libraries, verifies code syntax, and generates a `firmware.bin` file in the `.pio/build` directory.

### Step 2: Device Connection
* Connect the watch to your PC via a high-quality USB-C data cable.
* **Verify Port:** Open Device Manager > Ports (COM & LPT). You should see "Silicon Labs CP210x..." or "USB Serial Device (COMx)".

### Step 3: Upload (Flash)
* **Action:** Click the Right Arrow icon (→) in the bottom status bar.
* **What happens:** The editor resets the ESP32-S3 into bootloader mode and transfers the binary.
* **Successful Output:** You should see `[SUCCESS]` in the terminal window.

## 4. Diagnostic Monitoring
Once the app is uploaded, you must open the Serial Monitor to see debug data:
### Click the **Plug** icon in the bottom status bar.
### Set the baud rate to `115200` (this is usually handled automatically by `platformio.ini`).
### If the terminal is blank, **press the Reset button** on the device to restart the application loop.

## 🔍 Troubleshooting the Windows Connection
* **Device not found:** Try a different USB port. On desktops, use the ports directly on the motherboard (rear) rather than front-panel ports for more stable power.
* **Upload fails midway:** This is often a power issue. Ensure the device is not being powered through a non-powered USB hub.
* **Header not found errors:** If you get a "fatal error: No such file or directory," click the **PlatformIO icon > Project Tasks > Clean**, then **Build** again to refresh the library index.