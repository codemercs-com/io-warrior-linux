#!/usr/bin/env python3.3
import os, sys, atexit, ctypes
import ctypes, ctypes.util

# Product IDs of IO-Warrior family
IOWKIT_PRODUCT_ID_IOW40 = 0x1500
IOWKIT_PRODUCT_ID_IOW24 = 0x1501
IOWKIT_PRODUCT_ID_IOW56 = 0x1503
IOWKIT_PRODUCT_ID_IOW28 = 0x1504
IOWKIT_PRODUCT_ID_IOW100 = 0x1506

# Pipe count (iow24/40/56 supports only 2 pipes)
IOWKIT_MAX_PIPES = 4

# Pipe names
IOW_PIPE_IO_PINS = ctypes.c_ulong(0)
IOW_PIPE_SPECIAL_MODE = ctypes.c_ulong(1)
IOW_PIPE_I2C_MODE = ctypes.c_ulong(2)	    #only IOW28 / IOW100
IOW_PIPE_ADC_MODE = ctypes.c_ulong(3)	    #only IOW28 / IOW100

# Max IO-Warrior supported
IOWKIT_MAX_DEVICES = ctypes.c_ulong(16)

# Length of serial number string
SERIAL_NUMBER_BUFFER = 8

# Size of array for write/read
IOWKIT_IO_REPORT_SIZE = 5       # !deprecated
IOWKIT40_IO_REPORT_SIZE = 5
IOWKIT24_IO_REPORT_SIZE = 3
IOWKIT56_IO_REPORT_SIZE = 8
IOWKIT28_IO_REPORT_SIZE = 5
IOWKIT100_IO_REPORT_SIZE = 13

# IO-Report arrays (ReportID + data)
IOWKIT_IO_REPORT = ctypes.c_ubyte * IOWKIT_IO_REPORT_SIZE       # !deprecated
IOWKIT40_IO_REPORT = ctypes.c_ubyte * IOWKIT40_IO_REPORT_SIZE
IOWKIT24_IO_REPORT = ctypes.c_ubyte * IOWKIT24_IO_REPORT_SIZE
IOWKIT56_IO_REPORT = ctypes.c_ubyte * IOWKIT56_IO_REPORT_SIZE
IOWKIT28_IO_REPORT = ctypes.c_ubyte * IOWKIT28_IO_REPORT_SIZE
IOWKIT100_IO_REPORT = ctypes.c_ubyte * IOWKIT100_IO_REPORT_SIZE

# Size of array for write/read
IOWKIT_SPECIAL_REPORT_SIZE = 8      # !deprecated
IOWKIT40_SPECIAL_REPORT_SIZE = 8
IOWKIT24_SPECIAL_REPORT_SIZE = 8
IOWKIT56_SPECIAL_REPORT_SIZE = 64
IOWKIT28_SPECIAL_REPORT_SIZE = 64
IOWKIT100_SPECIAL_REPORT_SIZE = 64

# Special-Report arrays (ReportID + data)
IOWKIT_SPECIAL_REPORT = ctypes.c_ubyte * IOWKIT_SPECIAL_REPORT_SIZE     # !deprecated
IOWKIT40_SPECIAL_REPORT = ctypes.c_ubyte * IOWKIT40_SPECIAL_REPORT_SIZE
IOWKIT24_SPECIAL_REPORT = ctypes.c_ubyte * IOWKIT24_SPECIAL_REPORT_SIZE
IOWKIT56_SPECIAL_REPORT = ctypes.c_ubyte * IOWKIT56_SPECIAL_REPORT_SIZE
IOWKIT28_SPECIAL_REPORT = ctypes.c_ubyte * IOWKIT28_SPECIAL_REPORT_SIZE
IOWKIT100_SPECIAL_REPORT = ctypes.c_ubyte * IOWKIT100_SPECIAL_REPORT_SIZE



# Loading lib
if sys.platform == 'linux2':
    iowkit = ctypes.CDLL("libiowkit.so")
if sys.platform.startswith('win'):
    iowkit = ctypes.WinDLL("iowkit")
