# Pocket Spectrometer for M5Atom Lite

This is a version of the Pocket Spectrometer project adapted for the M5Atom Lite
board. The device creates a WiFi access point and web server, allowing you to
control the AS7341 spectral sensor and view measurements through a web
interface.

## Hardware Requirements

- M5Atom Lite
- AS7341 Spectral Sensor
- Connecting wires

## Wiring

Connect the AS7341 sensor to the M5Atom Lite:

- SDA: GPIO 26
- SCL: GPIO 32
- VCC: 3.3V
- GND: GND

## Installation Instructions

1. Install the required libraries:
   - M5Atom (by M5Stack)
   - WiFi (included with ESP32)
   - WebServer (included with ESP32)
   - Wire (included with Arduino)

2. Copy all the project files to your Arduino project directory:
   - po_spectrometer_webserver.ino
   - AS7341.h
   - AS7341.cpp
   - html_content.h

3. Upload the code to your M5Atom Lite.

## Usage

1. Power on the M5Atom Lite.
2. The device will create a WiFi access point named "PocketSpectro" with
   password "spectro123".
3. Connect to this WiFi network with your phone, tablet, or computer.
4. Open a web browser and navigate to http://192.168.4.1
5. Use the web interface to take measurements and adjust settings.
6. Press the button on the M5Atom Lite to take a measurement directly from the
   device.

## LED Indicators

The RGB LED on the M5Atom Lite provides status information:

- **Purple**: Starting up
- **Green**: Sensor initialized successfully or measurement completed
- **Cyan**: Setting up WiFi
- **Red (blinking)**: Server running, ready for connections
- **Blue**: Measurement in progress
- **Red/Off alternating**: Sensor initialization error
- **Red/Blue alternating**: WiFi AP creation error

## Web Interface

The web interface allows you to:

- View spectral data as a bar chart
- Take new measurements
- Change the selected wavelength
- Adjust gain settings
- Adjust integration time
- Download measurement data as a text file

## Default Settings

- Gain: 8x
- Integration time: 83.4ms (ATIME=29)
- Default wavelength: Blue (480nm)

## Troubleshooting

- If the LED flashes red/off, there's an issue with the sensor connection. Check
  your wiring.
- If the LED flashes red/blue, there was a problem creating the WiFi access
  point. Try resetting the device.
- If you can't connect to the WiFi, ensure you're using the correct password and
  that you're within range.
