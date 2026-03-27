#include "AS7341.h"
#include <Arduino.h>

// Base values for mock data - these represent typical readings for different wavelengths
const uint16_t BASE_VALUES[] = {120, 240, 350, 280, 190, 220, 310, 180, 90};

AS7341::AS7341(TwoWire &wire)
{
    _wire = &wire;
    _address = AS7341_I2C_ADDRESS;
    _measureMode = AS7341_MODE_SPM;
    _connected = false;
    _currentGain = 4;   // Default gain code (factor 8)
    _currentATime = 29; // Default ATIME
}

bool AS7341::begin()
{
    // In a real implementation, this would initialize the sensor
    // For mock purposes, we'll just pretend it succeeded
    _connected = true;
    return true;
}

bool AS7341::isConnected()
{
    return _connected;
}

void AS7341::setMeasureMode(uint8_t mode)
{
    _measureMode = mode;
    // In a real implementation, this would configure the sensor
}

void AS7341::setGain(uint8_t gain)
{
    if (gain <= 10)
    {
        _currentGain = gain;
        // In a real implementation, this would set the gain register
    }
}

void AS7341::setATime(uint8_t atime)
{
    _currentATime = atime;
    // In a real implementation, this would set the ATIME register
}

void AS7341::setAStep(uint16_t astep)
{
    // In a real implementation, this would set the ASTEP register
}

void AS7341::startMeasure(const char *selection)
{
    // In a real implementation, this would configure and start a measurement
    // For mock purposes, we'll just simulate a delay
    delay(50);
}

uint16_t AS7341::getChannelData(uint8_t channel)
{
    // Generate mock data for the requested channel
    return generateMockData(channel);
}

void AS7341::getSpectralData(uint16_t *data)
{
    // Generate mock data for all channels
    for (int i = 0; i < 6; i++)
    {
        data[i] = generateMockData(i);
    }
}

void AS7341::enableLED(bool enable)
{
    // In a real implementation, this would control the LED
}

void AS7341::setLEDCurrent(uint8_t current)
{
    // In a real implementation, this would set the LED current
}

// Private methods

uint16_t AS7341::generateMockData(uint8_t channel)
{
    // Generate realistic-looking mock data based on channel, gain, and integration time
    // Apply gain factor (0.5, 1, 2, 4, 8, 16, 32, 64, 128, 256, 512)
    float gainFactor = 0.5 * (1 << _currentGain);

    // Apply integration time factor
    float integrationFactor = (_currentATime + 1) / 29.0;

    // Get base value for this channel (or use a default if out of range)
    uint16_t baseValue = (channel < 9) ? BASE_VALUES[channel] : 200;

    // Add some random variation (±20%)
    float randomFactor = 0.8 + (random(40) / 100.0);

    // Calculate final value with all factors
    uint16_t value = baseValue * gainFactor * integrationFactor * randomFactor;

    return value;
}

// These methods would normally interact with the sensor via I2C
// For mock purposes, they just return success or dummy values

uint8_t AS7341::readByte(uint8_t reg)
{
    return 0;
}

uint16_t AS7341::readWord(uint8_t reg)
{
    return 0;
}

bool AS7341::writeByte(uint8_t reg, uint8_t value)
{
    return true;
}

bool AS7341::writeWord(uint8_t reg, uint16_t value)
{
    return true;
}

void AS7341::modifyReg(uint8_t reg, uint8_t mask, bool flag)
{
    // No actual implementation needed for mock
}

void AS7341::setBank(uint8_t bank)
{
    // No actual implementation needed for mock
}

void AS7341::setSpectralMeasurement(bool flag)
{
    // No actual implementation needed for mock
}

void AS7341::setSMUX(bool flag)
{
    // No actual implementation needed for mock
}

void AS7341::channelSelect(const char *selection)
{
    // No actual implementation needed for mock
}
