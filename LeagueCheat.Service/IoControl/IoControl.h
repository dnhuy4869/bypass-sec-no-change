#pragma once

#include "../DriverEntry.h"
#include "../IoMethod/IoMethod.h"

#define IOCTL_READ_MEMORY CTL_CODE(FILE_DEVICE_UNKNOWN, 0x6991, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)
#define IOCTL_WRITE_MEMORY CTL_CODE(FILE_DEVICE_UNKNOWN, 0x6992, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)
#define IOCTL_WRITE_PROTECTED_MEMORY CTL_CODE(FILE_DEVICE_UNKNOWN, 0x6993, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)
#define IOCTL_BYPASS_SEC_NO_CHANGE CTL_CODE(FILE_DEVICE_UNKNOWN, 0x6994, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)
#define IOCTL_ENABLE_SEC_NO_CHANGE CTL_CODE(FILE_DEVICE_UNKNOWN, 0x6995, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)

NTSTATUS CreateCall(PDEVICE_OBJECT DeviceObject, PIRP Irp);

NTSTATUS CloseCall(PDEVICE_OBJECT DeviceObject, PIRP Irp);

NTSTATUS IoControl(PDEVICE_OBJECT DeviceObject, PIRP Irp);