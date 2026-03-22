# Pocket Spectrometer - M5StickC Plus 2 Version

This is a **separate port** of the Pocket Spectrometer code for the **M5StickC Plus 2** device.

## Why a Separate Version?

The M5StickC Plus 2 has different hardware that requires code modifications:

| Hardware Difference | M5StickC Plus | M5StickC Plus 2 |
|---------------------|---------------|-----------------|
| **Library** | M5StickCPlus.h | M5StickCPlus2.h |
| **Display API** | M5.Lcd | M5.Display |
| **I2C Pins** | GPIO 32 (SDA), 33 (SCL) | GPIO 32 (SDA), 33 (SCL) — same pins |
| **I2C Init** | `Wire.begin(32, 33)` | `Wire.end(); Wire.begin(32, 33, 100000)` |

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

## Known Issues (Resolved)

~~⚠️ **I2C Sensor Detection Problem**~~

**FIXED.** The sensor error on M5StickC Plus 2 was caused by I2C bus initialisation, not wrong pins.

**Root cause:** `M5.begin()` leaves the Wire bus in a state that prevents the AS7341 from being detected. The fix is to call `Wire.end()` first, then re-initialise at 100kHz:

```cpp
Wire.end();
Wire.begin(32, 33, 100000);  // release + reinit at 100kHz
sensor = AS7341(Wire);
```

The Grove PORT A pins are the same on Plus and Plus 2: **GPIO32 (SDA), GPIO33 (SCL)**. Confirmed from the official M5Stack UserDemo source (`M5StickCPlus2-UserDemo/src/test/test_io.cpp`).

## Differences from Original

**Changed:**
- Line 5: `#include <M5StickCPlus2.h>` (was `M5StickCPlus.h`)
- Line 600: `Wire.end(); Wire.begin(32, 33, 100000)` — release before re-init, explicit 100kHz
- All display calls: `M5.Display` (was `M5.Lcd`)

**Unchanged:**
- WiFi configuration
- Web server implementation
- AS7341 sensor driver
- Web interface
- All functionality

## More Information

See the main Development Guide: `../po_spectrometer_webserver/Development.md`
