#include "./IoControl.h"

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
		case IOCTL_BYPASS_SEC_NO_CHANGE:
		{
			PBYPASS_SEC_NO_CHANGE_DATA data = (PBYPASS_SEC_NO_CHANGE_DATA)Irp->AssociatedIrp.SystemBuffer;

			PEPROCESS pProcess;
			if (NT_SUCCESS(PsLookupProcessByProcessId(data->ProcessId, &pProcess)))
			{
				BypassSecNoChange(pProcess, data->TargetAddress);
			}

			status = STATUS_SUCCESS;
			bytesIO = sizeof(BYPASS_SEC_NO_CHANGE_DATA);

			break;
		}
		case IOCTL_ENABLE_SEC_NO_CHANGE:
		{
			PENABLE_SEC_NO_CHANGE_DATA data = (PENABLE_SEC_NO_CHANGE_DATA)Irp->AssociatedIrp.SystemBuffer;

			PEPROCESS pProcess;
			if (NT_SUCCESS(PsLookupProcessByProcessId(data->ProcessId, &pProcess)))
			{
				EnableSecNoChange(pProcess, data->TargetAddress);
			}

			status = STATUS_SUCCESS;
			bytesIO = sizeof(ENABLE_SEC_NO_CHANGE_DATA);

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