else:
    NotImplementedError("loading the iowkit library not implemented yet")

# Import IowKit functions

# Open IO-Warrior
IowKitOpenDevice = iowkit.IowKitOpenDevice
IowKitOpenDevice.argtypes = []
IowKitOpenDevice.restype = ctypes.c_voidp

# Close IO-Warrior
IowKitCloseDevice = iowkit.IowKitCloseDevice
IowKitCloseDevice.argtypes = [ctypes.c_voidp]
IowKitCloseDevice.restype = None

# Get handle of specific IO-Warrior by number
IowKitGetDeviceHandle = iowkit.IowKitGetDeviceHandle
IowKitGetDeviceHandle.argtypes = [ctypes.c_ulong]
IowKitGetDeviceHandle.restype = ctypes.c_voidp

# Version of lib
IowKitVersion = iowkit.IowKitVersion
iowkit.IowKitVersion.argtypes = []
iowkit.IowKitVersion.restype = ctypes.c_char_p

# Get Product ID
IowKitGetProductId = iowkit.IowKitGetProductId
IowKitGetProductId.argtypes = [ctypes.c_voidp]
IowKitGetProductId.restype = ctypes.c_ulong

# Get serial number
IowKitGetSerialNumber = iowkit.IowKitGetSerialNumber
IowKitGetSerialNumber.argtypes = [ctypes.c_voidp, ctypes.POINTER(ctypes.c_ushort)]
IowKitGetSerialNumber.restype = ctypes.c_bool

# Get Revision
IowKitGetRevision = iowkit.IowKitGetRevision
IowKitGetRevision.argtypes = [ctypes.c_voidp]
IowKitGetRevision.restype = ctypes.c_ulong

# Get number of connected IO-Warrior
IowKitGetNumDevs = iowkit.IowKitGetNumDevs
IowKitGetNumDevs.argtypes = []
IowKitGetNumDevs.restype = ctypes.c_ulong

# Set read timeout (by default: INFINITE)
IowKitSetTimeout = iowkit.IowKitSetTimeout
IowKitSetTimeout.argtypes = [ctypes.c_voidp, ctypes.c_ulong]
IowKitSetTimeout.restype = None

# Set write timeout (by default: INFINITE)
IowKitSetWriteTimeout = iowkit.IowKitSetWriteTimeout
IowKitSetWriteTimeout.argtypes = [ctypes.c_voidp, ctypes.c_ulong]
IowKitSetWriteTimeout.restype = None

# Write to IO-Warrior
IowKitWrite = iowkit.IowKitWrite
IowKitWrite.argtypes = [ctypes.c_voidp, ctypes.c_ulong, ctypes.c_voidp, ctypes.c_ulong]
IowKitWrite.restype = ctypes.c_ulong

# Read from IO-Warrior
IowKitRead = iowkit.IowKitRead
IowKitRead.argtypes = [ctypes.c_voidp, ctypes.c_ulong, ctypes.c_voidp, ctypes.c_ulong]
IowKitRead.restype = ctypes.c_ulong

# Read none blocking from IO-Warrior
IowKitReadNonBlocking = iowkit.IowKitReadNonBlocking
IowKitReadNonBlocking.argtypes = [ctypes.c_voidp, ctypes.c_ulong, ctypes.c_voidp, ctypes.c_ulong]
IowKitReadNonBlocking.restype = ctypes.c_ulong

# Read IO-Ports from IO-Warrior (!deprecated, not working for IO-Warrior56, IO-Warrior28 and newer)
IowKitReadImmediate = iowkit.IowKitReadImmediate
IowKitReadImmediate.argtypes = [ctypes.c_voidp, ctypes.POINTER(ctypes.c_ulong)]
IowKitReadImmediate.restype = ctypes.c_bool

# Cancel IO on USB
IowKitCancelIo = iowkit.IowKitCancelIo
IowKitCancelIo.argtypes = [ctypes.c_voidp, ctypes.c_ulong]
IowKitCancelIo.restype = ctypes.c_bool

### END DEFINES AND WRAPPER


