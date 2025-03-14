# Pocket-Spectrometer-V2
<p float="left">
<img   src="https://github.com/scientistnobee/Pocket-Spectrometer-V2/blob/main/Images/IMG_7647c.jpg" alt="alt text" width="400"> 
<img  src="https://github.com/scientistnobee/Pocket-Spectrometer-V2/blob/main/Images/IMG_7632c.jpg" alt="alt text" width="400">
</p>   
I've always wanted to create a spectrometer that is so small it can fit into one's pocket and just barely larger than a cuvette itself. Working with bulky commercial spectrometers in labs, I was wondering about applications of such a small spectrometer. That's when I discovered the perfect combination – the M5StickC microcontroller and the AS7341 spectral sensor. My goal was simple: create an ultra-compact spectrometer that anyone could use. The first version of such miniature spectrometer is presented here: https://github.com/scientistnobee/Pocket-Spectrometer



This repository is the second version of the Pocket Spectrometer. The main improvement is the use of an AMS sensor from Adafruit that features a built-in grove-compatible port (known as Stemma QT) for I2C communication. While the previous version used a DFRobot AMS sensor that required manual soldering of I2C connections, this version simplifies assembly by using Adafruit's sensor with the Stemma port. The M5StickC and AMS sensor are connected using a Grove to Stemma connector, readily available from Adafruit. This cable offers improved stability with additional grip at the Grove port, making it more reliable than the original Grove cable used in the first version, which had a looser connection. This plug-and-play approach eliminates the need for soldering, reducing build steps and complexity. The 3D-printed main body has been modified to accommodate the Adafruit AMS sensor's different form factor. This version is improvement on the build side, but functionality of the spectrometer remains the same. 

## Bill of Materials (BOM)

