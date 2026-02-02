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
#define AS7341_CONFIG_INT_MODE_SYNS 0x01
#define AS7341_MODE_SYNS AS7341_CONFIG_INT_MODE_SYNS
#define AS7341_CONFIG_INT_MODE_SYND 0x03
#define AS7341_MODE_SYND AS7341_CONFIG_INT_MODE_SYND
#define AS7341_CONFIG_INT_SEL 0x04
#define AS7341_CONFIG_LED_SEL 0x08
#define AS7341_STAT 0x71
#define AS7341_STAT_READY 0x01
#define AS7341_STAT_WAIT_SYNC 0x02
#define AS7341_EDGE 0x72
#define AS7341_GPIO 0x73
#define AS7341_GPIO_PD_INT 0x01
#define AS7341_GPIO_PD_GPIO 0x02
#define AS7341_LED 0x74
#define AS7341_LED_LED_ACT 0x80
#define AS7341_ENABLE 0x80
#define AS7341_ENABLE_PON 0x01
#define AS7341_ENABLE_SP_EN 0x02
#define AS7341_ENABLE_WEN 0x08
#define AS7341_ENABLE_SMUXEN 0x10
#define AS7341_ENABLE_FDEN 0x40
#define AS7341_ATIME 0x81
#define AS7341_WTIME 0x83
#define AS7341_SP_TH_LOW 0x84
#define AS7341_SP_TH_L_LSB 0x84
#define AS7341_SP_TH_L_MSB 0x85
#define AS7341_SP_TH_HIGH 0x86
#define AS7341_SP_TH_H_LSB 0x86
#define AS7341_SP_TH_H_MSB 0x87
#define AS7341_AUXID 0x90
#define AS7341_REVID 0x91
#define AS7341_ID 0x92
#define AS7341_STATUS 0x93
#define AS7341_STATUS_ASAT 0x80
#define AS7341_STATUS_AINT 0x08
#define AS7341_STATUS_FINT 0x04
#define AS7341_STATUS_C_INT 0x02
#define AS7341_STATUS_SINT 0x01
#define AS7341_ASTATUS 0x94
#define AS7341_ASTATUS_ASAT_STATUS 0x80
#define AS7341_ASTATUS_AGAIN_STATUS 0x0F
#define AS7341_CH_DATA 0x95
#define AS7341_CH0_DATA_L 0x95
#define AS7341_CH0_DATA_H 0x96
#define AS7341_CH1_DATA_L 0x97
#define AS7341_CH1_DATA_H 0x98
#define AS7341_CH2_DATA_L 0x99
#define AS7341_CH2_DATA_H 0x9A
#define AS7341_CH3_DATA_L 0x9B
#define AS7341_CH3_DATA_H 0x9C
#define AS7341_CH4_DATA_L 0x9D
#define AS7341_CH4_DATA_H 0x9E
#define AS7341_CH5_DATA_L 0x9F
#define AS7341_CH5_DATA_H 0xA0
#define AS7341_STATUS_2 0xA3
#define AS7341_STATUS_2_AVALID 0x40
#define AS7341_STATUS_3 0xA4
#define AS7341_STATUS_5 0xA6
#define AS7341_STATUS_6 0xA7
#define AS7341_CFG_0 0xA9
#define AS7341_CFG_0_WLONG 0x04
#define AS7341_CFG_0_REG_BANK 0x10
#define AS7341_CFG_0_LOW_POWER 0x20
#define AS7341_CFG_1 0xAA
#define AS7341_CFG_3 0xAC
#define AS7341_CFG_6 0xAF
#define AS7341_CFG_6_SMUX_CMD_ROM 0x00
#define AS7341_CFG_6_SMUX_CMD_READ 0x08
#define AS7341_CFG_6_SMUX_CMD_WRITE 0x10
#define AS7341_CFG_8 0xB1
#define AS7341_CFG_9 0xB2
#define AS7341_CFG_10 0xB3
#define AS7341_CFG_12 0xB5
#define AS7341_PERS 0xBD
#define AS7341_GPIO_2 0xBE
#define AS7341_GPIO_2_GPIO_IN 0x01
#define AS7341_GPIO_2_GPIO_OUT 0x02
#define AS7341_GPIO_2_GPIO_IN_EN 0x04
#define AS7341_GPIO_2_GPIO_INV 0x08
#define AS7341_ASTEP 0xCA
#define AS7341_ASTEP_L 0xCA
#define AS7341_ASTEP_H 0xCB
#define AS7341_AGC_GAIN_MAX 0xCF
#define AS7341_AZ_CONFIG 0xD6
#define AS7341_FD_TIME_1 0xD8
#define AS7341_FD_TIME_2 0xDA
#define AS7341_FD_CFG0 0xD7
#define AS7341_FD_STATUS 0xDB
#define AS7341_FD_STATUS_FD_100HZ 0x01
#define AS7341_FD_STATUS_FD_120HZ 0x02
#define AS7341_FD_STATUS_FD_100_VALID 0x04
#define AS7341_FD_STATUS_FD_120_VALID 0x08
#define AS7341_FD_STATUS_FD_SAT_DETECT 0x10
#define AS7341_FD_STATUS_FD_MEAS_VALID 0x20
#define AS7341_INTENAB 0xF9
#define AS7341_INTENAB_SP_IEN 0x08
#define AS7341_CONTROL 0xFA
#define AS7341_FIFO_MAP 0xFC
#define AS7341_FIFO_LVL 0xFD
#define AS7341_FDATA 0xFE
#define AS7341_FDATA_L 0xFE
#define AS7341_FDATA_H 0xFF

// SMUX configurations
extern const uint8_t SMUX_F1F4CN[20];
extern const uint8_t SMUX_F5F8CN[20];
extern const uint8_t SMUX_FD[20];

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

    // Additional methods from Python implementation
    void disable();
    bool measurementCompleted();
    void setSpectralMeasurement(bool flag);
    void setSmux(bool flag);
    uint8_t getFlickerFrequency();
    void setFlickerDetection(bool flag);
    void setGpioInput(bool enable);
    bool getGpioValue();
    void setGpioOutput(bool inverted);
    void setGpioInverted(bool flag);
    void setGpioMask(uint8_t mask);
    float getIntegrationTime();
    uint8_t getAgain();
    float getAgainFactor();
    void setAgainFactor(float factor);
    void setWen(bool flag);
    void setWtime(uint8_t code);
    void checkInterrupt();
    void clearInterrupt();
    void setSpectralInterrupt(bool flag);
    void setInterruptPersistence(uint8_t value);
    void setSpectralThresholdChannel(uint8_t value);
    void setThresholds(uint16_t lo, uint16_t hi);
    void getSynsInt();

private:
    TwoWire *_wire;
    uint8_t _address;
    uint8_t _measureMode;
    bool _connected;
    uint8_t _currentGain;
    uint8_t _currentATime;

    // I2C communication methods
    uint8_t readByte(uint8_t reg);
    uint16_t readWord(uint8_t reg);
    void readAllChannels(uint16_t *data);
    bool writeByte(uint8_t reg, uint8_t value);
    bool writeWord(uint8_t reg, uint16_t value);
    bool writeBurst(uint8_t reg, const uint8_t *data, uint8_t length);
    void modifyReg(uint8_t reg, uint8_t mask, bool flag);
    void setBank(uint8_t bank);
    void channelSelect(const char *selection);
};

#endif
