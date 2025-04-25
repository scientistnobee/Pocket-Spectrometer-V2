import display
from machine import I2C, Pin
from time import sleep_ms
from as7341 import *  # Import the AS7341 sensor library
from m5stack import btnA, btnB  # Import both buttons

# Initialize I2C and AS7341 sensor
i2c = I2C(sda=Pin(32), scl=Pin(33))

# Global variables for gain and integration time settings
gain_codes = [0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10]  # Gain codes (0-10)
gain_factors = [0.5, 1, 2, 4, 8, 16, 32, 64, 128, 256, 512]  # Corresponding gain factors
gain_index = 4  # Starting with gain factor 8 (index 4)
atime_settings = [9, 19, 29, 39, 49, 59]  # ATIME values for different integration times
atime_index = 2  # Starting with 29 (default value)
current_mode = 0  # 0 for normal mode, 1 for gain settings, 2 for integration time settings, 3 for wavelength selection
wavelength_index = 2  # Default to blue (index 2)
wavelength_names = ["Violet", "Vio-Blue", "Blue", "Cyan", "Green", "Yellow", "Orange", "Red", "NIR"]
wavelength_values = [415, 445, 480, 515, 555, 590, 630, 680, 940]  # Center wavelengths in nm

sensor = AS7341(i2c)
if not sensor.isconnected():
    print("Failed to contact AS7341, terminating")
    sys.exit(1)

sensor.set_measure_mode(AS7341_MODE_SPM)
sensor.set_atime(29)                # 30 ASTEPS (default)
sensor.set_astep(599)               # 1.67 ms
sensor.set_again(4)                 # factor 8 (default)

# LED control functions
def set_led_current(current_mA):
    """Set the LED current in mA (range: 0 to 100 mA)."""
    sensor.set_led_current(current_mA)

def enable_led(enable):
    """Enable or disable the onboard LED."""
    sensor.enable_led(enable)

# Create the TFT display object
tft = display.TFT()

# Define custom colors for spectral bands based on actual wavelengths
DEEP_VIOLET = 0x8000FF  # F1 (405-425nm)
VIOLET_BLUE = 0x4B0082  # F2 (435-455nm)
BLUE = 0x0000FF        # F3 (470-490nm)
CYAN = 0x00FFFF        # F4 (505-525nm)
GREEN = 0x00FF00       # F5 (545-565nm)
YELLOW = 0xFFFF00      # F6 (580-600nm)
ORANGE = 0xFF8000      # F7 (620-640nm)
DEEP_RED = 0xFF0000    # F8 (670-690nm)
GREY = 0x808080        # NIR (~940nm)

def update_settings():
    """Update sensor settings with current gain and integration time"""
    sensor.set_atime(atime_settings[atime_index])
    sensor.set_again(gain_codes[gain_index])  # Pass the gain code, not the factor

def display_gain_settings():
    """Display gain settings screen with larger font"""
    tft.clear(tft.BLACK)
    
    # Use larger font
    tft.font(tft.FONT_DejaVu18)
    
    # Display title
    tft.text(10, 20, "GAIN", tft.YELLOW)
    
    # Display current gain with large font
    tft.font(tft.FONT_DejaVu24)
    tft.text(10, 60, "x" + str(gain_factors[gain_index]), tft.WHITE)
    
    # Display simplified instructions with medium font
    tft.font(tft.FONT_DejaVu18)
    tft.text(10, 120, "A: Change", tft.CYAN)
    tft.text(10, 160, "B: Time", tft.CYAN)
    
def display_integration_settings():
    """Display integration time settings with larger font"""
    tft.clear(tft.BLACK)
    
    # Use larger font
    tft.font(tft.FONT_DejaVu18)
    
    # Display title
    tft.text(10, 20, "INT TIME", tft.YELLOW)
    
    # Display current integration time with large font
    tft.font(tft.FONT_DejaVu24)
    atime_ms = (atime_settings[atime_index] + 1) * 2.78  # Convert to milliseconds
    tft.text(10, 60, str(round(atime_ms, 1)) + "ms", tft.WHITE)
    
    # Display simplified instructions with medium font
    tft.font(tft.FONT_DejaVu18)
    tft.text(10, 120, "A: Change", tft.CYAN)
    tft.text(10, 160, "B: Exit", tft.CYAN)

def display_wavelength_selection():
    """Display wavelength selection with larger font"""
    tft.clear(tft.BLACK)
    
    # Use larger font
    tft.font(tft.FONT_DejaVu18)
    
    # Display title
    tft.text(10, 20, "WAVE LENT", tft.YELLOW)
    
    # Display selected wavelength with large font
    tft.font(tft.FONT_DejaVu24)
    tft.text(10, 60, wavelength_names[wavelength_index], tft.WHITE)
    tft.text(10, 95, str(wavelength_values[wavelength_index]) + " nm", tft.WHITE)
    
    tft.font(tft.FONT_DejaVu18)
    # Display simplified instructions
    tft.text(10, 140, "A: Change", tft.CYAN)
    tft.text(10, 180, "B: Gain", tft.CYAN)

