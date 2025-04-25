#include <M5StickCPlus.h>
#include <WiFi.h>
#include <WebServer.h>
#include <Wire.h>
#include "html_content.h"
#include "AS7341.h"

// WiFi credentials - CHANGE THESE
const char *ssid = "wifi_ssid";
const char *password = "wifi_password";

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

// Add these global variables for UI state
int displayMode = 0; // 0: spectrum, 1: wavelength selection, 2: gain settings, 3: integration time
unsigned long lastUIUpdate = 0;
const int uiUpdateInterval = 100; // Update UI every 100ms

// Define 16-bit RGB565 colors for M5StickC Plus
// Note: These are 16-bit colors, not 24-bit
#define COLOR_VIOLET 0x801F // Approximate violet in RGB565
#define COLOR_INDIGO 0x4810 // Approximate indigo in RGB565
#define COLOR_BLUE 0x001F   // Blue in RGB565
#define COLOR_CYAN 0x07FF   // Cyan in RGB565
#define COLOR_GREEN 0x07E0  // Green in RGB565
#define COLOR_YELLOW 0xFFE0 // Yellow in RGB565
#define COLOR_ORANGE 0xFD20 // Approximate orange in RGB565
#define COLOR_RED 0xF800    // Red in RGB565
#define COLOR_GREY 0x8410   // Grey in RGB565
#define COLOR_WHITE 0xFFFF  // White in RGB565
#define COLOR_BLACK 0x0000  // Black in RGB565

// Array of colors for each wavelength
uint16_t wavelengthColors[] = {
    COLOR_VIOLET, COLOR_INDIGO, COLOR_BLUE, COLOR_CYAN, COLOR_GREEN,
    COLOR_YELLOW, COLOR_ORANGE, COLOR_RED, COLOR_GREY};

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
}

// Function to display spectrum on M5StickC Plus screen
// Function to display spectrum on M5StickC Plus screen in portrait orientation
void displaySpectrum()
{
    // Clear the screen
    M5.Lcd.fillScreen(COLOR_BLACK);

    // Create space at top for headers
    int topMargin = 20;
    int bottomMargin = 10;
    int leftMargin = 10;

    // Get selected values
    uint16_t selectedBar = spectralData[wavelengthIndex];
    uint16_t selectedColor = wavelengthColors[wavelengthIndex];
    String selectedName = wavelengthNames[wavelengthIndex];

    // Find maximum value for scaling
    uint16_t maxVal = 0;
    for (int i = 0; i < 9; i++)
    {
        if (spectralData[i] > maxVal)
            maxVal = spectralData[i];
    }

    // Scale factor - now for width instead of height
    float scale = (M5.Lcd.width() - leftMargin - 10) / (float)maxVal;
    if (maxVal == 0)
        scale = 1;

    // Calculate bar height and spacing for vertical bars
    int barHeight = 8;
    int spacing = 2;
    int startY = topMargin;

    // Draw bars horizontally from left to right
    for (int i = 0; i < 9; i++)
    {
        int width = (int)(spectralData[i] * scale);
        if (width < 1)
            width = 1; // Ensure at least 1 pixel width
        uint16_t color = wavelengthColors[i];

        // Highlight selected wavelength
        if (i == wavelengthIndex)
        {
            M5.Lcd.drawRect(
                leftMargin - 1,
                startY + i * (barHeight + spacing) - 1,
                width + 2,
                barHeight + 2,
                COLOR_WHITE);
        }

        // Draw the bar
        M5.Lcd.fillRect(
            leftMargin,
            startY + i * (barHeight + spacing),
            width,
            barHeight,
            color);

        // Display wavelength labels on the left side
        M5.Lcd.setTextSize(1);
        M5.Lcd.setTextColor(color);
        M5.Lcd.setCursor(1, startY + i * (barHeight + spacing));
        M5.Lcd.print(wavelengthValues[i]);
    }

    // Display information at the bottom
    M5.Lcd.setTextSize(1);

    // Selected wavelength
    int infoY = startY + 9 * (barHeight + spacing) + 5;
    M5.Lcd.setTextColor(selectedColor);
    M5.Lcd.setCursor(5, infoY);
    M5.Lcd.print(selectedName);
    M5.Lcd.print(" ");
    M5.Lcd.print(wavelengthValues[wavelengthIndex]);
    M5.Lcd.print("nm");

    // Count value
    M5.Lcd.setTextColor(COLOR_WHITE);
    M5.Lcd.setCursor(5, infoY + 10);
    M5.Lcd.print("Count: ");
    M5.Lcd.print(selectedBar);

    // Gain and integration time
    M5.Lcd.setCursor(5, infoY + 20);
    M5.Lcd.print("G:");
    M5.Lcd.print(gainFactors[gainIndex]);
    M5.Lcd.print("x I:");
    float integrationTimeMs = (atimeSettings[atimeIndex] + 1) * 2.78;
    M5.Lcd.print(integrationTimeMs);
    M5.Lcd.print("ms");
}

