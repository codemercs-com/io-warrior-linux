/*
 * Sample to use the digital LED function of IO-Warrior100
 */

#include <iostream>
#include <string.h>
#include <stdio.h>
#include <chrono>
#include <thread>

#include "iowkit.h"
#include "CIow28DigiLedClass.h"

int main(int argc, char *argv[])
{
    IOWKIT_HANDLE handle;   // Device handle
    CIow28DigiLedClass led; // Class for using the DAC mode
    uint8_t error = IOW28_DIGILED::ERROR_NONE;

    handle = IowKitOpenDevice(); // Open iowkit and return 1st. IO-Warrior found

    if (handle == NULL)
    {
        std::cout << "Fail to open device" << std::endl;
        return 0;
    }

    DWORD pid = IowKitGetProductId(handle); // Get product ID

    if (pid != IOWKIT_PID_IOW28)
    {
        std::cout << "No IO-Warrior28 found" << std::endl;
        IowKitCloseDevice(handle);
        return 1;
    }

    error = led.SetHandle(handle);              // Set IOWKIT_HANDLE to class
    error = led.Enable(0x3B, 0x0E, 0x3B, 0x1F); // Set LED-Stripe setup, values can be found in the data sheet
    if (error != IOW28_DIGILED::ERROR_NONE)
    {
        std::cout << "Can not ennable digiLED (write failed)" << std::endl;
        IowKitCloseDevice(handle);
        return 1;
    }

    led.SetPixelCount(64);                     // Set number of LEDs
    led.SetPixelType(IOW28_DIGILED::TYPE_GRB); // Set Color order, for WS2812 = GRB

    // Set first pixel to specific color
    std::cout << "Set 1st. LED to red and wait for 2 sec" << std::endl;
    led.SetPixel(0, 255, 0, 0); // Set the first
    led.Write();

    std::this_thread::sleep_for(std::chrono::milliseconds(2000));

    // Try the ROTATE effect for demo
    std::cout << "Rotate LEDs 100x to the right" << std::endl;
    for (int i = 0; i < 100; i++)
    {
        led.RotatePixelsRight();
        led.Write();
        std::this_thread::sleep_for(std::chrono::milliseconds(25));
    }

    // Try the SHIFT effect for demo
    std::cout << "Shift LEDs 100x to the left" << std::endl;
    srand((unsigned)time(NULL));
    CLed reservoir[5] = {CLed(0, 0, 0), CLed(255, 0, 0), CLed(0, 255, 0), CLed(0, 0, 255), CLed(255, 0, 255)};
    uint16_t num = 0;

    for (int i = 0; i < 100; i++)
    {
        num = rand() % 5;

        led.ShiftPixelsLeft(1, reservoir[num]);
        led.Write();
        std::this_thread::sleep_for(std::chrono::milliseconds(25));
    }

    std::cout << "Done! Wait for 2 sec and clear" << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(2000));

    led.Clear();
    led.Write();

    led.Disable(); // Disable digiLED mode and free IO-Pin.

    IowKitCloseDevice(handle); // Close iowkit at the end to free device for other tools
    return 0;
}