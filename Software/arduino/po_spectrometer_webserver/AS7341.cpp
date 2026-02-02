#include "AS7341.h"
#include <Arduino.h>

// SMUX configurations from Python implementation
const uint8_t SMUX_F1F4CN[20] = {0x30, 0x01, 0x00, 0x00, 0x00, 0x42, 0x00, 0x00, 0x50, 0x00, 0x00, 0x00, 0x20, 0x04, 0x00, 0x30, 0x01, 0x50, 0x00, 0x06};
const uint8_t SMUX_F5F8CN[20] = {0x00, 0x00, 0x00, 0x40, 0x02, 0x00, 0x10, 0x03, 0x50, 0x10, 0x03, 0x00, 0x00, 0x00, 0x24, 0x00, 0x00, 0x50, 0x00, 0x06};
const uint8_t SMUX_FD[20] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x2A};

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
    // Cycle power and check if AS7341 is connected
    disable();
    delay(50);

    // Power on
    if (!writeByte(AS7341_ENABLE, AS7341_ENABLE_PON))
    {
        return false;
    }
    delay(50);

    // Check device ID
    uint8_t id = readByte(AS7341_ID);
    if ((id & 0xFC) != AS7341_ID_VALUE)
    {
        return false;
    }

    // Configure with default measurement mode
    setMeasureMode(_measureMode);
    _connected = true;
    return true;
}

bool AS7341::isConnected()
{
    return _connected;
}

void AS7341::disable()
{
    // Disable all functions and power off
    setBank(1);
    writeByte(AS7341_CONFIG, 0x00);
    setBank(0);
    writeByte(AS7341_ENABLE, 0x00);
}

void AS7341::setMeasureMode(uint8_t mode)
{
    if (mode == AS7341_MODE_SPM || mode == AS7341_MODE_SYNS || mode == AS7341_MODE_SYND)
    {
        _measureMode = mode;
        setBank(1);
        uint8_t data = readByte(AS7341_CONFIG) & (~0x03);
        data |= mode;
        writeByte(AS7341_CONFIG, data);
        setBank(0);
    }
}

void AS7341::setGain(uint8_t gain)
{
    if (gain <= 10)
    {
        _currentGain = gain;
        writeByte(AS7341_CFG_1, gain);
    }
}

uint8_t AS7341::getAgain()
{
    return readByte(AS7341_CFG_1);
}

float AS7341::getAgainFactor()
{
    uint8_t code = readByte(AS7341_CFG_1);
    return pow(2, code - 1);
}

void AS7341::setAgainFactor(float factor)
{
    uint8_t code = 0;
    for (code = 10; code >= 0; code--)
    {
        if (pow(2, code - 1) <= factor)
        {
            break;
        }
    }
    writeByte(AS7341_CFG_1, code);
}

void AS7341::setATime(uint8_t atime)
{
    _currentATime = atime;
    writeByte(AS7341_ATIME, atime);
}

void AS7341::setAStep(uint16_t astep)
{
    writeWord(AS7341_ASTEP, astep);
}

float AS7341::getIntegrationTime()
{
    uint16_t astep = readWord(AS7341_ASTEP);
    uint8_t atime = readByte(AS7341_ATIME);
    return ((astep + 1) * (atime + 1) * 2.78 / 1000.0);
}

bool AS7341::measurementCompleted()
{
    return (readByte(AS7341_STATUS_2) & AS7341_STATUS_2_AVALID) != 0;
}

void AS7341::setSpectralMeasurement(bool flag)
{
    modifyReg(AS7341_ENABLE, AS7341_ENABLE_SP_EN, flag);
}

void AS7341::setSmux(bool flag)
{
    modifyReg(AS7341_ENABLE, AS7341_ENABLE_SMUXEN, flag);
}

void AS7341::channelSelect(const char *selection)
{
    const uint8_t *smuxConfig = NULL;

    if (strcmp(selection, "F1F4CN") == 0)
    {
        smuxConfig = SMUX_F1F4CN;
    }
    else if (strcmp(selection, "F5F8CN") == 0)
    {
        smuxConfig = SMUX_F5F8CN;
    }
    else if (strcmp(selection, "FD") == 0)
    {
        smuxConfig = SMUX_FD;
    }
    else
    {
        return; // Unknown selection
    }

    writeBurst(0x00, smuxConfig, 20);
}

void AS7341::startMeasure(const char *selection)
{
    modifyReg(AS7341_CFG_0, AS7341_CFG_0_LOW_POWER, false);
    setSpectralMeasurement(false);
    writeByte(AS7341_CFG_6, AS7341_CFG_6_SMUX_CMD_WRITE);

    if (_measureMode == AS7341_MODE_SPM)
    {
        channelSelect(selection);
        setSmux(true);
    }
    else if (_measureMode == AS7341_MODE_SYNS)
    {
        channelSelect(selection);
        setSmux(true);
        setGpioInput(true);
    }

    setSpectralMeasurement(true);

    if (_measureMode == AS7341_MODE_SPM)
    {
        // Wait for measurement to complete
        unsigned long startTime = millis();
        while (!measurementCompleted() && (millis() - startTime < 1000))
        {
            delay(10);
        }
    }
}

