// I2C Scanner for M5StickC Plus 2
#include <M5StickCPlus2.h>
#include <Wire.h>

void scanI2C(int sda, int scl) {
  Serial.print("Scanning I2C on SDA=");
  Serial.print(sda);
  Serial.print(", SCL=");
  Serial.println(scl);

  Wire.begin(sda, scl);
  delay(100);

  int found = 0;
  for (byte address = 1; address < 127; address++) {
    Wire.beginTransmission(address);
    byte error = Wire.endTransmission();

    if (error == 0) {
      Serial.print("Device found at 0x");
      if (address < 16)
        Serial.print("0");
      Serial.println(address, HEX);
      found++;
    }
  }

  if (found == 0) {
    Serial.println("No devices found!");
  } else {
    Serial.print("Found ");
    Serial.print(found);
    Serial.println(" device(s)");
  }
  Serial.println();
}

void setup() {
  Serial.begin(115200);
  delay(2000);
  Serial.println("\n\n=== M5StickC Plus 2 I2C Scanner ===");

  M5.begin();
  M5.Display.fillScreen(BLACK);
  M5.Display.setTextColor(GREEN);
  M5.Display.setTextSize(2);
  M5.Display.setCursor(5, 10);
  M5.Display.println("I2C");
  M5.Display.println("Scanner");

  // Try different pin combinations
  Serial.println("Testing pin combinations:\n");

  scanI2C(0, 26);  // Documented Plus 2 pins
  scanI2C(32, 33); // Original Plus pins
  scanI2C(21, 22); // Common ESP32 pins
  scanI2C(25, 26); // Alternative

  Serial.println("=== Scan Complete ===");
  Serial.println("AS7341 should be at address 0x39");

  M5.Display.setTextSize(1);
  M5.Display.setCursor(5, 50);
  M5.Display.println("Check Serial");
  M5.Display.println("Monitor!");
}

void loop() { delay(1000); }
