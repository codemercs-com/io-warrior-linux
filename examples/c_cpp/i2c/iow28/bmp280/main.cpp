#include <iostream>
#include <string.h>
#include <cstring>
#include <stdint.h>
#include <chrono>
#include <thread>
#include <iomanip>
#include "iowkit.h"

// Register für BME280
#define REG__CALIB00 0x88
// #define REG__CALIB01  0x89
//.......
// #define REG__CALIB25  0xA1
#define REG__ID 0xD0
#define REG__RESET 0xD0
#define REG__CALIB26 0xE1
// #define REG__CALIB27  0xE2
//.......
// #define REG__CALIB41  0xF0
#define REG__CTRL_HUM 0xF2
#define REG__STATUS 0xF3
#define REG__CTR_MEAS 0xF4
#define REG__CONFIG 0xF5

#define REG__PRESS_MSB 0xF7
#define REG__PRESS_LSB 0xF8
#define REG__PRESS_XSB 0xF9

#define REG__TEMP_MSB 0xFA
#define REG__TEMP_LSB 0xFB
#define REG__TEMP_XSB 0xFC

#define REG__HUM_MSB 0xFD
#define REG__HUM_LSB 0xFE

// BMP280 default 8bit I2C address
#define I2C_ADR 0xEC

/***** Stuff for BMP280/BME280 *****/
typedef int32_t BME280_S32_t;
typedef uint32_t BME280_U32_t;
typedef uint16_t BME280_S64_t;

// Compensation from datasheet
struct CompensationData
{
	uint16_t dig_T1; // 0x88 + 0x89
	int16_t dig_T2;	 // 0x8A + 0x8B
	int16_t dig_T3;	 // 0x8C + 0x8D
	uint16_t dig_P1; // 0x8E + 0x8F
	int16_t dig_P2;	 // 0x90 + 0x91
	int16_t dig_P3;	 // 0x92 + 0x93
	int16_t dig_P4;	 // 0x94 + 0x95
	int16_t dig_P5;	 // 0x96 + 0x97
	int16_t dig_P6;	 // 0x98 + 0x99
	int16_t dig_P7;	 // 0x9A + 0x9B
	int16_t dig_P8;	 // 0x9C + 0x9D
	int16_t dig_P9;	 // 0x9E + 0x9F
	uint8_t dig_H1;	 // 0xA1
	int16_t dig_H2;	 // 0xE1 + 0xE2
	uint8_t dig_H3;	 // 0xE3
	int16_t dig_H4;	 // 0xE4 + 0xE5(3:0)
	int16_t dig_H5;	 // 0xE5(7:4) + 0xE6
	uint8_t dig_H6;	 // 0xE7
};

struct SensorData
{
	int32_t pressure;	// msb, lsb, xlsb
	int32_t temperatur; // msb, lsb, xlsb
	int32_t humidity;	// msb lsb
};

struct TemperaturData
{
	BME280_S32_t temperature;
	BME280_S32_t tfine;
};

// Some globals
CompensationData m_Compensation;

// Returns temperature in DegC, resolution is 0.01 DegC. Output value of “5123” equals 51.23 DegC.
// t_fine carries fine temperature as global value
TemperaturData /*BME280_S32_t*/ BME280_compensate_T_int32(BME280_S32_t adc_T)
{
	TemperaturData data;
	BME280_S32_t var1, var2;//, T;

	var1 = ((((adc_T >> 3) - ((BME280_S32_t)m_Compensation.dig_T1 << 1))) * ((BME280_S32_t)m_Compensation.dig_T2)) >> 11;
	var2 = (((((adc_T >> 4) - ((BME280_S32_t)m_Compensation.dig_T1)) * ((adc_T >> 4) - ((BME280_S32_t)m_Compensation.dig_T1))) >> 12) * ((BME280_S32_t)m_Compensation.dig_T3)) >> 14;

	data.tfine = var1 + var2;
	data.temperature = ((var1 + var2) * 5 + 128) >> 8;
	return data;
}

