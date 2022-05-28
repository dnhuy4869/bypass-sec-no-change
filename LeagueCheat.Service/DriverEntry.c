#include <ntifs.h>

#include "ntos.h"
#include "NT.h"

#pragma warning (disable: 4100 4311 4047 4024 6273 4146 4113 4189)

#define LOG(x, ...) DbgPrintEx(0, 0, x, __VA_ARGS__)

TABLE_SEARCH_RESULT MiFindNodeOrParent(
	IN PMM_AVL_TABLE Table,
	IN ULONG_PTR StartingVpn,
	OUT PMMADDRESS_NODE* NodeOrParent
) {
	PMMADDRESS_NODE Child;
	PMMADDRESS_NODE NodeToExamine;
	PMMVAD_SHORT    VpnCompare;
	ULONG_PTR       startVpn;
	ULONG_PTR       endVpn;

	if (Table->NumberGenericTableElements == 0) {
		return TableEmptyTree;
	}

	NodeToExamine = (PMMADDRESS_NODE)(Table->BalancedRoot);

	for (;;) {

		VpnCompare = (PMMVAD_SHORT)NodeToExamine;
		startVpn = VpnCompare->StartingVpn;
		endVpn = VpnCompare->EndingVpn;

#if defined( _WIN81_ ) || defined( _WIN10_ )
		startVpn |= (ULONG_PTR)VpnCompare->StartingVpnHigh << 32;
		endVpn |= (ULONG_PTR)VpnCompare->EndingVpnHigh << 32;
#endif  

		//
		// Compare the buffer with the key in the tree element.
		//

		if (StartingVpn < startVpn) {

			Child = NodeToExamine->LeftChild;

			if (Child != NULL) {
				NodeToExamine = Child;
			}
			else {

				//
				// Node is not in the tree.  Set the output
				// parameter to point to what would be its
				// parent and return which child it would be.
				//

				*NodeOrParent = NodeToExamine;
				return TableInsertAsLeft;
			}
		}
		else if (StartingVpn <= endVpn) {

			//
			// This is the node.
			//

			*NodeOrParent = NodeToExamine;
			return TableFoundNode;
		}
		else {

			Child = NodeToExamine->RightChild;

			if (Child != NULL) {
				NodeToExamine = Child;
			}
			else {

				//
				// Node is not in the tree.  Set the output
				// parameter to point to what would be its
				// parent and return which child it would be.
				//

				*NodeOrParent = NodeToExamine;
				return TableInsertAsRight;
			}
		}

	};
}

NTSTATUS FindVAD(
	IN PEPROCESS pProcess,
	IN ULONG_PTR address,
	OUT PMMVAD_SHORT* pResult
) {
	NTSTATUS status = STATUS_SUCCESS;
	ULONG_PTR vpnStart = address >> PAGE_SHIFT;

	ASSERT(pProcess != NULL && pResult != NULL);
	if (pProcess == NULL || pResult == NULL)
		return STATUS_INVALID_PARAMETER;


	PMM_AVL_TABLE pTable = (PMM_AVL_TABLE)((PUCHAR)pProcess + 0x7d8);
	PMM_AVL_NODE pNode = (pTable->BalancedRoot);

	if (MiFindNodeOrParent(pTable, vpnStart, &pNode) == TableFoundNode) {
		*pResult = (PMMVAD_SHORT)pNode;
	}
	else {
		status = STATUS_NOT_FOUND;
	}

	return status;
}

NTSTATUS Sleep(ULONG64 milliseconds)
{
	LARGE_INTEGER delay;
	ULONG* split;

	milliseconds *= 1000000;
	milliseconds /= 100;
	milliseconds = -milliseconds;

	split = (ULONG*)&milliseconds;
	delay.LowPart = *split;
	split++;
	delay.HighPart = *split;

	KeDelayExecutionThread(KernelMode, 0, &delay);

	return STATUS_SUCCESS;
}

ULONG TargetId;
ULONG ImageBase;
ULONG ImageSize;

NTSTATUS UnprotectSecNoChange()
{
	Sleep(8000);

	NTSTATUS status = STATUS_SUCCESS;
	PEPROCESS pProcess = NULL;

	status = PsLookupProcessByProcessId((HANDLE)TargetId, &pProcess);
	if (!NT_SUCCESS(status))
	{
		//LOG("PsLookupProcessByProcessId failed 0x%lX", status);
		return STATUS_SUCCESS;
	}

	PMMVAD_SHORT pVadShort = NULL;
	status = FindVAD(pProcess, (ULONG64)ImageBase, &pVadShort);
	if (!NT_SUCCESS(status))
	{
		//LOG("FindVAD failed 0x%lX", status);
		return STATUS_SUCCESS;
	}

	pVadShort->u.VadFlags.NoChange = 0;
	pVadShort->u.VadFlags.Protection = 6;

	return STATUS_SUCCESS;
}

PLOAD_IMAGE_NOTIFY_ROUTINE ImageLoadCallback(PUNICODE_STRING FullImageName, HANDLE ProcessId, PIMAGE_INFO ImageInfo)
{
	if (wcsstr(FullImageName->Buffer, L"League of Legends.exe"))
	{
		TargetId = (ULONG)ProcessId;
		ImageBase = (ULONG)ImageInfo->ImageBase;
		ImageSize = (ULONG)ImageInfo->ImageSize;

		NTSTATUS status = STATUS_SUCCESS;
		HANDLE hThread = NULL;
		PEPROCESS pProcess = NULL;

		status = PsCreateSystemThread(
			&hThread, 
			THREAD_ALL_ACCESS,
			0, 0, 0,
			UnprotectSecNoChange,
			0
		);

		if (!NT_SUCCESS(status))
		{
			//LOG("PsCreateSystemThread failed 0x%lX", status);
			return STATUS_SUCCESS;
		}
	}

	return STATUS_SUCCESS;
}

NTSTATUS UnloadDriver(PDRIVER_OBJECT pDriverObject)
{
    PsRemoveLoadImageNotifyRoutine(ImageLoadCallback);

    return STATUS_SUCCESS;
}

NTSTATUS DriverEntry(PDRIVER_OBJECT pDriverObject, PUNICODE_STRING pRegistryPath)
{
    pDriverObject->DriverUnload = UnloadDriver;

    PsSetLoadImageNotifyRoutine(ImageLoadCallback);

    return STATUS_SUCCESS;
}