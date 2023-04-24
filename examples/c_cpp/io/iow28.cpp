/*
 * Simple IO sample to write and read IO-Ports of IO-Warrior28.
 * Only first found IO-Warrior will be used by default. If no IOW28, exit
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "iowkit.h"

// Clear buffers of iowkit if needed
void drain_buffers(IOWKIT_HANDLE handle)
{
	IOWKIT28_IO_REPORT report;
	memset(&report, 0xFF, IOWKIT28_IO_REPORT_SIZE);

	while (IowKitReadNonBlocking(handle, IOW_PIPE_IO_PINS, (char *)&report, IOWKIT28_IO_REPORT_SIZE))
	{
	}
}

// Write IO-Report to IO-Warrior28
ULONG write_io(IOWKIT_HANDLE handle, IOWKIT28_IO_REPORT report)
{
	return IowKitWrite(handle, IOW_PIPE_IO_PINS, (char *)&report, IOWKIT28_IO_REPORT_SIZE);
}

// Read IO-Ports via interface 0. If no changes this will be block until change detected or timeout reached
ULONG read_io(IOWKIT_HANDLE handle)
{
	IOWKIT28_IO_REPORT report;
	memset(&report, 0xFF, IOWKIT28_IO_REPORT_SIZE);

	if (IowKitRead(handle, IOW_PIPE_IO_PINS, (char *)&report, IOWKIT28_IO_REPORT_SIZE) == IOWKIT28_IO_REPORT_SIZE)
	{
		for (ULONG j = 0; j < (IOWKIT28_IO_REPORT_SIZE - 1); j++)
			printf("%02x ", report.Bytes[j]);

		printf("\n");

		return IOWKIT28_IO_REPORT_SIZE;
	}
	else
		return 0;
}

// Read IO-Ports non blocking via interface 0. Return the last state on IO-Pins.
ULONG read_io_nonblock(IOWKIT_HANDLE handle)
{
	IOWKIT28_IO_REPORT report;
	memset(&report, 0xFF, IOWKIT28_IO_REPORT_SIZE);

	if (IowKitReadNonBlocking(handle, IOW_PIPE_IO_PINS, (char *)&report, IOWKIT28_IO_REPORT_SIZE) == IOWKIT28_IO_REPORT_SIZE)
	{
		for (ULONG j = 0; j < (IOWKIT28_IO_REPORT_SIZE - 1); j++)
			printf("%02x ", report.Bytes[j]);

		printf("\n");

		return IOWKIT28_IO_REPORT_SIZE;
	}
	else
		return 0;
}

// Read IO-Ports via special mode 0xFF to get the last state of the IO-Pins
ULONG read_io_specialmode(IOWKIT_HANDLE handle)
{
	IOWKIT28_SPECIAL_REPORT rep;
	memset(&rep, 0x00, IOWKIT28_SPECIAL_REPORT_SIZE);

	rep.ReportID = 0xFF;
	if (IowKitWrite(handle, IOW_PIPE_SPECIAL_MODE, (char *)&rep, IOWKIT28_SPECIAL_REPORT_SIZE) == IOWKIT28_SPECIAL_REPORT_SIZE)
	{
		if (IowKitRead(handle, IOW_PIPE_SPECIAL_MODE, (char *)&rep, IOWKIT28_SPECIAL_REPORT_SIZE) == IOWKIT28_SPECIAL_REPORT_SIZE)
		{
			for (ULONG j = 0; j < IOWKIT28_IO_REPORT_SIZE; j++)
				printf("%02x ", rep.Bytes[j]);

			printf("\n");

			return IOWKIT28_SPECIAL_REPORT_SIZE;
		}
		else
			return 0;
	}
	else
		return 0;
}

int main(int argc, char *argv[])
{
	IOWKIT_HANDLE handle; // Device handle
	unsigned short sn[9]; // Serialnumber
	char snt[9];		  // Serialnumber as string

	// Open device and use first founc device furhter
	handle = IowKitOpenDevice();
	if (handle == NULL)
	{
		printf("Failed to open device, EXIT\n");
		return 0;
	}

	// Set Timeout for Read, default: INFINITE
	IowKitSetTimeout(handle, 1000);

	// Get product ID
	DWORD pid = IowKitGetProductId(handle);

	if (pid != IOWKIT_PID_IOW28)
	{
		printf("No IO-Warrior28 found on first position");
		IowKitCloseDevice(handle);
		return 1;
	}

	// Get serial number
	IowKitGetSerialNumber(handle, sn);

	// Store the serial number into a string
	for (int j = 0; j < 9; j++)
		snt[j] = sn[j];

	// Get firmware revision
	ULONG rev = IowKitGetRevision(handle);

	// Print out device information
	printf("PID %x (IO-Warrior28), S/N \"%s\", Rev %lX\n", (unsigned int)pid, snt, rev);

	// Write random bits on IO-Ports based on the count of IO-Ports
	printf("Write IO-Port 'P0' with random numbers (0..255)....\n");

	IOWKIT28_IO_REPORT report;
	memset(&report, 0xFF, IOWKIT28_IO_REPORT_SIZE);
	report.ReportID = 0x00;

	srand(time(NULL));
	for (int i = 0; i < 20; i++)
	{
		report.Bytes[0] = rand() % 255; // Write random number (0..255) only on port 'P0' for demo

		printf("Port 0: 0x%02x (%d)\n", report.Bytes[0], report.Bytes[0]);
		if (write_io(handle, report) != IOWKIT28_IO_REPORT_SIZE)
		{
			printf("IowKitWrite() failed. Exit");
			IowKitCloseDevice(handle);
			return 0;
		}

		// Sleep for 100ms
		usleep(100000);
	}

	// Read IO-Ports (if you use a starterkit or evalboard press the button to change the state of the IO-Pin)
	printf("Read IO-Ports (stops after 10 seconds). If you have connected a Starterkit/Evalboard press a button.\n");
	// drain_buffers(handle);

	/*
		//Read IO-Pins by using interface 0
		for (int i = 0; i < 100; i++)
		{
			if (read_io(handle) != IOWKIT28_IO_REPORT_SIZE)
				printf("Error during IowKitRead() or timeout reached\n");

			usleep(100000);
		}
	*/

	// Read IO-Pins by using interface 0 with non blocking read function
	for (int i = 0; i < 100; i++)
	{
		if (read_io_nonblock(handle) != IOWKIT28_IO_REPORT_SIZE)
			printf("-- No new data\n");

		usleep(100000);
	}

	/*
		//Use special mode for read IO-Pins. Can not block
		for (int i = 0; i < 100; i++)
		{
			if (read_io_specialmode(handle) != IOWKIT28_SPECIAL_REPORT_SIZE)
				printf("Error during IowKitRead() by using special mode\n");

			usleep(100000);
		}
	*/

	IowKitCloseDevice(handle);

	return 0;
}