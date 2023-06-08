/*
 * Class for easy use of the ADC function of the IO-Warrior100.
 * This class contains only the single shot functionality.
 */

#include <stdint.h>
#include "iowkit.h"

namespace IOW100_ADC
{
    // ReportIDs
    const uint8_t RID_MODE = 0x1C;
    const uint8_t RID_CONTINUOUS = 0x1D;
    const uint8_t RID_DATA = 0x1E;

    // Channel enable number for getting data
    const uint8_t DISABLE = 0x00;
    const uint8_t ENABLE = 0x01;

    // Mode bits for getting calibration or single shot data
    const uint8_t COMMAND_CALIBRATION = 0x00;
    const uint8_t COMMAND_DATA = 0x01;

    // Channel selection
    const uint8_t CHANNEL_0 = 0x00; // Enables only the temperatur sensor from the STM32
    const uint8_t CHANNEL_1 = 0x01;
    const uint8_t CHANNEL_2 = 0x02;
    const uint8_t CHANNEL_4 = 0x04;

    // Channel number for getting
    const uint8_t CH0 = 0x01;
    const uint8_t CH1 = 0x02;
    const uint8_t CH2 = 0x03;
    const uint8_t CH3 = 0x04;

    // Mode
    const uint8_t MODE_SINGLE = 0x00;
    const uint8_t MODE_CONTINUOUS = 0x01;

    // Some flags
    const uint8_t FLAG_NONE = 0x00;
    const uint8_t FLAG_ERROR = 0x01;

    // Return errors
    const uint8_t ERROR_NONE = 0x00;
    const uint8_t ERROR_HANDLE = 0x01;
    const uint8_t ERROR_WRITE = 0x02;
    const uint8_t ERROR_READ = 0x04;
    const uint8_t ERROR_DEVICE = 0x08;
    const uint8_t ERROR_PORT = 0x10;

    // Max ADC values
    const uint16_t ADC_MIN = 0x0000;
    const uint16_t ADC_MAX = 0x0FFF;

    // Table for frequency
    // Max sample rate for 4 channel: 6kHz, 2 channel: 15kHz, 1 channel: 30kHz
    // sample rate ist only for MODE_CONTINUOUS and will be ignored in single shot mode
    const uint8_t ADC_0KHZ = 0; // Dummy for Singleshot mode
    const uint8_t ADC_1KHZ = 0;
    const uint8_t ADC_2KHZ = 1;
    const uint8_t ADC_3KHZ = 2;
    const uint8_t ADC_4KHZ = 3;
    const uint8_t ADC_6KHZ = 4;
    const uint8_t ADC_8KHZ = 5;
    const uint8_t ADC_10KHZ = 6;
    const uint8_t ADC_12KHZ = 7;
    const uint8_t ADC_15KHZ = 8;
    const uint8_t ADC_16KHZ = 9;
    const uint8_t ADC_20KHZ = 10;
    const uint8_t ADC_24KHZ = 11;
    const uint8_t ADC_30KHZ = 12;
}

class CIow100AdcClass
{
public:
    // Struct for ADC data
    typedef struct IOW100_ADC_DATASTRUCT
    {
        uint8_t seq;
        uint16_t ch1;
        uint16_t ch2;
        uint16_t ch3;
        uint16_t ch4;
        uint16_t temperature;

    } IOW100_ADC_DATASTRUCT;

public:
    CIow100AdcClass();
    virtual ~CIow100AdcClass();

    int32_t SetHandle(IOWKIT_HANDLE handle);    //Set IOWKIT_HANDLE
    int32_t Enable(uint8_t channel, uint8_t mode, uint8_t sample);  //Enables the ADC function.
    int32_t Disable();  //Disable the ADC function
    int32_t GetCalibration();   //Get the calibration data from IO-Warrior100
    int32_t SingleShot();   //Perform a singleshot to get the actual ADC values
    uint16_t GetChannel(uint8_t channel);   //Return ADC raw data
    IOW100_ADC_DATASTRUCT GetChannel();     //Return ADC raw data as struct
    double GetVoltage(uint8_t channel);     //Return ADC voltage

private:
    // Struct for internal calibration of IO-Warrior100
    typedef struct IOW100_ADC_CONFIGSTRUCT
    {
        uint16_t VrefCal;
        uint16_t TempCal1;
        uint16_t TempCal2;
        uint16_t Vref;
        uint16_t Temp;
    } IOW100_ADC_CONFIGSTRUCT;

    IOWKIT_HANDLE m_Handle;
    IOWKIT100_SPECIAL_REPORT m_Report;
    IOW100_ADC_DATASTRUCT m_Data;
    IOW100_ADC_CONFIGSTRUCT m_Config;

    int32_t IowEnable(uint8_t enable, uint8_t channel, uint8_t mode, uint8_t sample); // Function to enable/disable ADC mode
    int32_t GetCalibrationData();
};