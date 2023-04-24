# Patch for IO-Warrior100 kernel module
There is a bug in the kernel module for the IO-Warrior100 IO report size. This bug make writing to the IO-Interface impossible.
The included **iowarrior.c** includes the fix for this bug. You have to replace the iowarrior.c in the kernel sources and compile the iowarrior module.  

The prefered solution: download the kernel sources and fix the bug for a specific kernel version by yourself for all latest in the iowarrior.c between the different versions and distributions.  
  
The *iowarrior.c* will be find in the kernel sources in **/drivers/usb/misc/**


## Fixing code
The bug will be found near line 817 in iowarrior.c.  


```
/*
     * Some devices need the report size to be different than the
     * endpoint size.
     */
    if (dev->interface->cur_altsetting->desc.bInterfaceNumber == 0) {
        switch (dev->product_id) {
        case USB_DEVICE_ID_CODEMERCS_IOW56:
        case USB_DEVICE_ID_CODEMERCS_IOW56AM:
            dev->report_size = 7;
            break;

        case USB_DEVICE_ID_CODEMERCS_IOW28:
        case USB_DEVICE_ID_CODEMERCS_IOW28L:
            dev->report_size = 4;
            break;

        case USB_DEVICE_ID_CODEMERCS_IOW100:
BUG->       dev->report_size = 13;
            break;
        }
    }
```

You must replace the following line
```
dev->report_size = 13;
```
with 
```
dev->report_size = 12;
```

After replacing the code line you have to compile the module und replace the **iowarrior.ko**.






