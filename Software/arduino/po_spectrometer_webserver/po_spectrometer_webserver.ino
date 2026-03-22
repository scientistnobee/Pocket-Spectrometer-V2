
#include <M5StickCPlus.h>
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
// Function to display spectrum in portrait mode
void displaySpectrum()
{
    // Clear the screen
    M5.Lcd.fillScreen(COLOR_BLACK);

    // Display key information at the top - larger text for better readability
    int infoY = 5;

    // Selected wavelength - large and prominent
    M5.Lcd.setTextSize(2);
    M5.Lcd.setTextColor(wavelengthColors[wavelengthIndex]);
    M5.Lcd.setCursor(5, infoY);
    M5.Lcd.print(wavelengthNames[wavelengthIndex]);

    // Show wavelength value on the right
    String valueStr = String(wavelengthValues[wavelengthIndex]) + "nm";
    int valueWidth = valueStr.length() * 12; // Approx width in pixels with size 2
    M5.Lcd.setCursor(M5.Lcd.width() - valueWidth - 5, infoY);
    M5.Lcd.print(valueStr);

    // Value for selected wavelength
    M5.Lcd.setTextSize(2);
    M5.Lcd.setTextColor(COLOR_WHITE);
    M5.Lcd.setCursor(5, infoY + 25);
    M5.Lcd.print("Val: ");
    M5.Lcd.print(spectralData[wavelengthIndex]);

    // Draw separator line
    M5.Lcd.drawLine(0, infoY + 50, M5.Lcd.width(), infoY + 50, COLOR_WHITE);

    // Set up chart dimensions for portrait mode
    int topMargin = infoY + 55; // Below the info
    int bottomMargin = 15;      // Space at bottom
    int leftMargin = 25;        // Space for wavelength labels
    int rightMargin = 5;

    int chartHeight = M5.Lcd.height() - topMargin - bottomMargin;
    int chartWidth = M5.Lcd.width() - leftMargin - rightMargin;

    // Find maximum value for scaling
    uint16_t maxVal = 0;
    for (int i = 0; i < 9; i++)
    {
        if (spectralData[i] > maxVal)
            maxVal = spectralData[i];
    }

    // Ensure maxVal is at least 1 to avoid division by zero
    if (maxVal == 0)
        maxVal = 1;

    // Calculate bar height and spacing for horizontal bars
    int barHeight = 10;
    int spacing = (chartHeight - (barHeight * 9)) / 10; // Evenly distribute bars
    if (spacing < 1)
        spacing = 1;

    // Display the chart as horizontal bars
    for (int i = 0; i < 9; i++)
    {
        // Calculate bar width based on value
        int barWidth = (spectralData[i] * chartWidth) / maxVal;
        if (barWidth < 1)
            barWidth = 1; // Ensure at least 1 pixel width

        int barY = topMargin + spacing + i * (barHeight + spacing);
        int barX = leftMargin;

        uint16_t color = wavelengthColors[i];

        // Display wavelength labels on the left side
        M5.Lcd.setTextSize(1);
        M5.Lcd.setTextColor(color);

        // Right-align the wavelength labels
        String label;
        if (wavelengthValues[i] == 940)
        {
            label = "940";
        }
        else
        {
            label = String(wavelengthValues[i]);
        }

        int textWidth = label.length() * 6; // Approximate width in pixels
        M5.Lcd.setCursor(leftMargin - textWidth - 3, barY + (barHeight - 8) / 2);
        M5.Lcd.print(label);

        // Highlight selected wavelength
        if (i == wavelengthIndex)
        {
            M5.Lcd.drawRect(
                barX - 1,
                barY - 1,
                barWidth + 2,
                barHeight + 2,
                COLOR_WHITE);
        }

        // Draw the bar
        M5.Lcd.fillRect(
            barX,
            barY,
            barWidth,
            barHeight,
            color);
    }

    // Small settings info at bottom
    M5.Lcd.setTextSize(1);
    M5.Lcd.setTextColor(COLOR_GREEN);
    M5.Lcd.setCursor(5, M5.Lcd.height() - 12);
    M5.Lcd.print("G:");
    M5.Lcd.print(gainFactors[gainIndex]);
    M5.Lcd.print("x");

    M5.Lcd.setTextColor(COLOR_CYAN);
    M5.Lcd.setCursor(M5.Lcd.width() / 2, M5.Lcd.height() - 12);
    float integrationTimeMs = (atimeSettings[atimeIndex] + 1) * 2.78;
    M5.Lcd.print("I:");
    M5.Lcd.print(integrationTimeMs, 1);
    M5.Lcd.print("ms");
}

