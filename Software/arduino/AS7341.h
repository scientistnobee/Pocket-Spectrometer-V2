#ifndef AS7341_H
#define AS7341_H

#include <Wire.h>

// AS7341 I2C address
#define AS7341_I2C_ADDRESS 0x39
#define AS7341_ID_VALUE 0x24

// Register addresses and constants
#define AS7341_CONFIG 0x70
#define AS7341_CONFIG_INT_MODE_SPM 0x00
#define AS7341_MODE_SPM AS7341_CONFIG_INT_MODE_SPM
#define AS7341_CONFIG_LED_SEL 0x08
#define AS7341_ENABLE 0x80
#define AS7341_ENABLE_PON 0x01
#define AS7341_ENABLE_SP_EN 0x02
#define AS7341_ENABLE_SMUXEN 0x10
#define AS7341_ATIME 0x81
#define AS7341_ID 0x92
#define AS7341_STATUS_2 0xA3
#define AS7341_STATUS_2_AVALID 0x40
#define AS7341_CFG_0 0xA9
#define AS7341_CFG_0_REG_BANK 0x10
#define AS7341_CFG_1 0xAA
#define AS7341_CFG_6 0xAF
#define AS7341_CFG_6_SMUX_CMD_WRITE 0x10
#define AS7341_ASTEP 0xCA
#define AS7341_LED 0x74
#define AS7341_LED_LED_ACT 0x80
#define AS7341_CH0_DATA_L 0x95

class AS7341
{
public:
    AS7341(TwoWire &wire = Wire);
    bool begin();
    bool isConnected();
    void setMeasureMode(uint8_t mode);
    void setGain(uint8_t gain);
    void setATime(uint8_t atime);
    void setAStep(uint16_t astep);
    void startMeasure(const char *selection);
    uint16_t getChannelData(uint8_t channel);
    void getSpectralData(uint16_t *data);
    void enableLED(bool enable);
    void setLEDCurrent(uint8_t current);

private:
    TwoWire *_wire;
    uint8_t _address;
    uint8_t _measureMode;
    bool _connected;
    uint8_t _currentGain;
    uint8_t _currentATime;

    // For mock data generation
    uint16_t generateMockData(uint8_t channel);

    // These would be real I2C functions in a real implementation
    uint8_t readByte(uint8_t reg);
    uint16_t readWord(uint8_t reg);
    bool writeByte(uint8_t reg, uint8_t value);
    bool writeWord(uint8_t reg, uint16_t value);
    void modifyReg(uint8_t reg, uint8_t mask, bool flag);
    void setBank(uint8_t bank);
    void setSpectralMeasurement(bool flag);
    void setSMUX(bool flag);
    void channelSelect(const char *selection);
};

#endif