uint16_t AS7341::getChannelData(uint8_t channel)
{
    if (channel <= 5)
    {
        return readWord(AS7341_CH_DATA + channel * 2);
    }
    return 0;
}

void AS7341::getSpectralData(uint16_t *data)
{
    // Add debug output
    Serial.println("Reading spectral data from sensor...");

    readAllChannels(data);

    // Debug the read values
    Serial.println("Raw channel data:");
    for (int i = 0; i < 6; i++)
    {
        Serial.print(i);
        Serial.print(": ");
        Serial.println(data[i]);
    }
}

void AS7341::enableLED(bool enable)
{
    setBank(1);
    modifyReg(AS7341_CONFIG, AS7341_CONFIG_LED_SEL, enable);

    if (enable)
    {
        modifyReg(AS7341_LED, AS7341_LED_LED_ACT, true);
    }
    else
    {
        modifyReg(AS7341_LED, AS7341_LED_LED_ACT, false);
    }

    setBank(0);
}

void AS7341::setLEDCurrent(uint8_t current)
{
    // Clamp current to valid range (4-258 mA)
    if (current > 258)
        current = 258;
    if (current < 4)
        current = 4;

    setBank(1);
    modifyReg(AS7341_CONFIG, AS7341_CONFIG_LED_SEL, true);

    // Set LED current (current - 4) / 2
    uint8_t ledValue = AS7341_LED_LED_ACT | ((current - 4) / 2);
    writeByte(AS7341_LED, ledValue);

    setBank(0);
    delay(100);
}

void AS7341::setFlickerDetection(bool flag)
{
    modifyReg(AS7341_ENABLE, AS7341_ENABLE_FDEN, flag);
}

uint8_t AS7341::getFlickerFrequency()
{
    modifyReg(AS7341_CFG_0, AS7341_CFG_0_LOW_POWER, false);
    setSpectralMeasurement(false);
    writeByte(AS7341_CFG_6, AS7341_CFG_6_SMUX_CMD_WRITE);
    channelSelect("FD");
    setSmux(true);
    setSpectralMeasurement(true);
    setFlickerDetection(true);

    // Wait for measurement to complete
    unsigned long startTime = millis();
    bool measurementValid = false;

    while ((millis() - startTime < 1000) && !measurementValid)
    {
        uint8_t fdStatus = readByte(AS7341_FD_STATUS);
        if (fdStatus & AS7341_FD_STATUS_FD_MEAS_VALID)
        {
            measurementValid = true;
        }
        delay(10);
    }

    if (!measurementValid)
    {
        setFlickerDetection(false);
        return 0;
    }

    // Wait for calculation to complete
    startTime = millis();
    bool calculationValid = false;
    uint8_t fdStatus = 0;

    while ((millis() - startTime < 1000) && !calculationValid)
    {
        fdStatus = readByte(AS7341_FD_STATUS);
        if ((fdStatus & AS7341_FD_STATUS_FD_100_VALID) ||
            (fdStatus & AS7341_FD_STATUS_FD_120_VALID))
        {
            calculationValid = true;
        }
        delay(10);
    }

    setFlickerDetection(false);
    writeByte(AS7341_FD_STATUS, 0x3C); // Clear all FD STATUS bits

    if (!calculationValid)
    {
        return 0;
    }

    if ((fdStatus & AS7341_FD_STATUS_FD_100_VALID) &&
        (fdStatus & AS7341_FD_STATUS_FD_100HZ))
    {
        return 100;
    }
    else if ((fdStatus & AS7341_FD_STATUS_FD_120_VALID) &&
             (fdStatus & AS7341_FD_STATUS_FD_120HZ))
    {
        return 120;
    }

    return 0;
}

void AS7341::setGpioInput(bool enable)
{
    uint8_t mask = AS7341_GPIO_2_GPIO_OUT;
    if (enable)
    {
        mask |= AS7341_GPIO_2_GPIO_IN_EN;
    }
    writeByte(AS7341_GPIO_2, mask);
}

bool AS7341::getGpioValue()
{
    return (readByte(AS7341_GPIO_2) & AS7341_GPIO_2_GPIO_IN) != 0;
}

void AS7341::setGpioOutput(bool inverted)
{
    uint8_t mask = 0x00;
    if (inverted)
    {
        mask |= AS7341_GPIO_2_GPIO_INV;
    }
    writeByte(AS7341_GPIO_2, mask);
}

