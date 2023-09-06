#include <iostream>
#include <string.h>

#include "iowkit.h"


int main(int argc, char *argv[])
{
	IOWKIT_HANDLE handle; // First device handle
	IOWKIT_HANDLE iows[IOWKIT_MAX_DEVICES]; //All iow HANDLES
	unsigned short sn[9]; //Serial
	char snt[9]; //Convert serial
	unsigned int pid; //ProductID

	//Open the API/iowkit and get the first HANDLE found
	handle = IowKitOpenDevice();

	if (handle == NULL)
	{
		printf("Error, No IO-Warrior found\n");
		IowKitCloseDevice(handle);
		return 1;
	}

	int numDev = IowKitGetNumDevs();

	//Get all IOWKIT_HANDLES for each devices (incl. the first one from "handle");
	for(int i=0; i<numDev; i++)
	{
		iows[i] = IowKitGetDeviceHandle(i+1);
		pid = IowKitGetProductId(iows[i]);
		IowKitGetSerialNumber(iows[i], sn);

		//Convert serial
		for (int j = 0; j < 9; j++)
			snt[j] = sn[j];

		printf("%d PID %x, S/N \"%s\"\n", i + 1, (unsigned int) pid, snt);
		IowKitSetWriteTimeout(iows[i], 1000);

		//Check for a specific device
		if(strcmp(snt, "000005CE") == 0)
			printf("Found specified IO-Warrior\n");
	}

	//Close the API. not nessessary to close each iow[] handle.
	IowKitCloseDevice(handle);

}