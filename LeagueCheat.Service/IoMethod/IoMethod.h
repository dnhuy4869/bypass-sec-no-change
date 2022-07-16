#pragma once

#include "../DriverEntry.h"
#include "../NativeStructs/Nt19041.h"

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

typedef struct _BYPASS_SEC_NO_CHANGE_DATA
{
	ULONG ProcessId;
	ULONG TargetAddress;

} BYPASS_SEC_NO_CHANGE_DATA, * PBYPASS_SEC_NO_CHANGE_DATA;

typedef struct _ENABLE_SEC_NO_CHANGE_DATA
{
	ULONG ProcessId;
	ULONG TargetAddress;

} ENABLE_SEC_NO_CHANGE_DATA, * PENABLE_SEC_NO_CHANGE_DATA;

NTSTATUS ReadMemory(PEPROCESS Process, PVOID SourceAddress, PVOID TargetAddress, SIZE_T BufferSize);

NTSTATUS WriteMemory(PEPROCESS Process, PVOID SourceAddress, PVOID TargetAddress, SIZE_T BufferSize);

NTSTATUS WriteProtectedMemory(PEPROCESS Process, PVOID SourceAddress, PVOID TargetAddress, SIZE_T BufferSize);

NTSTATUS BypassSecNoChange(PEPROCESS Process, ULONG TargetAddress);

NTSTATUS EnableSecNoChange(PEPROCESS Process, ULONG TargetAddress);