* 1x M5StickC  (https://docs.m5stack.com/en/core/m5stickc_plus)
* 1x AS7341 spectral sensor from Adafruit (https://www.adafruit.com/product/4698)
* 1x 100mm Grove to stemma connector cable (https://www.adafruit.com/product/4528)
* 4x M2 screws  (https://eu.mouser.com/ProductDetail/Harwin/M80-2260000B?qs=DXv0QSHKF4wN5XPxPv8mDw%3D%3D&utm_source=octopart&utm_medium=aggregator&utm_campaign=855-M80-2260000B&utm_content=Harwin)
* 4x M2 heat-set inserts  (https://www.mouser.com/ProductDetail/SI/IUTFB-M2?qs=DPoM0jnrROVcA4QFWOCRzw%3D%3D&srsltid=AfmBOopUU4Yk4SA0-Q8_GQOYWdSUdiCuBDWbHyQfNmQKO6hRtp1Q_SV6)
* Black PLA filament for 3D printing  
* Black electrical tape
* White reflector (Optional)

![alt text](https://github.com/scientistnobee/Pocket-Spectrometer-V2/blob/main/Images/IMG_7582c.jpg)

## 3D-Printed Components

* Main body  
* Cap (optional)
* White reflector 

## Hardware Tools Required

* 3D printer  
* Soldering iron (for heat-set inserts only)
* M2 screwdriver

## Software Tools Required

* OpenSCAD for 3D printed parts (Optional, only required to modify the designs)  
* 3D printer slicer software (e.g., Ultimaker CURA)  
* Thonny IDE for programming the M5StickC

## Reasoning for Component Selection

### AS7341 Spectral Sensor: Advanced Spectral Sensing

The AS7341 sensor is crucial for this project:

* 11 readable individual sensor elements (10 light channels plus flicker detection)  
* Coverage from 350-1000 nm for comprehensive analysis  
* Built-in LED control for consistent illumination  
* Low power consumption for extended battery life  
* Affordable price point (~$16)
* Stemma QT connector for easy, solder-free connection

### M5StickC: Portable but Powerful Mini Computer

The M5StickC is the perfect platform for this project because it packs incredible functionality into a tiny package:

- Built-in rechargeable battery for true portability and field applications  
- USB-C connector for easy charging and programming  
- Seeed Grove port for simple sensor connections  
- Two user-programmable buttons for interface control  
- TFT display screen for displaying spectra in real-time   
- Beautifully labeled breakout header for expansion  
- Extra built-in sensors (6-axis IMU, Microphone, IR transmitter)   
- Compact size (48.2 x 25.5 x 13.7mm) ideal for portable spectrometer  
- MicroPython support for easy programming  
- Affordable price point (~$20)

## Build Instructions

### 1. Firmware Setup

#### Prepare M5StickC

1. Power on the M5StickC  
2. Quickly press the M5 button after restart (using reset/PWR button) until you see the settings screen
<img src="https://github.com/scientistnobee/Pocket-Spectrometer-V2/blob/main/Images/IMG_7081c.jpg" alt="alt text" width="500">
3. Select "Switch Mode" using the M5 button
<img src="https://github.com/scientistnobee/Pocket-Spectrometer-V2/blob/main/Images/IMG_7082c.jpg" alt="alt text" width="500">
4. Choose "USB Mode" from the options
<img src="https://github.com/scientistnobee/Pocket-Spectrometer-V2/blob/main/Images/IMG_7083c.jpg" alt="alt text" width="500">

#### Set Up Thonny IDE

1. Download and install [Thonny IDE](https://thonny.org/)  
2. Open Thonny  
3. Navigate to Tools > Options > Interpreter  
4. Configure settings:  
   - Set interpreter to "MicroPython (ESP32)"  
   - Select the correct COM port for your M5StickC  
   - Click OK and reconnect  
5. Once connected, you can write and test code directly in the IDE  
6. Best practice is to write your code in `main.py` for automatic execution on startup

### 2. Hardware Assembly

1. First test the components:
   - Connect the Grove to stemma cable between M5StickC and AMS sensor 
   - Flash the firmware using Thonny IDE
   - Verify the spectral sensor functions correctly <img src="https://github.com/scientistnobee/Pocket-Spectrometer-V2/blob/main/Images/circuit.jpg" alt="alt text" width="500">
   - Disconnect the Grove connector after testing 

2. Prepare the 3D printed parts:
   - Print the main body and cap (no support required) <img src="https://github.com/scientistnobee/Pocket-Spectrometer-V2/blob/main/Images/cura1.PNG" alt="alt text" width="500">
   - Install M2 heat-set inserts into the main body using a soldering iron <img src="https://github.com/scientistnobee/Pocket-Spectrometer-V2/blob/main/Images/IMG_6666.jpg" alt="alt text" width="500">
   
3. Final assembly:
   - Mount the AMS sensor to the body using M2 screws
   - Press-fit the M5StickC into the body from the top
   - Route and connect the Grove connector from the M5StickC to the AMS sensor
   - Apply black electrical tape to cover the AMS sensor and wires for light isolation and protection
   - Attach the cap if desired

## Applications


## Technical Implementation

The code is written in MicroPython for simplicity and accessibility, making it easy to modify for various applications. Most AI tools nowadays work well with Python code, making MicroPython an ideal choice for future modifications and improvements.

## Future Work

* Upgrade to AS7343 for expanded capabilities (14 spectral channels vs AS7341's 10 channels)  
* Optimize battery life for extended field use  
* Conduct comprehensive testing (Work in progress)
  ![alt text](https://github.com/scientistnobee/Pocket-Spectrometer-V2/blob/main/Images/IMG_7294c.png)
  ![alt text](https://github.com/scientistnobee/Pocket-Spectrometer-V2/blob/main/Images/IMG_7298c.png)
* Document detailed measurement protocols and generate standard curve plots  
* Add data logging functionality with time-stamped spectra files
* Create a calibration procedure for improved accuracy

## Troubleshooting Tips

### Software Issues

* If code doesn't execute properly, use the stop/rerun button in Thonny IDE until you see three forward-looking arrows in the console  
* Restart the device if experiencing connection issues  
* Ensure proper USB mode selection on the M5StickC before programming  
* Ensure correct serial port is selected at the Thonny IDE 


### Hardware Issues

* Verify the Grove to stemma cable is fully seated at both ends
* Ensure the M2 screws aren't overtightened on the sensor