// Returns pressure in Pa as unsigned 32 bit integer in Q24.8 format (24 integer bits and 8 fractional bits).
// Output value of “24674867” represents 24674867/256 = 96386.2 Pa = 963.862 hPa
BME280_U32_t BME280_compensate_P_int64(BME280_S32_t adc_P, BME280_S32_t tfine)
{
	BME280_S64_t var1, var2, p;

	var1 = ((BME280_S64_t)tfine) - 128000;
	var2 = var1 * var1 * (BME280_S64_t)m_Compensation.dig_P6;
	var2 = var2 + ((var1 * (BME280_S64_t)m_Compensation.dig_P5) << 17);
	var2 = var2 + (((BME280_S64_t)m_Compensation.dig_P4) << 35);
	var1 = ((var1 * var1 * (BME280_S64_t)m_Compensation.dig_P3) >> 8) + ((var1 * (BME280_S64_t)m_Compensation.dig_P2) << 12);
	var1 = (((((BME280_S64_t)1) << 47) + var1)) * ((BME280_S64_t)m_Compensation.dig_P1) >> 33;

	//avoid exception caused by division by zero
	if (var1 == 0)
	{
		var1 = 1; 
		//return 0; // orignal from datasheet
	}

	p = 1048576 - ((BME280_S64_t)adc_P);
	p = (((p << 31) - var2) * 3125) / var1;
	var1 = (((BME280_S64_t)m_Compensation.dig_P9) * (p >> 13) * (p >> 13)) >> 25;
	var2 = (((BME280_S64_t)m_Compensation.dig_P8) * p) >> 19;
	p = ((p + var1 + var2) >> 8) + (((BME280_S64_t)m_Compensation.dig_P7) << 4);

	return (BME280_U32_t)p;
}

// Returns humidity in %RH as unsigned 32 bit integer in Q22.10 format (22 integer and 10 fractional bits).
// Output value of “47445” represents 47445/1024 = 46.333 %RH
BME280_U32_t BME280_compensate_H_int32(BME280_S32_t adc_H, BME280_S32_t tfine)
{
	BME280_S32_t v_x1_u32r;

	v_x1_u32r = (tfine - ((BME280_S32_t)76800));
	v_x1_u32r = (((((adc_H << 14) - (((BME280_S32_t)m_Compensation.dig_H4) << 20) - (((BME280_S32_t)m_Compensation.dig_H5) * v_x1_u32r)) + 
	((BME280_S32_t)16384)) >> 15) * (((((((v_x1_u32r * ((BME280_S32_t)m_Compensation.dig_H6)) >> 10) *
	(((v_x1_u32r * ((BME280_S32_t)m_Compensation.dig_H3)) >> 11) + ((BME280_S32_t)32768))) >> 10) +
	((BME280_S32_t)2097152)) * ((BME280_S32_t)m_Compensation.dig_H2) + 8192) >> 14));
	v_x1_u32r = (v_x1_u32r - (((((v_x1_u32r >> 15) * (v_x1_u32r >> 15)) >> 7) * ((BME280_S32_t)m_Compensation.dig_H1)) >> 4));
	v_x1_u32r = (v_x1_u32r < 0 ? 0 : v_x1_u32r);
	v_x1_u32r = (v_x1_u32r > 419430400 ? 419430400 : v_x1_u32r);

	return (BME280_U32_t)(v_x1_u32r >> 12);
}