// Function to display wavelength selection screen
// Function to display wavelength selection screen
void displayWavelengthSelection()
{
    M5.Lcd.fillScreen(COLOR_BLACK);

    M5.Lcd.setTextSize(1);
    M5.Lcd.setTextColor(COLOR_YELLOW);
    M5.Lcd.setCursor(5, 10);
    M5.Lcd.print("WAVELENGTH");

    M5.Lcd.setTextSize(1);
    M5.Lcd.setTextColor(COLOR_WHITE);
    M5.Lcd.setCursor(5, 30);
    M5.Lcd.print(wavelengthNames[wavelengthIndex]);

    M5.Lcd.setTextSize(1);
    M5.Lcd.setCursor(5, 45);
    M5.Lcd.print(wavelengthValues[wavelengthIndex]);
    M5.Lcd.print(" nm");

    M5.Lcd.setTextColor(COLOR_CYAN);
    M5.Lcd.setCursor(5, 70);
    M5.Lcd.print("A: Change");
    M5.Lcd.setCursor(5, 85);
    M5.Lcd.print("B: Gain");
}

// Function to display gain settings screen
void displayGainSettings()
{
    M5.Lcd.fillScreen(COLOR_BLACK);

    M5.Lcd.setTextSize(1);
    M5.Lcd.setTextColor(COLOR_YELLOW);
    M5.Lcd.setCursor(5, 10);
    M5.Lcd.print("GAIN");

    M5.Lcd.setTextSize(1);
    M5.Lcd.setTextColor(COLOR_WHITE);
    M5.Lcd.setCursor(5, 30);
    M5.Lcd.print("x");
    M5.Lcd.print(gainFactors[gainIndex]);

    M5.Lcd.setTextSize(1);
    M5.Lcd.setTextColor(COLOR_CYAN);
    M5.Lcd.setCursor(5, 70);
    M5.Lcd.print("A: Change");
    M5.Lcd.setCursor(5, 85);
    M5.Lcd.print("B: Integration");
}

