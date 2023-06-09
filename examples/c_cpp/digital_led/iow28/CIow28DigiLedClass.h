/*
 * Class for easy use of the ADC function of the IO-Warrior100.
 * This class contains only the single shot functionality.
 */

#include <stdint.h>
#include "iowkit.h"

namespace IOW28_DIGILED
{
    // ReportIDs
    const uint8_t RID_MODE = 0x28;
    const uint8_t RID_WRITE = 0x29;

    // Bitmask for write data
    const uint8_t WRITE_BUFFER = 0x00;
    const uint8_t WRITE_OUTPUT = 0x80;

    // Enable/Disable
    const uint8_t DISABLE = 0x00;
    const uint8_t ENABLE = 0x01;

    // Max values for digiLED mode
    const uint16_t PIXEL_MAX = 500; // Max LED count for IOW28
    const uint8_t BLOCK_COUNT = 24; // 1 BLOCK = 20 LEDs
    const uint8_t PIXEL_BLOCK = 20; // RGB-LED count per Block

    // Color mode for the LEDs
    const uint8_t TYPE_GRB = 0x00; // Color order: Greed, Red, Blue   ->   default, most types
    const uint8_t TYPE_RGB = 0x01;

    // Return errors
    const uint16_t ERROR_NONE = 0x0000;
    const uint16_t ERROR_HANDLE = 0x0001;
    const uint16_t ERROR_WRITE = 0x0002;
    const uint16_t ERROR_READ = 0x0004;
    const uint16_t ERROR_DEVICE = 0x0008;
    const uint16_t ERROR_PORT = 0x0010;
    const uint16_t ERROR_RANGE = 0x0020;
    const uint16_t ERROR_UNKNOWN = 0x8000;
}

// Class for RGB-LEDs
class CLed
{
public:
    CLed()
    {
        m_Red = 0;
        m_Green = 0;
        m_Blue = 0;
    }
    CLed(uint8_t r, uint8_t g, uint8_t b)
    {
        m_Red = r;
        m_Green = g;
        m_Blue = b;
    }
    ~CLed(){};

    void Reset()
    {
        m_Red = 0;
        m_Green = 0;
        m_Blue = 0;
    }
    void SetColor(uint8_t r, uint8_t g, uint8_t b)
    {
        m_Red = r;
        m_Green = g;
        m_Blue = b;
    }
    uint8_t Red() { return m_Red; }
    uint8_t Green() { return m_Green; }
    uint8_t Blue() { return m_Blue; }

private:
    uint8_t m_Red;
    uint8_t m_Green;
    uint8_t m_Blue;
};

class CIow28DigiLedClass
{
public:
    CIow28DigiLedClass();
    CIow28DigiLedClass(uint16_t num);
    virtual ~CIow28DigiLedClass();

    uint16_t SetHandle(IOWKIT_HANDLE handle);                                          // Set IOWKIT_HANDLE
    uint16_t Enable(uint8_t cycle_zero, uint8_t zero, uint8_t cycle_one, uint8_t one); // Enbale DigiLED mode with timing
    uint16_t Disable();                                                                // Disable DigiLED mode

    uint16_t SetPixelCount(uint16_t num); // Set LED number (max 500 possible)
    uint16_t GetPixelCount();

    void SetPixelType(uint8_t num); // Define the color mode for the LED (pixel)

    uint16_t SetPixel(uint16_t num, CLed pixel);                      // Set pixel by CLed
    uint16_t SetPixel(uint16_t num, uint8_t r, uint8_t g, uint8_t b); // Set pixel by r,g,b values
    CLed GetPixel(uint16_t num);                                      // Get pixel as CLed

    void Clear();     // Clear all pixel with 0,0,0
    uint16_t Write(); // Write data

    // Some effect functions: Rotate m_Pixel array
    void RotatePixelsLeft();
    void RotatePixelsRight();

    // Some effect functions: Shift m_Pixel array
    uint16_t ShiftPixelsLeft(uint16_t steps, CLed fill);
    uint16_t ShiftPixelsRight(uint16_t steps, CLed fill);

private:
    IOWKIT_HANDLE m_Handle;
    uint8_t m_Cycle_Zero;
    uint8_t m_Cycle_One;
    uint8_t m_Zero;
    uint8_t m_One;
    uint8_t m_ColorType;
    uint16_t m_PixelCount;
    CLed m_Pixel[IOW28_DIGILED::PIXEL_MAX];

    void Init();
};