# Get Serialnumber of IO-Warrior as useable STRING
def _IowKitGetSerialNumber(handle):
    ArrayType = ctypes.c_ushort * SERIAL_NUMBER_BUFFER
    array = ArrayType()
    IowKitGetSerialNumber(handle, ctypes.cast(array, ctypes.POINTER(ctypes.c_ushort)))

    return ''.join(chr(e) for e in array)

# Write the IO ports
def WriteIO(handle):
    write = IOWKIT100_IO_REPORT(0x00, 0x02, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF)
    ret = IowKitWrite(handle, IOW_PIPE_IO_PINS, ctypes.byref(write), ctypes.sizeof(IOWKIT100_IO_REPORT))

    if(ret != IOWKIT100_IO_REPORT_SIZE):
        print("Error on write data to IO-Warrior100")

    return 0

# Read the IO ports
def ReadIO(handle):
    read = IOWKIT100_IO_REPORT(0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00) #init report struct with 0
    ret = IowKitRead(handle, IOW_PIPE_IO_PINS, ctypes.byref(read), ctypes.sizeof(IOWKIT100_IO_REPORT))
    if(ret != IOWKIT100_IO_REPORT_SIZE):
        print("Error on read data from IO-Warrior100")
    else:
        print("Read Report: ", ' '.join(hex(e)[2:].zfill(2) for e in read))

    return 0

# Read the IO ports by using the spefial funtion "Getting current pin status" 0xFF (datahsset 5.9.10)
def ReadIOSpecial(handle):
    read_speacial = IOWKIT100_SPECIAL_REPORT(0xFF) #Set reportID (first item) to 0xFF

    if(IowKitWrite(handle, IOW_PIPE_SPECIAL_MODE, ctypes.byref(read_speacial), ctypes.sizeof(IOWKIT100_SPECIAL_REPORT)) == IOWKIT100_SPECIAL_REPORT_SIZE):
        if(IowKitRead(handle, IOW_PIPE_SPECIAL_MODE, ctypes.byref(read_speacial), ctypes.sizeof(IOWKIT100_SPECIAL_REPORT)) == IOWKIT100_SPECIAL_REPORT_SIZE):
            print("Read Report: ", ' '.join(hex(e)[2:].zfill(2) for e in read_speacial))
        else:
            print("Error on read data from IO-Warrior100 (special report)")
    else:
        print("Error on write data to IO-Warrior100 (special report)")

    return 0



# Main program #
print("DLL Version: ", IowKitVersion())
print("\n")

# Start communication with IO-Warrior
handle = IowKitOpenDevice() # 1st function to call to opwn the iowkit API

if handle:
    print("--- Get device informations ---")

    num = IowKitGetNumDevs()
    print("Number of connected IO-Warrior: ", num)

    if(num == 1):
        pid = IowKitGetProductId(handle)

        if(pid == IOWKIT_PRODUCT_ID_IOW24):
            print("Device: IO-Warrior24")
        if(pid == IOWKIT_PRODUCT_ID_IOW40):
            print("Device: IO-Warrior40")
        if(pid == IOWKIT_PRODUCT_ID_IOW56):
            print("Device: IO-Warrior56")
        if(pid == IOWKIT_PRODUCT_ID_IOW28):
            print("Device: IO-Warrior28")
        if(pid == IOWKIT_PRODUCT_ID_IOW100):
            print("Device: IO-Warrior100")

        print("Revision: ", hex(IowKitGetRevision(handle)))
        print("Serial: ", _IowKitGetSerialNumber(handle))

    else:
        print("More than one IO-Warrior connected. This example supports only one IO-Warrior at a time.")


    #Write IO ports, first item: ReportID
    WriteIO(handle)

    ##Read IO ports from IO-Warrior100. IowKitRead returns only if new data on the ports are available. If not it waits forever
    ReadIO(handle)

    #To get the actual port status without blocking use the special function '0xFF' (datasheet 5.9.10).
    ReadIOSpecial(handle)


else:
    print("Some issues with the API or not IO-Warrior detected")

IowKitCloseDevice(handle)   # Never forget to close @ the end!