CompensationData ReadCalibration(IOWKIT_HANDLE handle)
{
	CompensationData data;
	IOWKIT28_SPECIAL_REPORT report;

	memset(&report, 0x00, IOWKIT28_SPECIAL_REPORT_SIZE);
	report.ReportID = 0x02;
	report.Bytes[0] = 0xC2;
	report.Bytes[1] = I2C_ADR;
	report.Bytes[2] = REG__CALIB00;

	if (IowKitWrite(handle, IOW_PIPE_I2C_MODE, (char*) &report, IOWKIT28_SPECIAL_REPORT_SIZE) == IOWKIT28_SPECIAL_REPORT_SIZE)
	{
		if (IowKitRead(handle, IOW_PIPE_I2C_MODE, (char*) &report, IOWKIT28_SPECIAL_REPORT_SIZE) == IOWKIT28_SPECIAL_REPORT_SIZE)
		{
			// Answere from I2C device (ACK, errors, etc)
			// report[0] -> ReportID
			// report[1] -> flags
			// report[2] -> code
		}
		else
			std::cout << __FUNCTION__ << ": IowKitWrite() error" << std::endl;
	}
	else
		std::cout << __FUNCTION__ << ": IowKitRead() error" << std::endl;

	// Die Daten auslesen
	memset(&report, 0x00, IOWKIT28_SPECIAL_REPORT_SIZE);
	report.ReportID = 0x03;					   // Write to I2C-device
	report.Bytes[0] = 26;
	report.Bytes[1] = (uint8_t)(I2C_ADR | 0x01); // I2C-address fürs Lesen einstellen
	report.Bytes[2] = 0x00;

	if (IowKitWrite(handle, IOW_PIPE_I2C_MODE, (char*) &report, IOWKIT28_SPECIAL_REPORT_SIZE) == IOWKIT28_SPECIAL_REPORT_SIZE)
	{
		if (IowKitRead(handle, IOW_PIPE_I2C_MODE, (char*) &report, IOWKIT28_SPECIAL_REPORT_SIZE) == IOWKIT28_SPECIAL_REPORT_SIZE)
		{
			data.dig_T1 = ((report.Bytes[2] << 8) | report.Bytes[1]);
			data.dig_T2 = ((report.Bytes[4] << 8) | report.Bytes[3]);
			data.dig_T3 = ((report.Bytes[6] << 8) | report.Bytes[5]);
			data.dig_P1 = ((report.Bytes[8] << 8) | report.Bytes[7]);
			data.dig_P2 = ((report.Bytes[10] << 8) | report.Bytes[9]);
			data.dig_P3 = ((report.Bytes[12] << 8) | report.Bytes[11]);
			data.dig_P4 = ((report.Bytes[14] << 8) | report.Bytes[13]);
			data.dig_P5 = ((report.Bytes[16] << 8) | report.Bytes[15]);
			data.dig_P6 = ((report.Bytes[18] << 8) | report.Bytes[17]);
			data.dig_P7 = ((report.Bytes[20] << 8) | report.Bytes[19]);
			data.dig_P8 = ((report.Bytes[22] << 8) | report.Bytes[21]);
			data.dig_P9 = ((report.Bytes[24] << 8) | report.Bytes[23]);
			data.dig_H1 = report.Bytes[25];
		}
		else
			std::cout << __FUNCTION__ << ": IowKitWrite() error" << std::endl;
	}
	else
		std::cout << __FUNCTION__ << ": IowKitRead() error" << std::endl;

	memset(&report, 0x00, IOWKIT28_SPECIAL_REPORT_SIZE);
	report.ReportID = 0x02;
	report.Bytes[0] = 0xC2;
	report.Bytes[1] = I2C_ADR;
	report.Bytes[2] = REG__CALIB26;

	if (IowKitWrite(handle, IOW_PIPE_I2C_MODE, (char*) &report, IOWKIT28_SPECIAL_REPORT_SIZE) == IOWKIT28_SPECIAL_REPORT_SIZE)
	{
		if (IowKitRead(handle, IOW_PIPE_I2C_MODE, (char*) &report, IOWKIT28_SPECIAL_REPORT_SIZE) == IOWKIT28_SPECIAL_REPORT_SIZE)
		{
			// Rückantwort auswerten, Fehlerbits, etc.
		}
		else
			std::cout << __FUNCTION__ << ": IowKitRead() error" << std::endl;
	}
	else
		std::cout << __FUNCTION__ << ": IowKitWrite() error" << std::endl;

	// Die Daten auslesen
	memset(&report, 0x00, IOWKIT28_SPECIAL_REPORT_SIZE);
	report.ReportID = 0x03;					   // Write to I2C-device
	report.Bytes[0] = 8;
	report.Bytes[1] = I2C_ADR | 0x01; // I2C-address fürs Lesen einstellen
	report.Bytes[2] = 0x00;

	if (IowKitWrite(handle, IOW_PIPE_I2C_MODE, (char*) &report, IOWKIT28_SPECIAL_REPORT_SIZE) == IOWKIT28_SPECIAL_REPORT_SIZE)
	{
		if (IowKitRead(handle, IOW_PIPE_I2C_MODE, (char*) &report, IOWKIT28_SPECIAL_REPORT_SIZE) == IOWKIT28_SPECIAL_REPORT_SIZE)
		{
			data.dig_H2 = ((report.Bytes[2] << 8) | report.Bytes[1]);
			data.dig_H3 = (report.Bytes[3]);
			data.dig_H4 = ((report.Bytes[4] << 4) | (report.Bytes[5] & 0x0F));
			data.dig_H5 = ((report.Bytes[6] << 4) | ((report.Bytes[5] & 0xF0) >> 4));
			data.dig_H6 = report.Bytes[7];
		}
		else
			std::cout << __FUNCTION__ << ": IowKitRead() error" << std::endl;
	}
	else
		std::cout << __FUNCTION__ << ": IowKitWrite() error" << std::endl;

	return data;
}

