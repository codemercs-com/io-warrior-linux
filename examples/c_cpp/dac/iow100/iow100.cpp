/*
* DAC sample to write the DACs of IO-Warrior100.
*/

#include <string.h>
#include <stdio.h>

#include "iowkit.h"
#include "CIow100DacClass.h"

int main(int argc, char *argv[])
{
    IOWKIT_HANDLE handle;   // Device handle
    CIow100DacClass dac;    //Class for using the DAC mode

    handle = IowKitOpenDevice();    //Open iowkit and return 1st. IO-Warrior found
	
    if (handle == NULL)
	{
		printf("Failed to open device, EXIT\n");
		return 0;
	}

	DWORD pid = IowKitGetProductId(handle); // Get product ID

    //DAC mode only available for IO-Warrior100
	if (pid != IOWKIT_PID_IOW100)
	{
		printf("No IO-Warrior100 found");
		IowKitCloseDevice(handle);
		return 1;
	}

    dac.SetHandle(handle);      //Set IOWKIT_HANDLE to class

    //Enable DAC, locking IO-Pins for DAC only
    IOWKIT100_IO_REPORT report;
	memset(&report, 0xFF, IOWKIT100_IO_REPORT_SIZE);
	report.ReportID = IOW100_DAC::RID_ENABLE;         //Special mode function 
	report.Bytes[0] = IOW100_DAC::CHANNEL_2;        //Enable both DAC outputs

	IowKitWrite(handle, IOW_PIPE_SPECIAL_MODE, (char*)&report, IOWKIT100_SPECIAL_REPORT_SIZE);

    //Set DACs and write to
    uint16_t dac_0 = IOW100_DAC::DAC_MAX / 2;
    uint16_t dac_1 = IOW100_DAC::DAC_MAX / 3;

    dac.WriteDac(dac_0, dac_1); //Write the DAC values to the IO-Warrior100


    //Disable DAC mode and free IO-Pins. 
    //This has been commented out for this example to see the DAC on a multimeter.
    //Disable the DAC will restore the IO-Pins and its output voltage
    /*
    memset(&report, 0xFF, IOWKIT100_IO_REPORT_SIZE);
	report.ReportID = IOW100_DAC::RID_ENABLE;         //Special mode function 
	report.Bytes[0] = IOW100_DAC::DISABLE;        //Enable both DAC outputs

	IowKitWrite(handle, IOW_PIPE_SPECIAL_MODE, (char*)&report, IOWKIT100_SPECIAL_REPORT_SIZE);
    */

    IowKitCloseDevice(handle);  //Close iowkit at the end to free device for other tools
    return 0;
}