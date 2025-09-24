#include <iostream>
#include <iomanip>
#include <iowkit.h>
#include <cstring>

int main()
{
	IOWKIT_HANDLE handle;
	IOWKIT100_SPECIAL_REPORT report;
	int num = 0;
	handle = IowKitOpenDevice();

	if (handle != NULL)
	{
		//Only continue if IO-Warrior100 found
		if (IowKitGetProductId(handle) == IOWKIT_PID_IOW100)
		{
			//Enable PWM-Mode
			memset(&report, 0x00, IOWKIT100_SPECIAL_REPORT_SIZE);
			report.ReportID = 0x20;	//Special mode PWM
			report.Bytes[0] = 0x01;	//0: Disable, 1..4 Enable channels
			report.Bytes[1] = 0x00;

			IowKitWrite(handle, IOW_PIPE_SPECIAL_MODE, (char*)&report, IOWKIT100_SPECIAL_REPORT_SIZE);
			//No IowKitRead() possible for setting special mode

			//Set PWM Putput
			memset(&report, 0, IOWKIT100_SPECIAL_REPORT_SIZE);
			report.ReportID = 0x21;
			report.Bytes[0] = 0x00;

			//Prescaler 48MHz / (Presc+1)
			report.Bytes[1] = 0x1A;	//Presc -> Demo: 3500
			report.Bytes[2] = 0x04;	//Presc

			//Cycles len in clock cycles
			report.Bytes[3] = 0x36; //Cycle -> Demo: 7000
			report.Bytes[4] = 0x10; //Cycle

			//Channel 0: Length of active low 
			report.Bytes[5] = 0xFF; //Ch0
			report.Bytes[6] = 0x00; //Ch0
			
			//Channel 1: Length of active low 
			report.Bytes[7] = 0x00; //Ch1
			report.Bytes[8] = 0x00; //Ch1
			
			//Channel 2: Length of active low 
			report.Bytes[9] = 0x00; //Ch2
			report.Bytes[10] = 0x00; //Ch2
			
			//Channel 3: Length of active low 
			report.Bytes[11] = 0x00; //Ch3
			report.Bytes[12] = 0x00; //Ch3


			if (IowKitWrite(handle, IOW_PIPE_SPECIAL_MODE, (char*)&report, IOWKIT100_SPECIAL_REPORT_SIZE) != IOWKIT100_SPECIAL_REPORT_SIZE)
				std::cout << "Error: Fail to write IO-Warrior100 (IowKitWrite())" << std::endl;

			//No IowKitRead() possible / nessesary for PWM-Mode
		}
		else
		{
			std::cout << "No IO-Warrior100 found" << std::endl;
		}
	}
	else
	{
		std::cout << "No IO-Warrior found" << std::endl;
	}

	IowKitCloseDevice(handle); //Don't forget to close
}