SensorData ReadData(IOWKIT_HANDLE handle)
{
	SensorData data;
	IOWKIT28_SPECIAL_REPORT report;

	memset(&report, 0x00, IOWKIT28_SPECIAL_REPORT_SIZE);
	report.ReportID = 0x02;			// Write to I2C-device
	report.Bytes[0] = 0xC2;			// Start + Stop + 2 byte
	report.Bytes[1] = I2C_ADR;			// I2C-address
	report.Bytes[2] = REG__PRESS_MSB; // Set Start register to read from

	if (IowKitWrite(handle, IOW_PIPE_I2C_MODE, (char*) &report, IOWKIT28_SPECIAL_REPORT_SIZE) == IOWKIT28_SPECIAL_REPORT_SIZE)
	{
		if (IowKitRead(handle, IOW_PIPE_I2C_MODE, (char*) &report, IOWKIT28_SPECIAL_REPORT_SIZE) == IOWKIT28_SPECIAL_REPORT_SIZE)
		{
			// Answere from I2C device (ACK, errors, etc)
			// report[0] -> ReportID
			// report[1] -> flags
			// report[2] -> code
		}
		else
			std::cout << __FUNCTION__ << ": IowKitRead() error" << std::endl;
	}
	else
		std::cout << __FUNCTION__ << ": IowKitWrite() error" << std::endl;

	// Read Data from register postion set before
	memset(&report, 0x00, IOWKIT28_SPECIAL_REPORT_SIZE);
	report.ReportID = 0x03;					   // Read from I2C-device
	report.Bytes[0] = 9;
	report.Bytes[1] = I2C_ADR | 0x01; // I2C-address for read
	report.Bytes[2] = 0x00;

	if (IowKitWrite(handle, IOW_PIPE_I2C_MODE, (char*) &report, IOWKIT28_SPECIAL_REPORT_SIZE) == IOWKIT28_SPECIAL_REPORT_SIZE)
	{
		if (IowKitRead(handle, IOW_PIPE_I2C_MODE, (char*) &report, IOWKIT28_SPECIAL_REPORT_SIZE) == IOWKIT28_SPECIAL_REPORT_SIZE)
		{
			if ((report.Bytes[0] & 0x80) == 0x80)
			{
				// Error, set if slave does not ACK command byte
				std::cout << __FUNCTION__ << ": Error on I2C, no ACK byte from device received" << std::endl;
			}

			data.pressure = (report.Bytes[1] << 12) | (report.Bytes[2] << 4) | ((report.Bytes[3] & 0xF0) >> 4);
			data.temperatur = (report.Bytes[4] << 12) | (report.Bytes[5] << 4) | ((report.Bytes[6] & 0xF0) >> 4);
			data.humidity = (report.Bytes[7] << 8) | (report.Bytes[8]);
		}
		else
			std::cout << __FUNCTION__ << ": IowKitRead() error" << std::endl;
	}
	else
		std::cout << __FUNCTION__ << ": IowKitWrite() error" << std::endl;

	return data;
}

