/*
 * Class for easy use of the DAC function of the IO-Warrior100
 */

#include <stdint.h>
#include "iowkit.h"

namespace IOW100_DAC
{
	//ReportIDs for DAC
	const uint8_t RID_ENABLE = 0x24;
	const uint8_t RID_WRITE = 0x25;

	//Channel enable number for getting data
	const uint8_t DISABLE = 0x00;
	const uint8_t CHANNEL_1 = 0x01;
	const uint8_t CHANNEL_2 = 0x02;

	//Some flags
	const uint8_t FLAG_NONE = 0x00;
	const uint8_t FLAG_ERROR = 0x01;

	//Return errors
	const uint8_t ERROR_NONE = 0x00;
	const uint8_t ERROR_HANDLE = 0x01;
	const uint8_t ERROR_WRITE = 0x02;
	const uint8_t ERROR_READ = 0x04;
	const uint8_t ERROR_DEVICE = 0x08;
	const uint8_t ERROR_PORT = 0x10;

	const uint16_t DAC_MIN = 0x0000;
	const uint16_t DAC_MAX = 0x0FFF;
}

class CIow100DacClass
{
public:
	CIow100DacClass();
	virtual ~CIow100DacClass();

    int32_t SetHandle(IOWKIT_HANDLE handle);
    int32_t WriteDac(uint16_t value_ch1, uint16_t value_ch2);		//Set DAC values (only set and write)

private:
	typedef struct DAC_STRUCT
	{
		uint16_t channel1;
		uint16_t channel2;

	}DAC_STRUCT;

	IOWKIT_HANDLE m_Handle;
	DAC_STRUCT m_Dac;
    int32_t IowWrite();		//Write function
};
