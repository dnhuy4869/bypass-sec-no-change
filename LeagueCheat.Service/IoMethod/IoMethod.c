#include "./IoMethod.h"

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

TABLE_SEARCH_RESULT MiFindNodeOrParent(
	IN PRTL_AVL_TREE Table,
	IN ULONG_PTR StartingVpn,
	OUT PRTL_BALANCED_NODE* NodeOrParent
) {
	PRTL_BALANCED_NODE Child;
	PRTL_BALANCED_NODE NodeToExamine;
	PMMVAD_SHORT    VpnCompare;
	ULONG_PTR       startVpn;
	ULONG_PTR       endVpn;

	/*if (Table->NumberGenericTableElements == 0) {
		return TableEmptyTree;
	}*/

	NodeToExamine = (PRTL_BALANCED_NODE)(Table->Root);

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

			Child = NodeToExamine->Left;

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

			Child = NodeToExamine->Right;

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
	IN ULONG_PTR  address,
	OUT PMMVAD_SHORT* pResult
) {
	NTSTATUS status = STATUS_SUCCESS;
	ULONG_PTR vpnStart = address >> PAGE_SHIFT;

	ASSERT(pProcess != NULL && pResult != NULL);
	if (pProcess == NULL || pResult == NULL)
		return STATUS_INVALID_PARAMETER;


	PRTL_AVL_TREE pTable = (PRTL_AVL_TREE)((PUCHAR)pProcess + 0x7d8);
	PRTL_BALANCED_NODE pNode = (pTable->Root);

	if (MiFindNodeOrParent(pTable, vpnStart, &pNode) == TableFoundNode) {
		*pResult = (PMMVAD_SHORT)pNode;
	}
	else {
		status = STATUS_NOT_FOUND;
	}

	return status;
}

NTSTATUS BypassSecNoChange(PEPROCESS Process, ULONG TargetAddress)
{
	VMProtectBeginMutation(__FUNCTION__);

	PMMVAD_SHORT pVadShort = NULL;
	NTSTATUS status = FindVAD(Process, (ULONG64)TargetAddress, &pVadShort);
	if (!NT_SUCCESS(status))
	{
		return STATUS_ACCESS_DENIED;
	}

	pVadShort->u.VadFlags.NoChange = 0x0;
	pVadShort->u.VadFlags.Protection = 0x6;

	VMProtectEnd();
	return STATUS_SUCCESS;
}

NTSTATUS EnableSecNoChange(PEPROCESS Process, ULONG TargetAddress)
{
	VMProtectBeginMutation(__FUNCTION__);

	PMMVAD_SHORT pVadShort = NULL;
	NTSTATUS status = FindVAD(Process, (ULONG64)TargetAddress, &pVadShort);
	if (!NT_SUCCESS(status))
	{
		return STATUS_ACCESS_DENIED;
	}

	pVadShort->u.VadFlags.NoChange = 0x0;
	pVadShort->u.VadFlags.VadType = 0x0;
	pVadShort->u.VadFlags.Protection = 0x7;

	VMProtectEnd();
	return STATUS_SUCCESS;
}