void AS7341::setGpioInverted(bool flag)
{
    modifyReg(AS7341_GPIO_2, AS7341_GPIO_2_GPIO_INV, flag);
}

void AS7341::setGpioMask(uint8_t mask)
{
    writeByte(AS7341_GPIO_2, mask);
}

void AS7341::setWen(bool flag)
{
    modifyReg(AS7341_ENABLE, AS7341_ENABLE_WEN, flag);
}

void AS7341::setWtime(uint8_t code)
{
    writeByte(AS7341_WTIME, code);
}

void AS7341::checkInterrupt()
{
    uint8_t status = readByte(AS7341_STATUS);
    if (status & AS7341_STATUS_ASAT)
    {
        Serial.println("Spectral interrupt generation!");
    }
}

void AS7341::clearInterrupt()
{
    writeByte(AS7341_STATUS, 0xFF);
}

void AS7341::setSpectralInterrupt(bool flag)
{
    modifyReg(AS7341_INTENAB, AS7341_INTENAB_SP_IEN, flag);
}

void AS7341::setInterruptPersistence(uint8_t value)
{
    if (value <= 15)
    {
        writeByte(AS7341_PERS, value);
    }
}

void AS7341::setSpectralThresholdChannel(uint8_t value)
{
    if (value <= 4)
    {
        writeByte(AS7341_CFG_12, value);
    }
}

void AS7341::setThresholds(uint16_t lo, uint16_t hi)
{
    if (lo < hi)
    {
        writeWord(AS7341_SP_TH_LOW, lo);
        writeWord(AS7341_SP_TH_HIGH, hi);
        delay(20);
    }
}

void AS7341::getSynsInt()
{
    setBank(1);
    writeByte(AS7341_CONFIG, AS7341_CONFIG_INT_SEL | AS7341_CONFIG_INT_MODE_SYNS);
    setBank(0);
}

// Private methods for I2C communication

uint8_t AS7341::readByte(uint8_t reg)
{
    _wire->beginTransmission(_address);
    _wire->write(reg);
    if (_wire->endTransmission() != 0)
    {
        return 0; // Error
    }

    if (_wire->requestFrom(_address, (uint8_t)1) != 1)
    {
        return 0; // Error
    }

    return _wire->read();
}

uint16_t AS7341::readWord(uint8_t reg)
{
    _wire->beginTransmission(_address);
    _wire->write(reg);
    if (_wire->endTransmission() != 0)
    {
        return 0; // Error
    }

    if (_wire->requestFrom(_address, (uint8_t)2) != 2)
    {
        return 0; // Error
    }

    uint16_t low = _wire->read();
    uint16_t high = _wire->read();

    return (high << 8) | low; // Little endian
}

void AS7341::readAllChannels(uint16_t *data)
{
    // Reading ASTATUS latches the channel counts
    _wire->beginTransmission(_address);
    _wire->write(AS7341_ASTATUS);
    if (_wire->endTransmission() != 0)
    {
        return; // Error
    }

    if (_wire->requestFrom(_address, (uint8_t)13) != 13)
    {
        return; // Error
    }

    // Skip ASTATUS byte
    _wire->read();

    // Read 6 channels (12 bytes)
    for (int i = 0; i < 6; i++)
    {
        uint16_t low = _wire->read();
        uint16_t high = _wire->read();
        data[i] = (high << 8) | low;
    }
}

bool AS7341::writeByte(uint8_t reg, uint8_t value)
{
    _wire->beginTransmission(_address);
    _wire->write(reg);
    _wire->write(value);
    if (_wire->endTransmission() != 0)
    {
        return false; // Error
    }
    delay(10);
    return true;
}

bool AS7341::writeWord(uint8_t reg, uint16_t value)
{
    _wire->beginTransmission(_address);
    _wire->write(reg);
    _wire->write(value & 0xFF);        // Low byte
    _wire->write((value >> 8) & 0xFF); // High byte
    if (_wire->endTransmission() != 0)
    {
        return false; // Error
    }
    delay(20);
    return true;
}

bool AS7341::writeBurst(uint8_t reg, const uint8_t *data, uint8_t length)
{
    _wire->beginTransmission(_address);
    _wire->write(reg);
    for (uint8_t i = 0; i < length; i++)
    {
        _wire->write(data[i]);
    }
    if (_wire->endTransmission() != 0)
    {
        return false; // Error
    }
    delay(100);
    return true;
}

void AS7341::modifyReg(uint8_t reg, uint8_t mask, bool flag)
{
    uint8_t data = readByte(reg);
    if (flag)
    {
        data |= mask; // Set the bit(s)
    }
    else
    {
        data &= (~mask); // Reset the bit(s)
    }
    writeByte(reg, data);
}

void AS7341::setBank(uint8_t bank)
{
    modifyReg(AS7341_CFG_0, AS7341_CFG_0_REG_BANK, bank != 0);
}