int main(int argc, char **argv)
{
	IOWKIT_HANDLE handle = NULL;
	IOWKIT28_SPECIAL_REPORT report;

	handle = IowKitOpenDevice();

	if (handle == NULL)
	{
		std::cout << "No IO-Warrior found" << std::endl;
		return -1;
	}

	if (IowKitGetNumDevs() > 1)
	{
		std::cout << "Please connect only one IO-Warrior device at the time for this example" << std::endl;
		IowKitCloseDevice(handle);
		return -1;
	}

	if (IowKitGetProductId(handle) != IOWKIT_PID_IOW28)
	{
		std::cout << "This example is build for IO-Warrior28. Sourcecode for other IO-Warrior looks similar, but use different 'REPORT_SIZES'." << std::endl;
		IowKitCloseDevice(handle);
		return -1;
	}

	IowKitSetTimeout(handle, 1000);

	memset(&report, 0x00, IOWKIT28_SPECIAL_REPORT_SIZE);
	report.ReportID = 0x01; // I2C Mode
	report.Bytes[0] = 0x01; // Enable
	report.Bytes[1] = 0x00;

	if (IowKitWrite(handle, IOW_PIPE_I2C_MODE, (char *)&report, IOWKIT28_SPECIAL_REPORT_SIZE) != IOWKIT28_SPECIAL_REPORT_SIZE)
	{
		std::cout << "Can not Init the IO-Warrior, IowKitWrite() error" << std::endl;
		IowKitCloseDevice(handle);
		return -1;
	}

	// BMP280 initialize
	report.ReportID = 0x02;			 // Write to I2C-device
	report.Bytes[0] = 0xC3;			 // Start + Stop + 2 byte
	report.Bytes[1] = I2C_ADR;		 // I2C-address
	report.Bytes[2] = REG__CTR_MEAS; // Register -> CTRL_MEAS

	// osrs_t = b001
	// osrs_p = b001
	// mode = b11
	//============
	// value = b00100111 -> 0x27
	report.Bytes[3] = 0x27; // datasheet page 29 to enable Measurement

	if (IowKitWrite(handle, IOW_PIPE_I2C_MODE, (char *)&report, IOWKIT28_SPECIAL_REPORT_SIZE) == IOWKIT28_SPECIAL_REPORT_SIZE)
	{
		if (IowKitRead(handle, IOW_PIPE_I2C_MODE, (char *)&report, IOWKIT28_SPECIAL_REPORT_SIZE) == IOWKIT28_SPECIAL_REPORT_SIZE)
		{
			// Check return bits for i2c errors
		}
		else
			std::cout << __FUNCTION__ << ": IowKitRead() error" << std::endl;
	}
	else
		std::cout << __FUNCTION__ << ": IowKitWrite() error" << std::endl;


	//CompensationData compensation;
	SensorData sensor;
	TemperaturData temps;
	m_Compensation = ReadCalibration(handle);

	for(int i=0; i<20; i++)
	{
		sensor = ReadData(handle);

		temps /*BME280_S32_t t*/ = BME280_compensate_T_int32(sensor.temperatur);
		BME280_U32_t p = BME280_compensate_P_int64(sensor.pressure, temps.tfine);

		double temp = (double)temps.temperature / 100.0f;
		double press = ((double)p / 256.0f) / 100.0f;

		std::cout << std::setprecision(4) << "Temperature: " << temp << " °C  |  Pressure: " << press << " hPa" << std::endl;
		std::this_thread::sleep_for(std::chrono::milliseconds(250));
	}




	// BMP280 sleep
	report.ReportID = 0x02;			 // Write to I2C-device
	report.Bytes[0] = 0xC3;			 // Start + Stop + 2 byte
	report.Bytes[1] = I2C_ADR;		 // I2C-address
	report.Bytes[2] = REG__CTR_MEAS; // Register -> CTRL_MEAS
	report.Bytes[3] = 0x00; // datasheet page 29 to enable Measurement

	if (IowKitWrite(handle, IOW_PIPE_I2C_MODE, (char *)&report, IOWKIT28_SPECIAL_REPORT_SIZE) == IOWKIT28_SPECIAL_REPORT_SIZE)
	{
		if (IowKitRead(handle, IOW_PIPE_I2C_MODE, (char *)&report, IOWKIT28_SPECIAL_REPORT_SIZE) == IOWKIT28_SPECIAL_REPORT_SIZE)
		{
			// Check return bits for i2c errors
		}
		else
			std::cout << __FUNCTION__ << ": IowKitRead() error" << std::endl;
	}
	else
		std::cout << __FUNCTION__ << ": IowKitWrite() error" << std::endl;


	IowKitCloseDevice(handle);

	return 0;
}