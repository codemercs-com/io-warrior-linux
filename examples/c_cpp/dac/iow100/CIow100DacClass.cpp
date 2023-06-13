#include <string.h>
#include "CIow100DacClass.h"

CIow100DacClass::CIow100DacClass()
{
	m_Handle = NULL;
	m_Dac.channel1 = IOW100_DAC::DAC_MIN;
	m_Dac.channel2 = IOW100_DAC::DAC_MIN;
}

CIow100DacClass::~CIow100DacClass()
{
}

int32_t CIow100DacClass::SetHandle(IOWKIT_HANDLE handle)
{
	ULONG pid = IowKitGetProductId(handle);

	if (pid == IOWKIT_PID_IOW100)
	{
		m_Handle = handle;
		return IOW100_DAC::ERROR_NONE;
	}
	else
	{
		m_Handle = NULL;
		return IOW100_DAC::ERROR_DEVICE;
	}
}

int32_t CIow100DacClass::IowWrite()
{
	IOWKIT100_SPECIAL_REPORT report;
	memset(&report, 0x00, IOWKIT100_SPECIAL_REPORT_SIZE);

	if (m_Handle != NULL)
	{
		report.ReportID = IOW100_DAC::RID_WRITE;            //ReportID for write DAC
		report.Bytes[0] = 0x00;                             //unused
		report.Bytes[1] = m_Dac.channel1 & 0x00FF;			//DAC_0 LSB
		report.Bytes[2] = (m_Dac.channel1 & 0x0F00) >> 8;	//DAC_0 MSB
		report.Bytes[3] = m_Dac.channel2 & 0x00FF;			//DAC_1 LSB
		report.Bytes[4] = (m_Dac.channel2 & 0x0F00) >> 8;	//DAC_1 MSB

		if (IowKitWrite(m_Handle, IOW_PIPE_SPECIAL_MODE, (char*)&report, IOWKIT100_SPECIAL_REPORT_SIZE) == IOWKIT100_SPECIAL_REPORT_SIZE)
			return IOW100_DAC::ERROR_NONE;
		else
			return IOW100_DAC::ERROR_WRITE;

	}else
		return IOW100_DAC::ERROR_HANDLE;
}

int32_t CIow100DacClass::WriteDac(uint16_t value_ch1, uint16_t value_ch2)
{
	m_Dac.channel1 = value_ch1;
	m_Dac.channel2 = value_ch2;

    return IowWrite();
}