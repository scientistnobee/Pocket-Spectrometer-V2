#include <M5Atom.h>
#include <WiFi.h>
#include <WebServer.h>
#include <Wire.h>
#include "html_content.h"
#include "AS7341.h"

// Access Point Configuration
const char *ap_ssid = "PocketSpectro";  // Name of the WiFi network the device will create
const char *ap_password = "spectro123"; // Password for the WiFi network (min 8 characters)
IPAddress local_ip(192, 168, 4, 1);     // Fixed IP address for the device
IPAddress gateway(192, 168, 4, 1);      // Gateway (same as IP)
IPAddress subnet(255, 255, 255, 0);     // Subnet mask

// Create web server object
WebServer server(80);

// Create AS7341 sensor object
AS7341 sensor;

// Global variables for gain and integration time settings
uint8_t gainCodes[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
float gainFactors[] = {0.5, 1, 2, 4, 8, 16, 32, 64, 128, 256, 512};
int gainIndex = 4; // Starting with gain factor 8 (index 4)
uint8_t atimeSettings[] = {9, 19, 29, 39, 49, 59};
int atimeIndex = 2;      // Starting with 29 (default value)
int wavelengthIndex = 2; // Default to blue(index 2)
String wavelengthNames[] = {"Violet", "Vio-Blue", "Blue", "Cyan", "Green", "Yellow", "Orange", "Red", "NIR"};
int wavelengthValues[] = {415, 445, 480, 515, 555, 590, 630, 680, 940}; // Center wavelengths in nm

// Spectral data storage
uint16_t spectralData[9]; // F1-F8 + NIR

// Flag to indicate if a new measurement was taken
bool newMeasurementTaken = false;

// LED status indicators
bool isServerRunning = false;
bool isMeasurementInProgress = false;
unsigned long lastLedUpdate = 0;
const int ledBlinkInterval = 500; // Blink interval in ms
bool ledState = false;

// Button debounce variables
unsigned long lastButtonPress = 0;
const int debounceTime = 300; // Debounce time in ms

// Function to update sensor settings
void updateSettings()
{
    sensor.setATime(atimeSettings[atimeIndex]);
    sensor.setGain(gainCodes[gainIndex]);

    // Debug output
    Serial.print("Updated settings - Gain: x");
    Serial.print(gainFactors[gainIndex]);
    Serial.print(", Integration time: ");
    float integrationTimeMs = (atimeSettings[atimeIndex] + 1) * 2.78;
    Serial.print(integrationTimeMs);
    Serial.println("ms");
}

// Function to take a measurement
void takeMeasurement()
{
    // Set measurement in progress flag
    isMeasurementInProgress = true;

    // Show measurement in progress with LED
    M5.dis.drawpix(0, 0x0000FF); // Blue color during measurement

    // Update sensor settings first
    updateSettings();

    // Turn on LED for measurement
    sensor.enableLED(true);
    sensor.setLEDCurrent(25); // 25mA current

    Serial.println("Starting first measurement (F1F4CN)...");

    // First measurement: F1-F4 + Clear + NIR
    sensor.startMeasure("F1F4CN");

    // Read the spectral data for first 4 channels
    uint16_t channelData1[6];
    sensor.getSpectralData(channelData1);

    Serial.println("First measurement data:");
    for (int i = 0; i < 6; i++)
    {
        Serial.print(i);
        Serial.print(": ");
        Serial.println(channelData1[i]);
    }

    Serial.println("Starting second measurement (F5F8CN)...");

    // Second measurement: F5-F8 + Clear + NIR
    sensor.startMeasure("F5F8CN");

    // Read the spectral data for next 4 channels
    uint16_t channelData2[6];
    sensor.getSpectralData(channelData2);

    Serial.println("Second measurement data:");
    for (int i = 0; i < 6; i++)
    {
        Serial.print(i);
        Serial.print(": ");
        Serial.println(channelData2[i]);
    }

    // Turn off LED after measurement
    sensor.enableLED(false);

    // Map to our spectral data array (F1-F8 + NIR)
    for (int i = 0; i < 4; i++)
    {
        spectralData[i] = channelData1[i];     // F1-F4
        spectralData[i + 4] = channelData2[i]; // F5-F8
    }

    // Use NIR from second measurement (or average both if preferred)
    spectralData[8] = channelData2[5]; // NIR

    // Debug output
    Serial.println("Final Spectral Data:");
    for (int i = 0; i < 9; i++)
    {
        Serial.print(wavelengthValues[i]);
        Serial.print("nm: ");
        Serial.println(spectralData[i]);
    }

    // Set flag to indicate new measurement
    newMeasurementTaken = true;

    // Measurement complete
    isMeasurementInProgress = false;

    // Show success with green LED for a moment
    M5.dis.drawpix(0, 0x00FF00); // Green color for success
    delay(500);

    // Return to server running indication
    if (isServerRunning)
    {
        M5.dis.drawpix(0, 0xFF0000); // Red color for server running
    }
}

// Function to handle DNS requests for captive portal
void handleDNS()
{
    // Redirect all domains to our web server
    server.sendHeader("Location", String("http://") + WiFi.softAPIP().toString(), true);
    server.send(302, "text/plain", "");
}

// HTTP request handlers
void handleRoot()
{
    server.send(200, "text/html", index_html);
}

void handleData()
{
    String json = "{";
    json += "\"wavelengths\": [415, 445, 480, 515, 555, 590, 630, 680, 940],";
    json += "\"names\": [\"Violet\", \"Vio-Blue\", \"Blue\", \"Cyan\", \"Green\", \"Yellow\", \"Orange\", \"Red\", \"NIR\"],";

    // Add values array
    json += "\"values\": [";
    for (int i = 0; i < 9; i++)
    {
        json += String(spectralData[i]);
        if (i < 8)
            json += ",";
    }
    json += "],";

    // Add other parameters
    json += "\"gain\": " + String(gainFactors[gainIndex]) + ",";
    float integrationTimeMs = (atimeSettings[atimeIndex] + 1) * 2.78;
    json += "\"integration_time\": " + String(integrationTimeMs) + ",";
    json += "\"selected_index\": " + String(wavelengthIndex);
    json += "}";

    Serial.println("Sending JSON data to client:");
    Serial.println(json);

    server.send(200, "application/json", json);
}

void handleMeasure()
{
    Serial.println("Web client requested new measurement");

    // Take a new measurement
    takeMeasurement();

    // Return the same data format as handleData
    handleData();
}

void handleChangeWavelength()
{
    // Cycle to the next wavelength
    wavelengthIndex = (wavelengthIndex + 1) % 9;

    // Force UI update on the device
    newMeasurementTaken = true;
    // Return the new selected index
    String json = "{\"selected_index\": " + String(wavelengthIndex) + "}";
    server.send(200, "application/json", json);
}

void handleAdjustGain()
{
    // Cycle to the next gain setting
    gainIndex = (gainIndex + 1) % 11;
    updateSettings();

    // Force UI update on the device
    newMeasurementTaken = true;

    // Return the new gain value
    String json = "{\"gain\": " + String(gainFactors[gainIndex]) + "}";
    server.send(200, "application/json", json);
}

void handleAdjustIntegrationTime()
{
    // Cycle to the next integration time setting
    atimeIndex = (atimeIndex + 1) % 6;
    updateSettings();

    // Force UI update on the device
    newMeasurementTaken = true;

    // Calculate integration time in milliseconds
    float integrationTimeMs = (atimeSettings[atimeIndex] + 1) * 2.78;

    // Return the new integration time
    String json = "{\"integration_time\": " + String(integrationTimeMs) + "}";
    server.send(200, "application/json", json);
}

void setup()
{
    // Initialize serial communication
    Serial.begin(115200);
    delay(100); // Wait for serial monitor to open
    Serial.println("Pocket Spectrometer starting...");

    // Initialize M5Atom
    M5.begin(true, false, true); // Enable serial, disable I2C (we'll initialize it manually), enable display

    // Show startup with purple LED
    M5.dis.drawpix(0, 0x800080); // Purple color for startup

    // Initialize I2C
    Wire.begin(26, 32); // SDA=26, SCL=32 for M5Atom Lite
    Serial.println("I2C initialized");

    // Initialize AS7341 sensor
    sensor = AS7341(Wire);
    if (!sensor.begin())
    {
        Serial.println("Failed to initialize AS7341 sensor!");
        // Show error with red blinking LED
        while (1)
        {
            M5.dis.drawpix(0, 0xFF0000); // Red
            delay(300);
            M5.dis.drawpix(0, 0x000000); // Off
            delay(300);
        }
    }

    Serial.println("AS7341 sensor initialized successfully");

    // Show sensor success with green LED briefly
    M5.dis.drawpix(0, 0x00FF00); // Green
    delay(500);

    // Configure sensor with default settings (matching Python code)
    sensor.setMeasureMode(AS7341_MODE_SPM);
    sensor.setATime(29);  // 30 ASTEPS (default)
    sensor.setAStep(599); // 1.67 ms
    sensor.setGain(4);    // factor 8 (default)

    // Apply initial settings
    updateSettings();

    // Set up Access Point with progress indicator
    Serial.println("Setting up WiFi Access Point...");

    // Show WiFi setup with cyan LED
    M5.dis.drawpix(0, 0x00FFFF); // Cyan

    WiFi.mode(WIFI_AP);
    WiFi.softAPConfig(local_ip, gateway, subnet);

    bool apSuccess = WiFi.softAP(ap_ssid, ap_password);

    if (!apSuccess)
    {
        Serial.println("AP Creation Failed!");
        // Show error with red/blue alternating LED
        while (1)
        {
            M5.dis.drawpix(0, 0xFF0000); // Red
            delay(300);
            M5.dis.drawpix(0, 0x0000FF); // Blue
            delay(300);
        }
    }

    // Display AP information in serial
    Serial.println("WiFi Access Point Created!");
    Serial.print("SSID: ");
    Serial.println(ap_ssid);
    Serial.print("Password: ");
    Serial.println(ap_password);
    Serial.print("IP Address: ");
    Serial.println(WiFi.softAPIP().toString());

    // Set up web server routes
    server.on("/", HTTP_GET, handleRoot);
    server.on("/data", HTTP_GET, handleData);
    server.on("/measure", HTTP_GET, handleMeasure);
    server.on("/change_wavelength", HTTP_GET, handleChangeWavelength);
    server.on("/adjust_gain", HTTP_GET, handleAdjustGain);
    server.on("/adjust_integration", HTTP_GET, handleAdjustIntegrationTime);

    // Handle all other domains to create a captive portal effect
    server.onNotFound(handleDNS);

    // Start server
    server.begin();
    Serial.println("HTTP server started");

    // Set server running flag
    isServerRunning = true;

    // Show server running with red LED
    M5.dis.drawpix(0, 0xFF0000); // Red for server running

    // Take an initial measurement
    takeMeasurement();
}

void loop()
{
    server.handleClient();
    M5.update();

    // Handle button press for measurement
    if (M5.Btn.wasPressed() && (millis() - lastButtonPress > debounceTime))
    {
        lastButtonPress = millis();

        // Only take measurement if not already in progress
        if (!isMeasurementInProgress)
        {
            takeMeasurement();
        }
    }

    // Blink LED when server is running (only if not measuring)
    if (isServerRunning && !isMeasurementInProgress)
    {
        if (millis() - lastLedUpdate > ledBlinkInterval)
        {
            lastLedUpdate = millis();

            if (ledState)
            {
                M5.dis.drawpix(0, 0xFF0000); // Red
            }
            else
            {
                M5.dis.drawpix(0, 0x220000); // Dim red
            }

            ledState = !ledState;
        }
    }

    delay(10);
}