// Function to display wavelength selection screen in portrait mode
void displayWavelengthSelection()
{
    M5.Lcd.fillScreen(COLOR_BLACK);

    // Title
    M5.Lcd.setTextSize(2);
    M5.Lcd.setTextColor(COLOR_YELLOW);
    M5.Lcd.setCursor(5, 5);
    M5.Lcd.print("WAVELENGTH");

    // Draw separator line
    M5.Lcd.drawLine(5, 30, M5.Lcd.width() - 5, 30, COLOR_YELLOW);

    // Current selection with larger text
    M5.Lcd.setTextSize(2);
    M5.Lcd.setTextColor(wavelengthColors[wavelengthIndex]);
    M5.Lcd.setCursor(5, 40);
    M5.Lcd.print(wavelengthNames[wavelengthIndex]);

    // Wavelength value
    M5.Lcd.setTextSize(2);
    M5.Lcd.setCursor(5, 65);
    M5.Lcd.print(wavelengthValues[wavelengthIndex]);
    M5.Lcd.print(" nm");

    // Draw all available wavelengths as color bars
    int barWidth = M5.Lcd.width() - 20; // Leave 10px margin on each side
    int barHeight = 7;
    int spacing = 3;
    int startY = 100;

    for (int i = 0; i < 9; i++)
    {
        // Highlight selected wavelength
        if (i == wavelengthIndex)
        {
            M5.Lcd.fillRect(10, startY + i * (barHeight + spacing),
                            barWidth, barHeight,
                            wavelengthColors[i]);
        }
        else
        {
            M5.Lcd.fillRect(20, startY + i * (barHeight + spacing),
                            barWidth - 20, barHeight,
                            wavelengthColors[i]);
        }
    }

    // Control buttons
    M5.Lcd.setTextSize(1);
    M5.Lcd.setTextColor(COLOR_CYAN);
    M5.Lcd.setCursor(5, M5.Lcd.height() - 30);
    M5.Lcd.print("A: Change");
    M5.Lcd.setCursor(5, M5.Lcd.height() - 15);
    M5.Lcd.print("B: Gain");
}

