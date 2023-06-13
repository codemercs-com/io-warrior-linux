#include <string.h>
#include <math.h>

#include "iowkit.h"
#include "CIow28DigiLedClass.h"

CIow28DigiLedClass::CIow28DigiLedClass()
{
	Init();
	m_PixelCount = 0;
}

CIow28DigiLedClass::CIow28DigiLedClass(uint16_t num)
{
	Init();

	if ((num <= IOW28_DIGILED::PIXEL_MAX) && (num > 0))
		m_PixelCount = num;
	else
		m_PixelCount = 0;
}

CIow28DigiLedClass::~CIow28DigiLedClass()
{
}

uint16_t CIow28DigiLedClass::SetHandle(IOWKIT_HANDLE handle)
{
	ULONG pid = IowKitGetProductId(handle);

	if (pid == IOWKIT_PID_IOW28)
	{
		m_Handle = handle;
		return IOW28_DIGILED::ERROR_NONE;
	}
	else
	{
		m_Handle = NULL;
		return IOW28_DIGILED::ERROR_DEVICE;
	}
}

uint16_t CIow28DigiLedClass::Enable(uint8_t cycle_zero, uint8_t zero, uint8_t cycle_one, uint8_t one)
{
	IOWKIT28_SPECIAL_REPORT report;
	memset(&report, 0x00, IOWKIT28_SPECIAL_REPORT_SIZE);
	report.ReportID = IOW28_DIGILED::RID_MODE;
	report.Bytes[0] = IOW28_DIGILED::ENABLE;
	report.Bytes[1] = cycle_zero;
	report.Bytes[2] = zero;
	report.Bytes[3] = cycle_one;
	report.Bytes[4] = one;

	// Store in Class
	m_Cycle_Zero = cycle_zero;
	m_Zero = zero;
	m_Cycle_One = cycle_one;
	m_One = one;

	if (m_Handle != NULL)
	{
		if (IowKitWrite(m_Handle, IOW_PIPE_SPECIAL_MODE, (char *)&report, IOWKIT28_SPECIAL_REPORT_SIZE) == IOWKIT28_SPECIAL_REPORT_SIZE)
			return IOW28_DIGILED::ERROR_NONE;
		else
			return IOW28_DIGILED::ERROR_WRITE;
	}
	else
		return IOW28_DIGILED::ERROR_HANDLE;

	return IOW28_DIGILED::ERROR_UNKNOWN;
}

uint16_t CIow28DigiLedClass::Disable()
{
	IOWKIT28_SPECIAL_REPORT report;
	memset(&report, 0x00, IOWKIT28_SPECIAL_REPORT_SIZE);
	report.ReportID = IOW28_DIGILED::RID_MODE;
	report.Bytes[0] = IOW28_DIGILED::DISABLE;

	if (m_Handle != NULL)
	{
		if (IowKitWrite(m_Handle, IOW_PIPE_SPECIAL_MODE, (char *)&report, IOWKIT28_SPECIAL_REPORT_SIZE) == IOWKIT28_SPECIAL_REPORT_SIZE)
			return IOW28_DIGILED::ERROR_NONE;
		else
			return IOW28_DIGILED::ERROR_WRITE;
	}
	else
		return IOW28_DIGILED::ERROR_HANDLE;

	return IOW28_DIGILED::ERROR_UNKNOWN;
}

uint16_t CIow28DigiLedClass::SetPixelCount(uint16_t num)
{
	if ((num <= IOW28_DIGILED::PIXEL_MAX) && (num > 0))
	{
		m_PixelCount = num;
		return IOW28_DIGILED::ERROR_NONE;
	}
	else
		return IOW28_DIGILED::ERROR_RANGE;
}

uint16_t CIow28DigiLedClass::GetPixelCount()
{
	return m_PixelCount;
}

void CIow28DigiLedClass::SetPixelType(uint8_t num)
{
	if (num == IOW28_DIGILED::TYPE_RGB)
		m_ColorType = IOW28_DIGILED::TYPE_RGB;
	else
		m_ColorType = IOW28_DIGILED::TYPE_GRB;
}

uint16_t CIow28DigiLedClass::SetPixel(uint16_t num, CLed pixel)
{
	if ((num < m_PixelCount) && (num >= 0))
	{
		switch (m_ColorType)
		{
		case IOW28_DIGILED::TYPE_RGB:
			m_Pixel[num].SetColor(pixel.Red(), pixel.Green(), pixel.Blue());
			break;
		case IOW28_DIGILED::TYPE_GRB:
		default:
			m_Pixel[num].SetColor(pixel.Green(), pixel.Red(), pixel.Blue());
			break;
		}

		return IOW28_DIGILED::ERROR_NONE;
	}
	else
		return IOW28_DIGILED::ERROR_RANGE;
}

uint16_t CIow28DigiLedClass::SetPixel(uint16_t num, uint8_t r, uint8_t g, uint8_t b)
{
	if ((num < m_PixelCount) && (num >= 0))
	{
		switch (m_ColorType)
		{
		case IOW28_DIGILED::TYPE_RGB:
			m_Pixel[num].SetColor(r, g, b);
			break;
		case IOW28_DIGILED::TYPE_GRB:
		default:
			m_Pixel[num].SetColor(g, r, b);
			break;
		}
		return IOW28_DIGILED::ERROR_NONE;
	}
	else
		return IOW28_DIGILED::ERROR_RANGE;
}

