# Pocket Spectrometer - M5StickC Plus 2 Version

This is a **separate port** of the Pocket Spectrometer code for the **M5StickC Plus 2** device.

## Why a Separate Version?

The M5StickC Plus 2 has different hardware that requires code modifications:

| Hardware Difference | M5StickC Plus | M5StickC Plus 2 |
|---------------------|---------------|-----------------|
| **Library** | M5StickCPlus.h | M5StickCPlus2.h |
| **Display API** | M5.Lcd | M5.Display |
| **I2C Pins** | GPIO 32 (SDA), 33 (SCL) | GPIO 0 (SDA), 26 (SCL) |

## Files

- `po_spectrometer_webserver_plus2.ino` - Main firmware (ported from original)
- `AS7341.cpp` / `AS7341.h` - AS7341 sensor driver (unchanged)
- `html_content.h` - Web interface HTML (unchanged)

## Installation

1. **Install libraries:**
   - Arduino IDE → Tools → Manage Libraries
   - Install: **M5StickCPlus2**

2. **Upload code:**
   - Open `po_spectrometer_webserver_plus2.ino`
   - Select board: **M5Stick-C-Plus** or **M5Stick-C-Plus2**
   - Upload

3. **Connect to device:**
   - WiFi SSID: `PocketSpectro`
   - Password: `spectro123`
   - Web interface: `http://192.168.4.1`

## Known Issues

⚠️ **I2C Sensor Detection Problem**

Some M5StickC Plus 2 units cannot detect the AS7341 sensor over I2C.

**Symptoms:**
- Display shows "SENSOR ERROR!"
- I2C scanner finds no devices

**Possible causes:**
- Undocumented I2C pin changes in some Plus 2 revisions
- Faulty AS7341 sensor
- Incompatible Grove connector

**Troubleshooting:**
1. Use I2C scanner: `../i2c_scanner_plus2/`
2. Test sensor with original M5StickC Plus
3. Check Grove cable and connector

## Differences from Original

**Changed:**
- Line 5: `#include <M5StickCPlus2.h>` (was `M5StickCPlus.h`)
- Line 600: `Wire.begin(0, 26)` (was `Wire.begin(32, 33)`)
- All display calls: `M5.Display` (was `M5.Lcd`)

**Unchanged:**
- WiFi configuration
- Web server implementation
- AS7341 sensor driver
- Web interface
- All functionality

## More Information

See the main Development Guide: `../po_spectrometer_webserver/Development.md`