// Function to display gain settings screen
void displayGainSettings()
{
    M5.Lcd.fillScreen(COLOR_BLACK);

    // Title
    M5.Lcd.setTextSize(2);
    M5.Lcd.setTextColor(COLOR_YELLOW);
    M5.Lcd.setCursor(5, 5);
    M5.Lcd.print("GAIN");

    // Draw separator line
    M5.Lcd.drawLine(5, 25, M5.Lcd.width() - 5, 25, COLOR_YELLOW);

    // Current gain value - make it very large and centered
    M5.Lcd.setTextSize(3);
    M5.Lcd.setTextColor(COLOR_GREEN);

    // Format the gain value for display
    String gainText = "x" + String(gainFactors[gainIndex]);

    // Calculate position to center the text
    int textWidth = gainText.length() * 18; // Approximate width with size 3 font
    int centerX = (M5.Lcd.width() - textWidth) / 2;

    M5.Lcd.setCursor(centerX, 45);
    M5.Lcd.print(gainText);

    // Visual gain scale as a bar
    int barY = 85;
    int barHeight = 15;
    int maxBarWidth = M5.Lcd.width() - 20;

    // Calculate a logarithmic width for the bar based on the gain factor
    // since gain values range from 0.5 to 512
    float logGain = log10(gainFactors[gainIndex]);
    float maxLogGain = log10(512);
    int barWidth = (logGain / maxLogGain) * maxBarWidth;

    // Draw the bar with gradient from blue to red
    uint16_t barColor;
    if (gainFactors[gainIndex] <= 16)
    {
        barColor = COLOR_BLUE; // Low gains are blue
    }
    else if (gainFactors[gainIndex] <= 64)
    {
        barColor = COLOR_GREEN; // Medium gains are green
    }
    else if (gainFactors[gainIndex] <= 128)
    {
        barColor = COLOR_YELLOW; // Higher gains are yellow
    }
    else
    {
        barColor = COLOR_RED; // Highest gains are red
    }

    M5.Lcd.fillRect(10, barY, barWidth, barHeight, barColor);
    M5.Lcd.drawRect(10, barY, maxBarWidth, barHeight, COLOR_WHITE);

    // Show all available gain options
    M5.Lcd.setTextSize(1);
    M5.Lcd.setTextColor(COLOR_WHITE);
    M5.Lcd.setCursor(10, barY + barHeight + 5);
    M5.Lcd.print("0.5");
    M5.Lcd.setCursor(M5.Lcd.width() - 25, barY + barHeight + 5);
    M5.Lcd.print("512");

    // Control buttons
    M5.Lcd.setTextSize(1);
    M5.Lcd.setTextColor(COLOR_CYAN);
    M5.Lcd.setCursor(5, M5.Lcd.height() - 30);
    M5.Lcd.print("A: Change");
    M5.Lcd.setCursor(5, M5.Lcd.height() - 15);
    M5.Lcd.print("B: Integration");
}

// Function to display integration time settings screen
void displayIntegrationSettings()
{
    M5.Lcd.fillScreen(COLOR_BLACK);

    // Title - make it fit with word wrapping
    M5.Lcd.setTextSize(2);
    M5.Lcd.setTextColor(COLOR_YELLOW);
    M5.Lcd.setCursor(5, 5);
    M5.Lcd.print("INTEGRATION");
    M5.Lcd.setCursor(5, 25);
    M5.Lcd.print("TIME");

    // Draw separator line
    M5.Lcd.drawLine(5, 45, M5.Lcd.width() - 5, 45, COLOR_YELLOW);

    // Calculate current integration time
    float integrationTimeMs = (atimeSettings[atimeIndex] + 1) * 2.78;

    // Current integration time value - large and centered
    M5.Lcd.setTextSize(3);
    M5.Lcd.setTextColor(COLOR_CYAN);

    // Format with one decimal place
    char timeText[10];
    sprintf(timeText, "%.1f", integrationTimeMs);
    String displayText = String(timeText) + " ms";

    // Calculate position to center the text
    int textWidth = displayText.length() * 16; // Approximate width
    int centerX = max(0, (M5.Lcd.width() - textWidth) / 2);

    M5.Lcd.setCursor(centerX, 55);
    M5.Lcd.print(displayText);

    // Visual integration time slider
    int sliderY = 95;
    int sliderHeight = 10;
    int sliderWidth = M5.Lcd.width() - 20;

    // Calculate position on the slider based on current setting
    float minTime = (atimeSettings[0] + 1) * 2.78;
    float maxTime = (atimeSettings[5] + 1) * 2.78;
    int markerPosition = ((integrationTimeMs - minTime) / (maxTime - minTime)) * sliderWidth;

    // Draw the slider bar
    M5.Lcd.drawRect(10, sliderY, sliderWidth, sliderHeight, COLOR_WHITE);

    // Fill the slider up to the current position
    M5.Lcd.fillRect(10, sliderY, markerPosition, sliderHeight, COLOR_CYAN);

    // Draw the marker
    M5.Lcd.fillRect(10 + markerPosition - 2, sliderY - 5, 4, sliderHeight + 10, COLOR_WHITE);

    // Show min and max values
    M5.Lcd.setTextSize(1);
    M5.Lcd.setTextColor(COLOR_WHITE);

    char minTimeText[10], maxTimeText[10];
    sprintf(minTimeText, "%.1f", minTime);
    sprintf(maxTimeText, "%.1f", maxTime);

    M5.Lcd.setCursor(10, sliderY + sliderHeight + 5);
    M5.Lcd.print(minTimeText);

    // Right-align max value
    int maxWidth = strlen(maxTimeText) * 6;
    M5.Lcd.setCursor(M5.Lcd.width() - 10 - maxWidth, sliderY + sliderHeight + 5);
    M5.Lcd.print(maxTimeText);

    // Control buttons
    M5.Lcd.setTextSize(1);
    M5.Lcd.setTextColor(COLOR_CYAN);
    M5.Lcd.setCursor(5, M5.Lcd.height() - 30);
    M5.Lcd.print("A: Change");
    M5.Lcd.setCursor(5, M5.Lcd.height() - 15);
    M5.Lcd.print("B: Spectrum");
}

