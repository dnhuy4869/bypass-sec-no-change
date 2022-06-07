#include <ntifs.h>

#include "ntos.h"

#pragma warning (disable: 4022 4100 4311 4047 4024 6273 4146 4113 4189 6011)

#define LOG(x, ...) DbgPrintEx(0, 0, x, __VA_ARGS__)

#define IOCTL_READ_MEMORY CTL_CODE(FILE_DEVICE_UNKNOWN, 0x6991, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)
#define IOCTL_WRITE_MEMORY CTL_CODE(FILE_DEVICE_UNKNOWN, 0x6992, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)
#define IOCTL_WRITE_PROTECTED_MEMORY CTL_CODE(FILE_DEVICE_UNKNOWN, 0x6993, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)

typedef struct _READ_MEMORY_DATA
{
	ULONG ProcessId;
	ULONG TargetAddress;
	PVOID Buffer;
	ULONG BufferSize;

} READ_MEMORY_DATA, * PREAD_MEMORY_DATA;

typedef struct _WRITE_MEMORY_DATA
{
	ULONG ProcessId;
	ULONG TargetAddress;
	PVOID Buffer;
	ULONG BufferSize;

} WRITE_MEMORY_DATA, * PWRITE_MEMORY_DATA;

typedef struct _WRITE_PROTECTED_MEMORY_DATA
{
	ULONG ProcessId;
	ULONG TargetAddress;
	PVOID Buffer;
	ULONG BufferSize;

} WRITE_PROTECTED_MEMORY_DATA, * PWRITE_PROTECTED_MEMORY_DATA;

PDEVICE_OBJECT pDeviceObject;
UNICODE_STRING Dev, Dos;

NTSTATUS ReadMemory(PEPROCESS Process, PVOID SourceAddress, PVOID TargetAddress, SIZE_T BufferSize)
{
	SIZE_T bytes;
	if (NT_SUCCESS(MmCopyVirtualMemory(Process,
									   SourceAddress,
									   PsGetCurrentProcess(), 
									   TargetAddress, 
									   BufferSize,
									   KernelMode, 
									   &bytes)))
	{
		return STATUS_SUCCESS;
	}

	return STATUS_ACCESS_DENIED;
}

NTSTATUS WriteMemory(PEPROCESS Process, PVOID SourceAddress, PVOID TargetAddress, SIZE_T BufferSize)
{
	SIZE_T bytes;
	if (NT_SUCCESS(MmCopyVirtualMemory(PsGetCurrentProcess(), 
									   SourceAddress, 
									   Process, 
									   TargetAddress, 
									   BufferSize,
									   KernelMode, 
									   &bytes)))
	{
		return STATUS_SUCCESS;
	}

	return STATUS_ACCESS_DENIED;
}

NTSTATUS WriteProtectedMemory(PEPROCESS Process, PVOID SourceAddress, PVOID TargetAddress, SIZE_T BufferSize)
{
	SIZE_T bytes;
	NTSTATUS status;

	__try
	{
		_disable();
		__writecr0(__readcr0() & (~(1 << 16)));

		status = MmCopyVirtualMemory(PsGetCurrentProcess(),
									 SourceAddress,
									 Process,
									 TargetAddress,
									 BufferSize,
									 KernelMode,
									 &bytes);

		__writecr0(__readcr0() | (1 << 16));
		_enable();
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		return STATUS_ACCESS_DENIED;
	}

	if (!NT_SUCCESS(status))
	{
		return STATUS_ACCESS_DENIED;
	}

	return STATUS_SUCCESS;
}

NTSTATUS CreateCall(PDEVICE_OBJECT DeviceObject, PIRP Irp)
{
	Irp->IoStatus.Status = STATUS_SUCCESS;
	Irp->IoStatus.Information = 0;
	IoCompleteRequest(Irp, IO_NO_INCREMENT);

	return STATUS_SUCCESS;
}