def display_spectrum(f1, f2, f3, f4, f5, f6, f7, f8, nir):
    tft.clear(tft.BLACK)

    # Create space at top for headers (four lines of larger text)
    top_margin = 95  # Increased top margin for four text lines

    bars = [f1, f2, f3, f4, f5, f6, f7, f8, nir]  # Added NIR
    colors = [DEEP_VIOLET, VIOLET_BLUE, BLUE, CYAN, GREEN, YELLOW, ORANGE, DEEP_RED, GREY]  # Updated colors for accuracy
    wavelength_display = wavelength_values  # Use wavelength values defined globally

    # Get selected values first
    selected_bar = bars[wavelength_index]
    selected_color = colors[wavelength_index]
    selected_name = wavelength_names[wavelength_index]

    # Scale the values to fit on the screen
    max_val = max(bars)
    scale = (240 - top_margin - 20) / max_val if max_val > 0 else 1  # Adjusted for top margin and bottom labels

    # Adjust the bar width and spacing to fit the screen with 9 bars
    bar_width = 10  # Slightly smaller to fit NIR
    spacing = 4
    start_x = (135 - (9 * bar_width + 8 * spacing)) // 2

    for i, bar in enumerate(bars):
        height = int(bar * scale)
        color = colors[i]
        
        # Make the selected wavelength bar brighter
        if i == wavelength_index:
            # Draw a highlighted border for the selected wavelength
            tft.rect(start_x + i * (bar_width + spacing) - 2, 240 - height - 2, 
                     bar_width + 4, height + 4, tft.WHITE)
        
        # Draw the bar - adjust Y position for top margin
        tft.rect(start_x + i * (bar_width + spacing), 240 - height, 
                 bar_width, height, color, color)
        
        # Display wavelength value at the bottom with slightly larger font
        tft.font(tft.FONT_Default)  # Upgrade from DefaultSmall
        
        # For the visible wavelengths (0-7), show the value
        if i < 8:
            tft.text(start_x + i * (bar_width + spacing) - 2, 245, str(wavelength_values[i]), tft.WHITE)
        else:
            # For NIR, just display "NIR"
            tft.text(start_x + i * (bar_width + spacing) - 2, 245, "NIR", tft.WHITE)

    # Display the four information lines at the top with improved layout
    tft.font(tft.FONT_DejaVu24)
    
    # Line 1 - selected wavelength (centered, with color)
    name_width = len(selected_name) * 14  # Approximate width based on font
    center_x = (135 - name_width) // 2
    tft.text(center_x, 5, selected_name, selected_color)
    
    # Line 2 - count value (centered)
    count_width = len(str(selected_bar)) * 14  # Approximate width
    count_x = (135 - count_width) // 2
    tft.text(count_x, 30, str(selected_bar), tft.WHITE)
    
    # Switch to smaller font for lines 3 and 4
    tft.font(tft.FONT_DejaVu18)
    
    # Line 3 - gain information (left-aligned)
    tft.text(5, 55, "Gain: x" + str(gain_factors[gain_index]), tft.WHITE)
    
    # Line 4 - integration time (left-aligned, no units)
    atime_ms = (atime_settings[atime_index] + 1) * 2.78  # Convert to milliseconds
    tft.text(5, 75, "Time: " + str(round(atime_ms, 1)), tft.WHITE)

try:
    while True:
        if current_mode == 0:  # Normal measurement mode
            if btnA.wasPressed():  # Take measurement
                print("Taking measurement")
                set_led_current(25)
                enable_led(True)

                sensor.start_measure("F1F4CN")
                f1, f2, f3, f4, clr, nir = sensor.get_spectral_data()
                sensor.start_measure("F5F8CN")
                f5, f6, f7, f8, clr, nir = sensor.get_spectral_data()

                display_spectrum(f1, f2, f3, f4, f5, f6, f7, f8, nir)
                enable_led(False)
                
            elif btnB.wasPressed():  # Enter wavelength selection mode (changed order)
                current_mode = 3
                display_wavelength_selection()
                
        elif current_mode == 1:  # Gain settings mode
            if btnA.wasPressed():  # Cycle through gain settings
                gain_index = (gain_index + 1) % len(gain_codes)
                update_settings()
                display_gain_settings()
                
            elif btnB.wasPressed():  # Go to integration time settings mode
                current_mode = 2
                display_integration_settings()
                
        elif current_mode == 2:  # Integration time settings mode
            if btnA.wasPressed():  # Cycle through integration time settings
                atime_index = (atime_index + 1) % len(atime_settings)
                update_settings()
                display_integration_settings()
                
            elif btnB.wasPressed():  # Return to normal mode
                current_mode = 0
                tft.clear(tft.BLACK)
                
        elif current_mode == 3:  # Wavelength selection mode
            if btnA.wasPressed():  # Cycle through wavelengths
                wavelength_index = (wavelength_index + 1) % len(wavelength_names)
                display_wavelength_selection()
                
            elif btnB.wasPressed():  # Go to gain settings mode (changed order)
                current_mode = 1
                display_gain_settings()
        
        sleep_ms(100)

except KeyboardInterrupt:
    print("Interrupted from keyboard")

sensor.disable()