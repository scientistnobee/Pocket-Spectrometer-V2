// Minimal M5StickC Plus 2 test - SIMPLIFIED VERSION
#include <M5StickCPlus2.h>

void setup() {
  // Start serial FIRST before anything else
  Serial.begin(115200);
  delay(1000);
  Serial.println("\n\n=== M5StickC Plus 2 Test ===");
  Serial.println("Serial initialized");

  // Try simple M5.begin()
  Serial.println("Calling M5.begin()...");
  M5.begin();
  Serial.println("M5.begin() completed!");

  // Try to use display
  Serial.println("Initializing display...");
  M5.Display.setRotation(0);
  M5.Display.fillScreen(BLACK);
  M5.Display.setTextColor(GREEN);
  M5.Display.setTextSize(2);
  M5.Display.setCursor(10, 10);
  M5.Display.println("Hello");
  M5.Display.setCursor(10, 30);
  M5.Display.println("Plus 2!");
  Serial.println("Display initialized successfully!");

  Serial.println("=== Setup Complete ===");
}

void loop() {
  M5.update();
  delay(100);
}
