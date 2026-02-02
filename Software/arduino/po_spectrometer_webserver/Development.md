# Pocket Spectrometer V2 - Development Guide

## Overview

The Pocket Spectrometer V2 is a compact spectral analysis device built using the
M5StickC Plus microcontroller and the AS7341 spectral sensor. This guide covers
the development process, setup instructions, and implementation details of the
web interface that allows users to control the spectrometer and visualize
spectral data through a browser.

## Development Environment Setup

### 1. Arduino IDE Configuration

1. **Install Arduino IDE**:
   - Download and install the latest version of Arduino IDE from
     [arduino.cc](https://www.arduino.cc/en/software)
   - Launch the Arduino IDE

2. **Install ESP32 Board Support**:
   - Go to **File > Preferences**
   - Add the following URL to the "Additional Boards Manager URLs" field:
     ```
     https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
     ```
   - Go to **Tools > Board > Boards Manager**
   - Search for "ESP32" and install the "ESP32 by Espressif Systems"

3. **Install M5StickC Plus Library**:
   - Go to **Tools > Manage Libraries**
   - Search for "M5StickC" and install the "M5StickCPlus" library
   - Also install "WiFi" and "WebServer" libraries if not already included
   
   **For M5StickC Plus 2 Users**:
   - Also install the "M5StickCPlus2" library from the Library Manager
   - The Plus 2 version uses a separate codebase (see below)

4. **Configure Board Settings**:
   - Select **Tools > Board > ESP32 Arduino > M5Stick-C**
   - Set **Tools > Upload Speed** to "1500000"
   - Set **Tools > Flash Frequency** to "80MHz"
   - Set **Tools > Partition Scheme** to "Default 4MB with spiffs"

### 2. Connecting M5StickC Plus

1. **Hardware Connection**:
   - Connect the M5StickC Plus to your computer using a USB-C cable
   - Connect the AS7341 sensor to the M5StickC Plus using the Grove to Stemma QT
     connector
   - The Grove port on the M5StickC Plus is at the bottom of the device

2. **Port Selection**:
   - Go to **Tools > Port** and select the COM port that corresponds to your
     M5StickC Plus
   - On Windows, this will be a COM port (e.g., COM3)
   - On macOS, this will be a /dev/cu.usbserial device
   - On Linux, this will be a /dev/ttyUSB device

3. **Testing Connection**:
   - Load a simple example sketch (e.g., M5StickC Plus > Basics > HelloWorld)
   - Click the Upload button (right arrow icon)
   - If successful, you should see "Hello World" on the M5StickC Plus display

## M5StickC Plus 2 Support

A **separate version** of the code has been created for the M5StickC Plus 2 due to hardware differences:

### Code Location

- **M5StickC Plus**: `Software/arduino/po_spectrometer_webserver/`
- **M5StickC Plus 2**: `Software/arduino/po_spectrometer_webserver_plus2/`

### Key Differences

| Feature | M5StickC Plus | M5StickC Plus 2 |
|---------|---------------|------------------|
| **Library** | `M5StickCPlus.h` | `M5StickCPlus2.h` |
| **Display API** | `M5.Lcd` | `M5.Display` |
| **I2C Pins (SDA, SCL)** | GPIO 32, 33 | GPIO 0, 26 |
| **Code Folder** | `po_spectrometer_webserver` | `po_spectrometer_webserver_plus2` |

### Using the Plus 2 Version

1. Open `Software/arduino/po_spectrometer_webserver_plus2/po_spectrometer_webserver_plus2.ino`
2. Make sure you have the **M5StickCPlus2** library installed
3. Select **Tools > Board > M5Stick-C-Plus** or **M5Stick-C-Plus2** (if available)
4. Upload as normal

### Known Issues - M5StickC Plus 2

> [!WARNING]
> Some M5StickC Plus 2 units may have I2C connectivity issues with the AS7341 sensor.

**Symptoms:**
- Display shows "SENSOR ERROR!"
- I2C scanner finds no devices
- Code uploads successfully but sensor is not detected

**Possible Causes:**
1. **Different Grove connector pins** - The Plus 2 may use undocumented I2C pins
2. **Faulty sensor** - The AS7341 sensor may be defective
3. **Hardware incompatibility** - Some Plus 2 revisions may have incompatible Grove ports

**Troubleshooting:**
1. Use the included I2C scanner tool: `Software/arduino/i2c_scanner_plus2/`
2. Test with the working M5StickC Plus to verify the sensor is functional
3. Check physical Grove connector for damage
4. Try different I2C pin combinations if documented for your specific Plus 2 revision

## Project Implementation

### 1. Spectrometer Web Server Implementation

We developed a sketch (`po_spectrometer_webserver.ino`) that turns the M5StickC
Plus into both a spectrometer and a web server:

1. **Core Functionality**:
   - Initializes the M5StickC Plus and its display
   - Sets up WiFi connection to create a web server
   - Configures I2C communication with the AS7341 sensor
   - Implements HTTP endpoints for web interface interaction
   - Handles physical button inputs (A for measurement, B for wavelength
     selection)

2. **Sensor Integration**:
   - Created a custom AS7341 driver (`AS7341.h` and `AS7341.cpp`)
   - Implemented spectral data acquisition from 9 channels (415nm to 940nm)
   - Added controls for gain and integration time settings
   - Included LED control for sample illumination

3. **Web Server Features**:
   - Serves HTML/CSS/JavaScript content to browsers
   - Provides JSON API endpoints for data retrieval and device control
   - Handles real-time measurement requests
   - Processes parameter adjustments (wavelength, gain, integration time)

### 2. Web Interface Design

We designed a responsive web interface (`html_content.h`) that provides
intuitive control and visualization:

1. **User Interface Elements**:
   - Information panel showing current settings (selected wavelength, gain,
     integration time)
   - Interactive bar graph visualization of spectral data
   - Control buttons for measurement and parameter adjustments
   - Status indicator for operation feedback

2. **Control Features**:
   - "Take Measurement" button to trigger new readings
   - "Change Wavelength" button to cycle through available wavelengths
   - "Adjust Gain" button to cycle through gain settings (0.5x to 512x)
   - "Adjust Integration" button to cycle through integration times (30ms to
     170ms)
   - "Download Data" button to export measurements as CSV files

3. **Data Visualization**:
   - Color-coded bars representing different wavelengths
   - Numerical display of intensity values
   - Highlighted bar for the currently selected wavelength
   - Automatic scaling based on maximum intensity

4. **Data Export**:
   - Automatic filename generation based on timestamp and current settings
   - CSV format with headers and metadata
   - Client-side download functionality

## Usage Instructions

### 1. Uploading the Code

1. **Open the Project**:
   - Open `po_spectrometer_webserver.ino` in Arduino IDE
   - Ensure that `html_content.h`, `AS7341.h`, and `AS7341.cpp` are in the same
     folder

2. **WiFi Configuration (Optional)**:
   - The device is pre-configured to create its own WiFi network (Access Point mode)
   - Default settings in the code:
     ```cpp
     // Access Point Configuration
     const char *ap_ssid = "PocketSpectro";  // WiFi network name
     const char *ap_password = "spectro123"; // WiFi password (min 8 characters)
     IPAddress local_ip(192, 168, 4, 1);     // Fixed IP address
     ```
   - **You can customize these settings if desired**, then re-upload the code

3. **Upload to M5StickC Plus**:
   - Connect the M5StickC Plus to your computer
   - Select the correct board and port in Arduino IDE
   - Click the Upload button

### 2. Using the Spectrometer

1. **Starting the Device**:
   - After uploading, the M5StickC Plus will restart automatically
   - The display will show initialization steps, then display:
     - **WiFi Network (SSID):** `PocketSpectro`
     - **Password:** `spectro123`
     - **IP Address:** `192.168.4.1`
   - The device creates its own WiFi network (Access Point mode) - no router connection needed!

2. **Accessing the Web Interface**:
   - On your mobile phone, tablet, or computer:
     1. Go to WiFi settings
     2. Connect to the network named **"PocketSpectro"**
     3. Enter password: **"spectro123"**
     4. Open a web browser (Chrome, Safari, Firefox, etc.)
     5. Navigate to: **`http://192.168.4.1`**
   - The spectrometer web interface should load immediately

3. **Taking Measurements**:
   - Adjust gain, integration time, and wavelength as needed
   - Place a sample in front of the sensor
   - Press "Take Measurement" button
   - View the spectral data in the bar graph

4. **Downloading Data**:
   - After taking a measurement, press "Download Data"
   - A txt file will be generated with a unique filename based on timestamp and
     settings
   - The file will be automatically downloaded to your device

5. **Using Physical Controls**:
   - Press Button A on the M5StickC Plus to take a measurement
   - Press Button B to cycle through wavelengths

## Technical Details

### Spectral Channels

The AS7341 sensor provides readings from 9 spectral channels:

| Channel | Wavelength | Color         |
| ------- | ---------- | ------------- |
| F1      | 415nm      | Violet        |
| F2      | 445nm      | Violet-Blue   |
| F3      | 480nm      | Blue          |
| F4      | 515nm      | Cyan          |
| F5      | 555nm      | Green         |
| F6      | 590nm      | Yellow        |
| F7      | 630nm      | Orange        |
| F8      | 680nm      | Red           |
| NIR     | 940nm      | Near-Infrared |

### Sensor Settings

The spectrometer allows adjustment of two key parameters:

1. **Gain**: Controls the amplification of the sensor signal
   - Available settings: 0.5x, 1x, 2x, 4x, 8x, 16x, 32x, 64x, 128x, 256x, 512x
   - Higher gain increases sensitivity but may cause saturation with bright
     samples

2. **Integration Time**: Controls how long the sensor collects light
   - Range: Approximately 30ms to 170ms
   - Longer integration times increase sensitivity for dim samples

## Troubleshooting

### Common Issues

1. **Can't See the WiFi Network**:
   - Ensure the M5StickC Plus is powered on
   - Look for the startup screen showing "WiFi Created!"
   - The network name should be "PocketSpectro"
   - Make sure WiFi is enabled on your connecting device

2. **Can't Connect to PocketSpectro Network**:
   - Double-check the password is exactly: `spectro123` (case-sensitive)
   - Try forgetting the network on your device and reconnecting
   - Restart the M5StickC Plus by pressing the power button

3. **Web Interface Doesn't Load**:
   - Confirm you're using the correct IP address: `http://192.168.4.1`
   - Ensure your browser has JavaScript enabled
   - Try a different browser or device
   - Make sure you're connected to the "PocketSpectro" WiFi network

4. **Sensor Readings Are Inconsistent**:
   - Adjust gain and integration time for optimal readings
   - Ensure consistent lighting conditions
   - Check that the sensor is properly connected to the M5StickC Plus

4. **Upload Errors**:
   - Press and hold the M5 button (power button) for 6 seconds to reset the
     device
   - Select the correct board and port in Arduino IDE
   - Try a different USB cable or port

## Future Enhancements

1. **Data Logging**:
   - Implement time-series data collection
   - Add storage for historical measurements

2. **Calibration**:
   - Add reference spectrum calibration
   - Implement dark current subtraction

3. **Advanced Analysis**:
   - Add peak detection algorithms
   - Implement spectral matching against known samples

4. **User Experience**:
   - Add graphical configuration for WiFi settings
   - Implement user preferences storage

This project demonstrates how a compact, affordable device can provide powerful
spectral analysis capabilities through an intuitive web interface, making
spectroscopy accessible for education, hobbyists, and field applications.