// Function to display integration time settings screen
void displayIntegrationSettings()
{
    M5.Lcd.fillScreen(COLOR_BLACK);

    M5.Lcd.setTextSize(1);
    M5.Lcd.setTextColor(COLOR_YELLOW);
    M5.Lcd.setCursor(5, 10);
    M5.Lcd.print("INTEGRATION TIME");

    M5.Lcd.setTextSize(1);
    M5.Lcd.setTextColor(COLOR_WHITE);
    M5.Lcd.setCursor(5, 30);
    float integrationTimeMs = (atimeSettings[atimeIndex] + 1) * 2.78;
    M5.Lcd.print(integrationTimeMs);
    M5.Lcd.print(" ms");

    M5.Lcd.setTextSize(1);
    M5.Lcd.setTextColor(COLOR_CYAN);
    M5.Lcd.setCursor(5, 70);
    M5.Lcd.print("A: Change");
    M5.Lcd.setCursor(5, 85);
    M5.Lcd.print("B: Spectrum");
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

    // Initialize M5StickC Plus
    M5.begin();
    M5.Lcd.setRotation(1);
    M5.Lcd.fillScreen(COLOR_BLACK);
    M5.Lcd.setCursor(0, 0);
    M5.Lcd.setTextColor(COLOR_WHITE);
    M5.Lcd.setTextSize(1);
    M5.Lcd.println("Pocket Spectrometer");
    M5.Lcd.println("Initializing...");

    // Initialize random seed for mock data variation
    // randomSeed(analogRead(0));

    // Initialize I2C
    Wire.begin(32, 33); // SDA=32, SCL=33 for M5StickC Plus
    Serial.println("I2C initialized");

    // Initialize AS7341 sensor
    sensor = AS7341(Wire);
    if (!sensor.begin())
    {
        Serial.println("Failed to initialize AS7341 sensor!");
        M5.Lcd.println("Sensor init failed!");
        while (1)
            delay(100); // Halt if sensor init fails
    }

    Serial.println("AS7341 sensor initialized successfully");

    // Configure sensor with default settings (matching Python code)
    sensor.setMeasureMode(AS7341_MODE_SPM);
    sensor.setATime(29);  // 30 ASTEPS (default)
    sensor.setAStep(599); // 1.67 ms
    sensor.setGain(4);    // factor 8 (default)

    // Apply initial settings
    updateSettings();

    // Connect to WiFi
    WiFi.begin(ssid, password);
    M5.Lcd.print("Connecting to WiFi");

    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        M5.Lcd.print(".");
    }

    M5.Lcd.println("");
    M5.Lcd.println("WiFi connected!");
    M5.Lcd.print("IP: ");
    M5.Lcd.println(WiFi.localIP());

    // Set up web server routes
    server.on("/", HTTP_GET, handleRoot);
    server.on("/data", HTTP_GET, handleData);
    server.on("/measure", HTTP_GET, handleMeasure);
    server.on("/change_wavelength", HTTP_GET, handleChangeWavelength);
    server.on("/adjust_gain", HTTP_GET, handleAdjustGain);
    server.on("/adjust_integration", HTTP_GET, handleAdjustIntegrationTime);

    // Start server
    server.begin();
    M5.Lcd.println("HTTP server started");
    delay(2000);

    // Take an initial measurement
    takeMeasurement();

    // Show the spectrum display
    displaySpectrum();
}
void loop()
{
    server.handleClient();
    M5.update();

    // Handle button presses based on current display mode
    if (M5.BtnA.wasPressed())
    {
        switch (displayMode)
        {
        case 0: // Spectrum display - Take measurement
            M5.Lcd.fillScreen(COLOR_BLACK);
            M5.Lcd.setCursor(5, 40);
            M5.Lcd.setTextSize(1);
            M5.Lcd.print("Taking measurement...");

            takeMeasurement();

            displaySpectrum();
            break;

        case 1: // Wavelength selection - Change wavelength
            wavelengthIndex = (wavelengthIndex + 1) % 9;
            displayWavelengthSelection();
            break;

        case 2: // Gain settings - Change gain
            gainIndex = (gainIndex + 1) % 11;
            updateSettings();
            displayGainSettings();
            break;

        case 3: // Integration time - Change integration time
            atimeIndex = (atimeIndex + 1) % 6;
            updateSettings();
            displayIntegrationSettings();
            break;
        }
    }

    if (M5.BtnB.wasPressed())
    {
        // Cycle through display modes
        displayMode = (displayMode + 1) % 4;

        // Always update the display when changing modes, regardless of newMeasurementTaken flag
        switch (displayMode)
        {
        case 0:
            displaySpectrum();
            break;
        case 1:
            displayWavelengthSelection();
            break;
        case 2:
            displayGainSettings();
            break;
        case 3:
            displayIntegrationSettings();
            break;
        }
    }

    // Update the display periodically in spectrum mode
    if (displayMode == 0 && millis() - lastUIUpdate > uiUpdateInterval)
    {
        lastUIUpdate = millis();
        // Only redraw if new measurement was taken
        if (newMeasurementTaken)
        {
            displaySpectrum();
            newMeasurementTaken = false;
        }
    }

    // Check if we need to update the UI due to web interface changes when not in spectrum mode
    if (displayMode != 0 && newMeasurementTaken)
    {
        switch (displayMode)
        {
        case 1:
            displayWavelengthSelection();
            break;
        case 2:
            displayGainSettings();
            break;
        case 3:
            displayIntegrationSettings();
            break;
        }
        newMeasurementTaken = false; // Reset the flag after displaying
    }

    delay(10);
}
