#include "./DriverEntry.h"
#include "./IoMethod/IoMethod.h"
#include "./IoControl/IoControl.h"
#include "./VMProtectSDK/VMProtectDDK.h"

PDEVICE_OBJECT pDeviceObject;
UNICODE_STRING Dev, Dos;

NTSTATUS DriverEntry(PDRIVER_OBJECT pDriverObject, PUNICODE_STRING pRegistryPath);
NTSTATUS UnloadDriver(PDRIVER_OBJECT pDriverObject);

NTSTATUS DriverEntry(PDRIVER_OBJECT pDriverObject, PUNICODE_STRING pRegistryPath)
{
	VMProtectBeginMutation(__FUNCTION__);

	RtlInitUnicodeString(&Dev, VMProtectDecryptStringW(L"\\Device\\LeagueCheat.IO"));
	RtlInitUnicodeString(&Dos, VMProtectDecryptStringW(L"\\DosDevices\\LeagueCheat.IO"));

	IoCreateDevice(pDriverObject, 0, &Dev, FILE_DEVICE_UNKNOWN, FILE_DEVICE_SECURE_OPEN, FALSE, &pDeviceObject);
	IoCreateSymbolicLink(&Dos, &Dev);

	pDriverObject->MajorFunction[IRP_MJ_CREATE] = CreateCall;
	pDriverObject->MajorFunction[IRP_MJ_CLOSE] = CloseCall;
	pDriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = IoControl;
	pDriverObject->DriverUnload = UnloadDriver;

	pDeviceObject->Flags |= DO_DIRECT_IO;
	pDeviceObject->Flags &= ~DO_DEVICE_INITIALIZING;

	VMProtectEnd();
    return STATUS_SUCCESS;
}

NTSTATUS UnloadDriver(PDRIVER_OBJECT pDriverObject)
{
	IoDeleteSymbolicLink(&Dos);
	IoDeleteDevice(pDriverObject->DeviceObject);

	return STATUS_SUCCESS;
}