CLed CIow28DigiLedClass::GetPixel(uint16_t num)
{
	if ((num < m_PixelCount) && (num >= 0))
		return m_Pixel[num];
	else
		return CLed(0, 0, 0);
}

void CIow28DigiLedClass::Clear()
{
	for (int i = 0; i < IOW28_DIGILED::PIXEL_MAX; i++)
		m_Pixel[i].SetColor(0, 0, 0);
}

uint16_t CIow28DigiLedClass::Write()
{
	IOWKIT28_SPECIAL_REPORT report;
	uint16_t error = IOW28_DIGILED::ERROR_NONE;

	if (m_Handle != NULL)
	{
		int32_t repCount = (int)ceil((double)m_PixelCount / (double)IOW28_DIGILED::PIXEL_BLOCK);
		int32_t ledCount = m_PixelCount;
		int32_t ledSend = 0;
		int32_t offset = 0;
		int32_t pixel = 0;

		for (int i = 0; i < repCount; i++)
		{
			if (ledCount >= IOW28_DIGILED::PIXEL_BLOCK)
			{
				ledSend = IOW28_DIGILED::PIXEL_BLOCK;
				ledCount -= IOW28_DIGILED::PIXEL_BLOCK;
			}
			else
				ledSend = ledCount;

			memset(&report, 0x00, IOWKIT28_SPECIAL_REPORT_SIZE);
			report.ReportID = IOW28_DIGILED::RID_WRITE;
			report.Bytes[0] = IOW28_DIGILED::WRITE_BUFFER | i; // Write data into buffer
			report.Bytes[1] = 0x00;							   // ignored if write into buffer
			report.Bytes[2] = 0x00;							   // ignored if write into buffer

			offset = 0;

			for (int j = 0; j < ledSend; j++)
			{
				offset += 3;
				report.Bytes[offset + 0] = m_Pixel[pixel].Red();
				report.Bytes[offset + 1] = m_Pixel[pixel].Green();
				report.Bytes[offset + 2] = m_Pixel[pixel].Blue();

				pixel++;
			}

			if (IowKitWrite(m_Handle, IOW_PIPE_SPECIAL_MODE, (char *)&report, IOWKIT28_SPECIAL_REPORT_SIZE) != IOWKIT28_SPECIAL_REPORT_SIZE)
			{
				error = IOW28_DIGILED::ERROR_WRITE;
				break;
			}
		}

		// Write data from iow28 buffer
		report.ReportID = IOW28_DIGILED::RID_WRITE;
		report.Bytes[0] = IOW28_DIGILED::WRITE_OUTPUT | repCount; // IOW28_DIGILED::BLOCK_COUNT;
		report.Bytes[1] = m_PixelCount & 0x00FF;				  // num of LEDs LSB
		report.Bytes[2] = (m_PixelCount & 0xFF00) >> 8;			  // num of LEDs MSB

		if (IowKitWrite(m_Handle, IOW_PIPE_SPECIAL_MODE, (char *)&report, IOWKIT28_SPECIAL_REPORT_SIZE) != IOWKIT28_SPECIAL_REPORT_SIZE)
			error = IOW28_DIGILED::ERROR_WRITE;
	}
	else
		error = IOW28_DIGILED::ERROR_HANDLE;

	return error;
}

void CIow28DigiLedClass::Init()
{
	m_Handle = NULL;
	m_PixelCount = 0;
	m_ColorType = IOW28_DIGILED::TYPE_GRB;

	// Set WS2812B timing as default values
	m_Cycle_Zero = 59;
	m_Cycle_One = 59;
	m_Zero = 19;
	m_One = 39;
}

void CIow28DigiLedClass::RotatePixelsLeft()
{
	CLed pixel(0, 0, 0);
	pixel = m_Pixel[0];

	for (int i = 0; i < (m_PixelCount - 1); i++)
		m_Pixel[i] = m_Pixel[i + 1];

	m_Pixel[m_PixelCount - 1] = pixel;
}

void CIow28DigiLedClass::RotatePixelsRight()
{
	CLed pixel(0, 0, 0);
	pixel = m_Pixel[m_PixelCount - 1];

	for (int i = (m_PixelCount - 1); i >= 0; i--)
		m_Pixel[i + 1] = m_Pixel[i];

	m_Pixel[0] = pixel;
}

uint16_t CIow28DigiLedClass::ShiftPixelsLeft(uint16_t steps, CLed fill)
{
	if ((steps > m_PixelCount) || (steps < 1))
		return IOW28_DIGILED::ERROR_RANGE;
	else
	{
		for (int j = 0; j < steps; j++)
		{
			for (int i = 0; i < (m_PixelCount - 1); i++)
				m_Pixel[i] = m_Pixel[i + 1];

			m_Pixel[m_PixelCount - 1] = fill;
		}
	}
	return IOW28_DIGILED::ERROR_NONE;
}

uint16_t CIow28DigiLedClass::ShiftPixelsRight(uint16_t steps, CLed fill)
{
	if ((steps > m_PixelCount) || (steps < 1))
		return IOW28_DIGILED::ERROR_RANGE;
	else
	{
		for (int j = 0; j < steps; j++)
		{
			for (int i = (m_PixelCount - 1); i >= 0; i--)
				m_Pixel[i + 1] = m_Pixel[i];

			m_Pixel[0] = fill;
		}
	}
	return IOW28_DIGILED::ERROR_NONE;
}