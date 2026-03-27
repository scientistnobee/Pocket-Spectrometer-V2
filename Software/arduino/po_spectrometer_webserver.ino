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
}

// Function to take a measurement
void takeMeasurement()
{
    // Generate mock data based on gain and integration time
    float gainFactor = gainFactors[gainIndex];
    float integrationFactor = (atimeSettings[atimeIndex] + 1) / 29.0;

    // Base values for different wavelengths
    uint16_t baseValues[] = {120, 240, 350, 280, 190, 220, 310, 180, 90};

    // Generate realistic-looking mock data
    for (int i = 0; i < 9; i++)
    {
        // Add some random variation (±20%)
        float randomFactor = 0.8 + (random(40) / 100.0);

        // Calculate final value with all factors
        spectralData[i] = baseValues[i] * gainFactor * integrationFactor * randomFactor;
    }

    // Set flag to indicate new measurement
    newMeasurementTaken = true;
}

// Function to display spectrum on M5StickC Plus screen
void displaySpectrum()
{
    // Clear the screen
    M5.Lcd.fillScreen(COLOR_BLACK);

    // Create space at top for headers
    int topMargin = 40;

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

    // Scale factor
    float scale = (M5.Lcd.height() - topMargin - 20) / (float)maxVal;
    if (maxVal == 0)
        scale = 1;

    // Calculate bar width and spacing
    int barWidth = 8;
    int spacing = 2;
    int startX = (M5.Lcd.width() - (9 * barWidth + 8 * spacing)) / 2;

    // Draw bars
    for (int i = 0; i < 9; i++)
    {
        int height = (int)(spectralData[i] * scale);
        if (height < 1)
            height = 1; // Ensure at least 1 pixel height
        uint16_t color = wavelengthColors[i];

        // Highlight selected wavelength
        if (i == wavelengthIndex)
        {
            M5.Lcd.drawRect(
                startX + i * (barWidth + spacing) - 1,
                M5.Lcd.height() - height - 1,
                barWidth + 2,
                height + 2,
                COLOR_WHITE);
        }

        // Draw the bar
        M5.Lcd.fillRect(
            startX + i * (barWidth + spacing),
            M5.Lcd.height() - height,
            barWidth,
            height,
            color);
    }

    // Display information at the top
    M5.Lcd.setTextSize(1);

    // Selected wavelength
    M5.Lcd.setTextColor(selectedColor);
    M5.Lcd.setCursor(5, 5);
    M5.Lcd.print(selectedName);
    M5.Lcd.print(" ");
    M5.Lcd.print(wavelengthValues[wavelengthIndex]);
    M5.Lcd.print("nm");

    // Count value
    M5.Lcd.setTextColor(COLOR_WHITE);
    M5.Lcd.setCursor(5, 15);
    M5.Lcd.print("Count: ");
    M5.Lcd.print(selectedBar);

    // Gain and integration time
    M5.Lcd.setCursor(5, 25);
    M5.Lcd.print("G:");
    M5.Lcd.print(gainFactors[gainIndex]);
    M5.Lcd.print("x I:");
    float integrationTimeMs = (atimeSettings[atimeIndex] + 1) * 2.78;
    M5.Lcd.print(integrationTimeMs);
    M5.Lcd.print("ms");
}

// Function to display wavelength selection screen
void displayWavelengthSelection()
{
    M5.Lcd.fillScreen(COLOR_BLACK);

    M5.Lcd.setTextSize(1);
    M5.Lcd.setTextColor(COLOR_YELLOW);
    M5.Lcd.setCursor(5, 10);
    M5.Lcd.print("WAVELENGTH");

    M5.Lcd.setTextSize(2);
    M5.Lcd.setTextColor(COLOR_WHITE);
    M5.Lcd.setCursor(5, 30);
    M5.Lcd.print(wavelengthNames[wavelengthIndex]);

    M5.Lcd.setTextSize(1);
    M5.Lcd.setCursor(5, 50);
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

    M5.Lcd.setTextSize(2);
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

    M5.Lcd.setTextSize(2);
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

    server.send(200, "application/json", json);
}

void handleMeasure()
{
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
    // Initialize M5StickC Plus
    M5.begin();
    M5.Lcd.setRotation(3);
    M5.Lcd.fillScreen(COLOR_BLACK);
    M5.Lcd.setCursor(0, 0);
    M5.Lcd.setTextColor(COLOR_WHITE);
    M5.Lcd.setTextSize(1);
    M5.Lcd.println("Pocket Spectrometer");
    M5.Lcd.println("Initializing...");

    // Initialize random seed for mock data variation
    randomSeed(analogRead(0));

    // Initialize I2C
    Wire.begin(32, 33); // SDA=32, SCL=33 for M5StickC Plus

    // Initialize AS7341 sensor (mock)
    sensor = AS7341(Wire);

    // Initialize spectral data array with zeros
    for (int i = 0; i < 9; i++)
    {
        spectralData[i] = 0;
    }

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
