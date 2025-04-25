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
* Black PLA filament for 3D printing of the enclosure and the cap
* White PLA for 3D printing of the reflector    
* * Black electrical tape

![alt text](https://github.com/scientistnobee/Pocket-Spectrometer-V2/blob/main/Images/IMG_7582c.jpg)

## 3D-Printed Components

* Main body  
* Cap 
* White reflector 

## Hardware Tools Required

* 3D printer  
* Soldering iron (for heat-set inserts only)
* M2 screwdriver

## Software Tools Required

* OpenSCAD for 3D printed parts (Optional, only required if you wany to modify the designs)  
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

#### Set Up Thonny IDE and upload the firmware

1. Download and install [Thonny IDE](https://thonny.org/)  
2. Connect the M5stickC to computer and Open Thonny
4. Navigate to Tools > Options > Interpreter  
5. Configure settings:  
   - Set interpreter to "MicroPython (ESP32)"  
   - Select the correct COM port for your M5StickC  
   - Click OK and reconnect  
6. Once connected, you can write and test code directly in the IDE
7. Download the software from this repository and save into a folder (For example to Downloads folder).
8. Unzip the folder. Navigate to this folder's sub folder named Software inside Thorny. The files of the Software should should up at the top-left side as shown in the below figure.  
<img src="https://github.com/scientistnobee/Pocket-Spectrometer-V2/blob/main/Images/micropython7c.png" alt="alt text" width="500"> 
9. If M5StickC is connected and set-up correctly within Thorny then the existing files of M5stick's flash memoery can be seen at the lower left side of the Thorny.
<img src="https://github.com/scientistnobee/Pocket-Spectrometer-V2/blob/main/Images/micropython8.png" alt="alt text" width="500">
11. Now copy (drag) the files from the computer (top left) to M5stickC (bottom-left).
<img src="https://github.com/scientistnobee/Pocket-Spectrometer-V2/blob/main/Images/micropython9.png" alt="alt text" width="500">



### 2. Hardware Assembly

1. First test the components:
   - Connect the Grove to stemma cable between M5StickC and AMS sensor  <img src="https://github.com/scientistnobee/Pocket-Spectrometer-V2/blob/main/Images/circuit.jpg" alt="alt text" width="500">
   - Upload the firmware using Thonny IDE as described above
   - Verify the spectral sensor functions correctly. If M5 button is pressed, white light of the AMS sensor should be on and a spectrum should be displayed on the screen as shown in the picture. 
   - Disconnect the Grove connector cable from the M5StickC after the above testing  <img src="https://github.com/scientistnobee/Pocket-Spectrometer-V2/blob/main/Images/IMG_7592c.jpg" alt="alt text" width="500">

2. Prepare the 3D printed parts:
   - Print the main body and cap using Black PLA (no support required). Look here for the print orinetation <img src="https://github.com/scientistnobee/Pocket-Spectrometer-V2/blob/main/Images/cura1.PNG" alt="alt text" width="500">
   - Print the reflector using a white  PLA (no support required). Look here for the print orinetation 
   - Install M2 heat-set inserts into the main body using a soldering iron <img src="https://github.com/scientistnobee/Pocket-Spectrometer-V2/blob/main/Images/IMG_6666.jpg" alt="alt text" width="500">
   
3. Final assembly:
   - Mount the AMS sensor to the body using M2 screws
     <img src="https://github.com/scientistnobee/Pocket-Spectrometer-V2/blob/main/Images/IMG_7597c.jpg" alt="alt text" width="500">
   - Route the Grove to stemma cable from the AMS sensor through the hole at the bottom into the slot for M5stickC as shown in the image here.
     <img src="https://github.com/scientistnobee/Pocket-Spectrometer-V2/blob/main/Images/IMG_7598c.jpg" alt="alt text" width="500">
   - Connect the Grove to stemma cable to the M5StickC
     <img src="https://github.com/scientistnobee/Pocket-Spectrometer-V2/blob/main/Images/IMG_7602c.jpg" alt="alt text" width="500">
   - See this image for more clarity.
     
      <img src="https://github.com/scientistnobee/Pocket-Spectrometer-V2/blob/main/Images/IMG_7600c.jpg" alt="alt text" width="500">
   - Press-fit the M5StickC into the body from the top
   - Apply black electrical tape to cover the AMS sensor and wires for light isolation and protection
   - Press fit the white reflector at the narrow slot.

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
* Ensure correct serial port is selected at the Thonny IDE M

### Hardware Issues

* Verify the Grove to stemma cable is fully seated at both ends
* Ensure the M2 screws aren't overtightened on the sensor