NTSTATUS CloseCall(PDEVICE_OBJECT DeviceObject, PIRP Irp)
{
	Irp->IoStatus.Status = STATUS_SUCCESS;
	Irp->IoStatus.Information = 0;
	IoCompleteRequest(Irp, IO_NO_INCREMENT);

	return STATUS_SUCCESS;
}

NTSTATUS IoControl(PDEVICE_OBJECT DeviceObject, PIRP Irp)
{
	NTSTATUS status = STATUS_SUCCESS;
	ULONG bytesIO = 0;

	PIO_STACK_LOCATION stack = IoGetCurrentIrpStackLocation(Irp);
	ULONG controlCode = stack->Parameters.DeviceIoControl.IoControlCode;

	switch (controlCode)
	{
		case IOCTL_READ_MEMORY:
		{
			PREAD_MEMORY_DATA data = (PREAD_MEMORY_DATA)Irp->AssociatedIrp.SystemBuffer;

			PEPROCESS pProcess;
			if (NT_SUCCESS(PsLookupProcessByProcessId(data->ProcessId, &pProcess)))
			{
				ReadMemory(pProcess, data->TargetAddress, data->Buffer, data->BufferSize);
			}

			status = STATUS_SUCCESS;
			bytesIO = sizeof(READ_MEMORY_DATA);

			break;
		}
		case IOCTL_WRITE_MEMORY:
		{
			PWRITE_MEMORY_DATA data = (PWRITE_MEMORY_DATA)Irp->AssociatedIrp.SystemBuffer;

			PEPROCESS pProcess;
			if (NT_SUCCESS(PsLookupProcessByProcessId(data->ProcessId, &pProcess)))
			{
				WriteMemory(pProcess, data->Buffer, data->TargetAddress, data->BufferSize);
			}

			status = STATUS_SUCCESS;
			bytesIO = sizeof(WRITE_MEMORY_DATA);

			break;
		}
		case IOCTL_WRITE_PROTECTED_MEMORY:
		{
			PWRITE_PROTECTED_MEMORY_DATA data = (PWRITE_PROTECTED_MEMORY_DATA)Irp->AssociatedIrp.SystemBuffer;

			PEPROCESS pProcess;
			if (NT_SUCCESS(PsLookupProcessByProcessId(data->ProcessId, &pProcess)))
			{
				WriteProtectedMemory(pProcess, data->Buffer, data->TargetAddress, data->BufferSize);
			}

			status = STATUS_SUCCESS;
			bytesIO = sizeof(WRITE_PROTECTED_MEMORY_DATA);

			break;
		}
		default:
		{
			status = STATUS_INVALID_PARAMETER;
			break;
		}
	}

	Irp->IoStatus.Status = status;
	Irp->IoStatus.Information = bytesIO;
	IoCompleteRequest(Irp, IO_NO_INCREMENT);

	return status;
}

NTSTATUS UnloadDriver(PDRIVER_OBJECT pDriverObject)
{
	IoDeleteSymbolicLink(&Dos);
	IoDeleteDevice(pDriverObject->DeviceObject);

    return STATUS_SUCCESS;
}

NTSTATUS DriverEntry(PDRIVER_OBJECT pDriverObject, PUNICODE_STRING pRegistryPath)
{
	RtlInitUnicodeString(&Dev, L"\\Device\\LeagueCheat.IO");
	RtlInitUnicodeString(&Dos, L"\\DosDevices\\LeagueCheat.IO");

	IoCreateDevice(pDriverObject, 0, &Dev, FILE_DEVICE_UNKNOWN, FILE_DEVICE_SECURE_OPEN, FALSE, &pDeviceObject);
	IoCreateSymbolicLink(&Dos, &Dev);

	pDriverObject->MajorFunction[IRP_MJ_CREATE] = CreateCall;
	pDriverObject->MajorFunction[IRP_MJ_CLOSE] = CloseCall;
	pDriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = IoControl;
	pDriverObject->DriverUnload = UnloadDriver;

	pDeviceObject->Flags |= DO_DIRECT_IO;
	pDeviceObject->Flags &= ~DO_DEVICE_INITIALIZING;

    return STATUS_SUCCESS;
}