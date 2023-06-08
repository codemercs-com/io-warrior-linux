/*
* Sample to use the ADCs of IO-Warrior100 in single shot mode
*/

#include <string.h>
#include <stdio.h>

#include "iowkit.h"
#include "CIow100AdcClass.h"


int main(int argc, char *argv[])
{
    IOWKIT_HANDLE handle;   // Device handle
    CIow100AdcClass adc;    //Class for using the DAC mode
    uint8_t error = IOW100_ADC::ERROR_NONE;

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

    error = adc.SetHandle(handle);      //Set IOWKIT_HANDLE to class
    if (error != IOW100_ADC::ERROR_NONE)
    {
        printf("Error: IOWKIT_HANDLE incorrect");
        IowKitCloseDevice(handle);
        return 1;
    }

    //Enable DAC, locking IO-Pins for ADC only, use all 4 channel, for singleshot samplerate will be ignored
    error = adc.Enable(IOW100_ADC::CHANNEL_4, IOW100_ADC::MODE_SINGLE, IOW100_ADC::ADC_0KHZ);
    if (error != IOW100_ADC::ERROR_NONE)
    {
        printf("Error: Can not enable ADC mode");
        IowKitCloseDevice(handle);
        return 1;
    }

    error = adc.GetCalibration();//Read the calibration data for internal use
    if (error != IOW100_ADC::ERROR_NONE)
    {
        printf("Error: Can not get calibration data");
        IowKitCloseDevice(handle);
        return 1;
    }

    error = adc.SingleShot();   //Run a single shot to get the actual ADC values
    if (error != IOW100_ADC::ERROR_NONE)
    {
        printf("Error: Can not write/read data");
        IowKitCloseDevice(handle);
        return 1;
    }

    //Get the ADC raw values
    uint16_t channel_0 = adc.GetChannel(IOW100_ADC::CH0);
    uint16_t channel_1 = adc.GetChannel(IOW100_ADC::CH1);
    uint16_t channel_2 = adc.GetChannel(IOW100_ADC::CH2);
    uint16_t channel_3 = adc.GetChannel(IOW100_ADC::CH3);

    //Get the voltage
    float voltage_0 = adc.GetVoltage(IOW100_ADC::CH0);
    float voltage_1 = adc.GetVoltage(IOW100_ADC::CH1);
    float voltage_2 = adc.GetVoltage(IOW100_ADC::CH2);
    float voltage_3 = adc.GetVoltage(IOW100_ADC::CH3);

    //Output into terminal
    printf("==== ADC Data ====\n");
    printf("Channel 0: %i | Voltage: %0.3f V\n", channel_0, voltage_0);
    printf("Channel 1: %i | Voltage: %0.3f V\n", channel_1, voltage_1);
    printf("Channel 2: %i | Voltage: %0.3f V\n", channel_2, voltage_2);
    printf("Channel 3: %i | Voltage: %0.3f V\n", channel_3, voltage_3);
    printf("==== ADC END ====\n");
    
    adc.Disable();  //Disable ADC mode and free IO-Pins. 

    IowKitCloseDevice(handle);  //Close iowkit at the end to free device for other tools
    return 0;
}