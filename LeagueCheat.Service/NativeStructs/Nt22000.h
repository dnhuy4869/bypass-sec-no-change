#pragma once

#include "../DriverEntry.h"

typedef struct _RTL_AVL_TREE
{
    /* 0x0000 */ struct _RTL_BALANCED_NODE* Root;
} RTL_AVL_TREE, * PRTL_AVL_TREE; /* size: 0x0008 */

typedef struct _EX_PUSH_LOCK
{
    union
    {
        struct /* bitfield */
        {
            /* 0x0000 */ unsigned __int64 Locked : 1; /* bit position: 0 */
            /* 0x0000 */ unsigned __int64 Waiting : 1; /* bit position: 1 */
            /* 0x0000 */ unsigned __int64 Waking : 1; /* bit position: 2 */
            /* 0x0000 */ unsigned __int64 MultipleShared : 1; /* bit position: 3 */
            /* 0x0000 */ unsigned __int64 Shared : 60; /* bit position: 4 */
        }; /* bitfield */
        /* 0x0000 */ unsigned __int64 Value;
        /* 0x0000 */ void* Ptr;
    }; /* size: 0x0008 */
};

typedef struct _MMVAD_FLAGS
{
    struct /* bitfield */
    {
        ULONG Lock : 1;                                                           //0x0
        ULONG LockContended : 1;                                                  //0x0
        ULONG DeleteInProgress : 1;                                               //0x0
        ULONG NoChange : 1;                                                       //0x0
        ULONG VadType : 3;                                                        //0x0
        ULONG Protection : 5;                                                     //0x0
        ULONG PreferredNode : 7;                                                  //0x0
        ULONG PageSize : 2;                                                       //0x0
        ULONG PrivateMemory : 1;                                                  //0x0
    }; /* bitfield */
} MMVAD_FLAGS, * PMMVAD_FLAGS; /* size: 0x0004 */

typedef struct _MM_PRIVATE_VAD_FLAGS
{
    struct /* bitfield */
    {
        ULONG Lock : 1;                                                           //0x0
        ULONG LockContended : 1;                                                  //0x0
        ULONG DeleteInProgress : 1;                                               //0x0
        ULONG NoChange : 1;                                                       //0x0
        ULONG VadType : 3;                                                        //0x0
        ULONG Protection : 5;                                                     //0x0
        ULONG PreferredNode : 7;                                                  //0x0
        ULONG PageSize : 2;                                                       //0x0
        ULONG PrivateMemoryAlwaysSet : 1;                                         //0x0
        ULONG WriteWatch : 1;                                                     //0x0
        ULONG FixedLargePageSize : 1;                                             //0x0
        ULONG ZeroFillPagesOptional : 1;                                          //0x0
        ULONG Graphics : 1;                                                       //0x0
        ULONG Enclave : 1;                                                        //0x0
        ULONG ShadowStack : 1;                                                    //0x0
        ULONG PhysicalMemoryPfnsReferenced : 1;                                   //0x0
    }; /* bitfield */
} MM_PRIVATE_VAD_FLAGS, * PMM_PRIVATE_VAD_FLAGS; /* size: 0x0004 */

typedef struct _MM_GRAPHICS_VAD_FLAGS
{
    struct /* bitfield */
    {
        ULONG Lock : 1;                                                           //0x0
        ULONG LockContended : 1;                                                  //0x0
        ULONG DeleteInProgress : 1;                                               //0x0
        ULONG NoChange : 1;                                                       //0x0
        ULONG VadType : 3;                                                        //0x0
        ULONG Protection : 5;                                                     //0x0
        ULONG PreferredNode : 7;                                                  //0x0
        ULONG PageSize : 2;                                                       //0x0
        ULONG PrivateMemoryAlwaysSet : 1;                                         //0x0
        ULONG WriteWatch : 1;                                                     //0x0
        ULONG FixedLargePageSize : 1;                                             //0x0
        ULONG ZeroFillPagesOptional : 1;                                          //0x0
        ULONG GraphicsAlwaysSet : 1;                                              //0x0
        ULONG GraphicsUseCoherentBus : 1;                                         //0x0
        ULONG GraphicsNoCache : 1;                                                //0x0
        ULONG GraphicsPageProtection : 3;                                         //0x0
    }; /* bitfield */
} MM_GRAPHICS_VAD_FLAGS, * PMM_GRAPHICS_VAD_FLAGS; /* size: 0x0004 */

typedef struct _MM_SHARED_VAD_FLAGS
{
    struct /* bitfield */
    {
        ULONG Lock : 1;                                                           //0x0
        ULONG LockContended : 1;                                                  //0x0
        ULONG DeleteInProgress : 1;                                               //0x0
        ULONG NoChange : 1;                                                       //0x0
        ULONG VadType : 3;                                                        //0x0
        ULONG Protection : 5;                                                     //0x0
        ULONG PreferredNode : 7;                                                  //0x0
        ULONG PageSize : 2;                                                       //0x0
        ULONG PrivateMemoryAlwaysClear : 1;                                       //0x0
        ULONG PrivateFixup : 1;                                                   //0x0
        ULONG HotPatchState : 2;                                                  //0x0
    }; /* bitfield */
} MM_SHARED_VAD_FLAGS, * PMM_SHARED_VAD_FLAGS; /* size: 0x0004 */

typedef struct _MMVAD_FLAGS1
{
    struct /* bitfield */
    {
        ULONG CommitCharge : 31;                                                  //0x0
        ULONG MemCommit : 1;                                                      //0x0
    }; /* bitfield */
} MMVAD_FLAGS1, * PMMVAD_FLAGS1; /* size: 0x0004 */

typedef struct _MMVAD_SHORT
{
    union
    {
        struct
        {
            struct _MMVAD_SHORT* NextVad;                                   //0x0
            VOID* ExtraCreateInfo;                                          //0x8
        };
        struct _RTL_BALANCED_NODE VadNode;                                  //0x0
    };
    ULONG StartingVpn;                                                      //0x18
    ULONG EndingVpn;                                                        //0x1c
    UCHAR StartingVpnHigh;                                                  //0x20
    UCHAR EndingVpnHigh;                                                    //0x21
    UCHAR CommitChargeHigh;                                                 //0x22
    UCHAR SpareNT64VadUChar;                                                //0x23
    LONG ReferenceCount;                                                    //0x24
    struct _EX_PUSH_LOCK PushLock;                                          //0x28
    union
    {
        ULONG LongFlags;                                                    //0x30
        struct _MMVAD_FLAGS VadFlags;                                       //0x30
        struct _MM_PRIVATE_VAD_FLAGS PrivateVadFlags;                       //0x30
        struct _MM_GRAPHICS_VAD_FLAGS GraphicsVadFlags;                     //0x30
        struct _MM_SHARED_VAD_FLAGS SharedVadFlags;                         //0x30
        volatile ULONG VolatileVadLong;                                     //0x30
    } u;                                                                    //0x30
    union
    {
        ULONG LongFlags1;                                                   //0x34
        struct _MMVAD_FLAGS1 VadFlags1;                                     //0x34
    } u1;                                                                   //0x34
    union
    {
        ULONGLONG EventListULongPtr;                                        //0x38
        UCHAR StartingVpnHigher : 4;                                          //0x38
    } u5;
} MMVAD_SHORT, * PMMVAD_SHORT; /* size: 0x0040 */