// Function to handle measurement action
void performMeasurement()
{
    // Show measurement in progress screen
    M5.Lcd.fillScreen(COLOR_BLACK);
    M5.Lcd.setTextSize(2);
    M5.Lcd.setTextColor(COLOR_WHITE);

    // Center text for portrait mode
    int centerX = (M5.Lcd.width() - 11 * 12) / 2; // 11 chars * ~12 pixels/char at size 2
    M5.Lcd.setCursor(centerX, M5.Lcd.height() / 2 - 20);
    M5.Lcd.print("MEASURING");

    // Animated dots
    for (int i = 0; i < 3; i++)
    {
        M5.Lcd.print(".");
        delay(200);
    }

    // Perform the actual measurement
    takeMeasurement();

    // Show updated display
    displaySpectrum();
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

    // Initialize M5StickC Plus
    M5.begin();
    M5.Lcd.setRotation(0); // Portrait mode
    M5.Lcd.fillScreen(COLOR_BLACK);

    // Improved startup screen with larger text
    M5.Lcd.setTextColor(COLOR_CYAN);
    M5.Lcd.setTextSize(2);
    M5.Lcd.setCursor(5, 10);
    M5.Lcd.println("Pocket");
    M5.Lcd.setCursor(5, 30);
    M5.Lcd.println("Spectrometer");

    M5.Lcd.setTextColor(COLOR_WHITE);
    M5.Lcd.setTextSize(1);
    M5.Lcd.setCursor(5, 55);
    M5.Lcd.println("Initializing...");

    // Initialize I2C
    Wire.begin(32, 33); // SDA=32, SCL=33 for M5StickC Plus
    Serial.println("I2C initialized");

    // Initialize AS7341 sensor
    sensor = AS7341(Wire);
    if (!sensor.begin())
    {
        Serial.println("Failed to initialize AS7341 sensor!");
        M5.Lcd.setTextSize(2);
        M5.Lcd.setTextColor(COLOR_RED);
        M5.Lcd.setCursor(5, 70);
        M5.Lcd.println("SENSOR");
        M5.Lcd.setCursor(5, 90);
        M5.Lcd.println("ERROR!");
        while (1)
            delay(100); // Halt if sensor init fails
    }

    M5.Lcd.setTextSize(1);
    M5.Lcd.setTextColor(COLOR_GREEN);
    M5.Lcd.setCursor(5, 70);
    M5.Lcd.println("Sensor initialized!");

    Serial.println("AS7341 sensor initialized successfully");

    // Configure sensor with default settings (matching Python code)
    sensor.setMeasureMode(AS7341_MODE_SPM);
    sensor.setATime(29);  // 30 ASTEPS (default)
    sensor.setAStep(599); // 1.67 ms
    sensor.setGain(4);    // factor 8 (default)

    // Apply initial settings
    updateSettings();

    // Set up Access Point with progress indicator
    Serial.println("Setting up WiFi Access Point...");

    M5.Lcd.setTextColor(COLOR_WHITE);
    M5.Lcd.setCursor(5, 85);
    M5.Lcd.print("Setting up AP ");

    WiFi.mode(WIFI_AP);
    WiFi.softAPConfig(local_ip, gateway, subnet);
    M5.Lcd.print(".");
    delay(300);

    bool apSuccess = WiFi.softAP(ap_ssid, ap_password);
    M5.Lcd.print(".");
    delay(300);

    if (!apSuccess)
    {
        M5.Lcd.setTextColor(COLOR_RED);
        M5.Lcd.setCursor(5, 100);
        M5.Lcd.println("AP Creation Failed!");
        delay(2000);
    }

    // Clear screen for AP info
    M5.Lcd.fillScreen(COLOR_BLACK);

    // Display AP information with improved formatting
    M5.Lcd.setTextSize(2);
    M5.Lcd.setTextColor(COLOR_YELLOW);
    M5.Lcd.setCursor(5, 5);
    M5.Lcd.println("WiFi Created!");

    M5.Lcd.setTextSize(1);
    M5.Lcd.setTextColor(COLOR_WHITE);
    M5.Lcd.setCursor(5, 35);
    M5.Lcd.print("SSID: ");

    M5.Lcd.setTextColor(COLOR_GREEN);
    M5.Lcd.setCursor(45, 35);
    M5.Lcd.println(ap_ssid);

    M5.Lcd.setTextColor(COLOR_WHITE);
    M5.Lcd.setCursor(5, 50);
    M5.Lcd.print("Pass: ");

    M5.Lcd.setTextColor(COLOR_GREEN);
    M5.Lcd.setCursor(45, 50);
    M5.Lcd.println(ap_password);

    M5.Lcd.setTextColor(COLOR_WHITE);
    M5.Lcd.setCursor(5, 65);
    M5.Lcd.print("IP: ");

    M5.Lcd.setTextSize(2);
    M5.Lcd.setTextColor(COLOR_CYAN);
    M5.Lcd.setCursor(5, 80);
    M5.Lcd.println(WiFi.softAPIP().toString());

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

    M5.Lcd.setTextSize(1);
    M5.Lcd.setTextColor(COLOR_GREEN);
    M5.Lcd.setCursor(5, 110);
    M5.Lcd.println("HTTP server started");

    // Prompt user to connect
    M5.Lcd.setTextColor(COLOR_WHITE);
    M5.Lcd.setCursor(5, 125);
    M5.Lcd.println("Connect to WiFi & open IP");
    M5.Lcd.println("in your browser");

    delay(4000); // Give user time to read the instructions

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
            performMeasurement();
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

        // Show a brief transition message
        M5.Lcd.fillScreen(COLOR_BLACK);
        M5.Lcd.setTextSize(2);
        M5.Lcd.setTextColor(COLOR_CYAN);

        String modeText;
        switch (displayMode)
        {
        case 0:
            modeText = "SPECTRUM";
            break;
        case 1:
            modeText = "WAVELENGTH";
            break;
        case 2:
            modeText = "GAIN";
            break;
        case 3:
            modeText = "INTEGRATION";
            break;
        }

        // Calculate center position for text
        int textWidth = modeText.length() * 12; // Approximate width for size 2 font
        int centerX = (M5.Lcd.width() - textWidth) / 2;
        int centerY = (M5.Lcd.height() - 16) / 2;

        M5.Lcd.setCursor(centerX, centerY);
        M5.Lcd.print(modeText);
        delay(300); // Brief pause to show the mode change

        // Update the display to the new mode
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