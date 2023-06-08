#include <string.h>
#include "CIow100AdcClass.h"

CIow100AdcClass::CIow100AdcClass()
{
    m_Handle = NULL;
}

CIow100AdcClass::~CIow100AdcClass()
{
}

int32_t CIow100AdcClass::SetHandle(IOWKIT_HANDLE handle)
{
    ULONG pid = IowKitGetProductId(handle);

    if (pid == IOWKIT_PID_IOW100)
    {
        m_Handle = handle;
        return IOW100_ADC::ERROR_NONE;
    }
    else
    {
        m_Handle = NULL;
        return IOW100_ADC::ERROR_DEVICE;
    }
}

int32_t CIow100AdcClass::IowEnable(uint8_t enable, uint8_t channel, uint8_t mode, uint8_t sample)
{
    if (m_Handle != NULL)
    {
        IOWKIT100_SPECIAL_REPORT report;
        memset(&report, 0, IOWKIT100_SPECIAL_REPORT_SIZE);

        report.ReportID = IOW100_ADC::RID_MODE;
        report.Bytes[0] = enable;  // Enable / Disable ADC mode
        report.Bytes[1] = channel; // Channel count
        report.Bytes[2] = 0x00;    // Must be 0
        report.Bytes[3] = 0x00;    // Must be 0
        report.Bytes[4] = mode;    // Single short or continues mode
        report.Bytes[5] = sample;  // Samplerate

        if (IowKitWrite(m_Handle, IOW_PIPE_ADC_MODE, (char *)&report, IOWKIT100_SPECIAL_REPORT_SIZE) == IOWKIT100_SPECIAL_REPORT_SIZE)
            return IOW100_ADC::ERROR_NONE;
        else
            return IOW100_ADC::ERROR_WRITE;
    }
    else
        return IOW100_ADC::ERROR_HANDLE;
}

int32_t CIow100AdcClass::GetCalibrationData()
{
    IOWKIT100_SPECIAL_REPORT report;
    memset(&report, 0, IOWKIT100_SPECIAL_REPORT_SIZE);

    if (m_Handle != NULL)
    {
        memset(&report, 0, IOWKIT100_SPECIAL_REPORT_SIZE);
        report.ReportID = IOW100_ADC::RID_DATA;
        report.Bytes[0] = IOW100_ADC::COMMAND_CALIBRATION;

        if (IowKitWrite(m_Handle, IOW_PIPE_ADC_MODE, (char *)&report, IOWKIT100_SPECIAL_REPORT_SIZE) == IOWKIT100_SPECIAL_REPORT_SIZE)
        {
            if (IowKitRead(m_Handle, IOW_PIPE_ADC_MODE, (char *)&report, IOWKIT100_SPECIAL_REPORT_SIZE) == IOWKIT100_SPECIAL_REPORT_SIZE)
            {
                m_Config.VrefCal = (report.Bytes[2] << 8) | report.Bytes[1];
                m_Config.TempCal1 = (report.Bytes[4] << 8) | report.Bytes[3];
                m_Config.TempCal2 = (report.Bytes[6] << 8) | report.Bytes[5];
                m_Config.Vref = (report.Bytes[8] << 8) | report.Bytes[7];
                m_Config.Temp = (report.Bytes[10] << 8) | report.Bytes[9];

                return IOW100_ADC::ERROR_NONE;
            }
            else
                return IOW100_ADC::ERROR_READ;
        }
        else
            return IOW100_ADC::ERROR_WRITE;
    }
    else
        return IOW100_ADC::ERROR_HANDLE;
}

int32_t CIow100AdcClass::GetCalibration()
{
    return GetCalibrationData();
}

int32_t CIow100AdcClass::Enable(uint8_t channel, uint8_t mode, uint8_t sample)
{
    return IowEnable(IOW100_ADC::ENABLE, channel, mode, sample);
}

int32_t CIow100AdcClass::Disable()
{
    return IowEnable(IOW100_ADC::DISABLE, 0, 0, 0);
}

int32_t CIow100AdcClass::SingleShot()
{
    IOWKIT100_SPECIAL_REPORT report;
    memset(&report, 0, IOWKIT100_SPECIAL_REPORT_SIZE);

    if (m_Handle != NULL)
    {
        memset(&report, 0, IOWKIT100_SPECIAL_REPORT_SIZE);
        report.ReportID = IOW100_ADC::RID_DATA;
        report.Bytes[0] = IOW100_ADC::COMMAND_DATA;

        if (IowKitWrite(m_Handle, IOW_PIPE_ADC_MODE, (char *)&report, IOWKIT100_SPECIAL_REPORT_SIZE) == IOWKIT100_SPECIAL_REPORT_SIZE)
        {
            if (IowKitRead(m_Handle, IOW_PIPE_ADC_MODE, (char *)&report, IOWKIT100_SPECIAL_REPORT_SIZE) == IOWKIT100_SPECIAL_REPORT_SIZE)
            {
                m_Data.seq = report.Bytes[0];
                m_Data.ch1 = (report.Bytes[2] << 8) | report.Bytes[1];
                m_Data.ch2 = (report.Bytes[4] << 8) | report.Bytes[3];
                m_Data.ch3 = (report.Bytes[6] << 8) | report.Bytes[5];
                m_Data.ch4 = (report.Bytes[8] << 8) | report.Bytes[7];

                return IOW100_ADC::ERROR_NONE;
            }
            else
                return IOW100_ADC::ERROR_READ;
        }
        else
            return IOW100_ADC::ERROR_WRITE;
    }
    else
        return IOW100_ADC::ERROR_HANDLE;
}

uint16_t CIow100AdcClass::GetChannel(uint8_t channel)
{
    switch (channel)
    {
    case 1:
        return m_Data.ch1;
    case 2:
        return m_Data.ch2;
    case 3:
        return m_Data.ch3;
    case 4:
        return m_Data.ch4;
    default:
        return -1;
    }
}

double CIow100AdcClass::GetVoltage(uint8_t channel)
{
    switch (channel)
    {
    case 1:
        return 3.3 /*V*/ * m_Config.VrefCal * m_Data.ch1 / m_Config.Vref / 4095.0;
    case 2:
        return 3.3 /*V*/ * m_Config.VrefCal * m_Data.ch2 / m_Config.Vref / 4095.0;
    case 3:
        return 3.3 /*V*/ * m_Config.VrefCal * m_Data.ch3 / m_Config.Vref / 4095.0;
    case 4:
        return 3.3 /*V*/ * m_Config.VrefCal * m_Data.ch4 / m_Config.Vref / 4095.0;
    default:
        return -1;
    }
}