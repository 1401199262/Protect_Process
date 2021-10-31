#pragma once

#define PROCESS_TERMINATE                  (0x0001)  
#define PROCESS_CREATE_THREAD              (0x0002)  
#define PROCESS_SET_SESSIONID              (0x0004)  
#define PROCESS_VM_OPERATION               (0x0008)  
#define PROCESS_VM_READ                    (0x0010)  
#define PROCESS_VM_WRITE                   (0x0020)  
#define PROCESS_DUP_HANDLE                 (0x0040)  
#define PROCESS_CREATE_PROCESS             (0x0080)  
#define PROCESS_SET_QUOTA                  (0x0100)  
#define PROCESS_SET_INFORMATION            (0x0200)  
#define PROCESS_QUERY_INFORMATION          (0x0400)  
#define PROCESS_SUSPEND_RESUME             (0x0800)  
#define PROCESS_QUERY_LIMITED_INFORMATION  (0x1000)  
#define PROCESS_SET_LIMITED_INFORMATION    (0x2000)  
#if (NTDDI_VERSION >= NTDDI_VISTA)
#define PROCESS_ALL_ACCESS        (STANDARD_RIGHTS_REQUIRED | SYNCHRONIZE | \
                                   0xFFFF)
#else
#define PROCESS_ALL_ACCESS        (STANDARD_RIGHTS_REQUIRED | SYNCHRONIZE | \
                                   0xFFF)
#endif

#define EXTERN_C extern"C"
#define DPRINT DbgPrint

typedef enum _WinVer
{
    WINVER_7 = 0x0610,
    WINVER_7_SP1 = 0x0611,

    WINVER_8 = 0x0620,
    WINVER_81 = 0x0630,

    WINVER_10 = 0x0A00,
    WINVER_10_RS1 = 0x0A01, // Anniversary update
    WINVER_10_RS2 = 0x0A02, // Creators update
    WINVER_10_RS3 = 0x0A03, // Fall creators update
    WINVER_10_RS4 = 0x0A04, // Spring creators update
    WINVER_10_RS5 = 0x0A05, // October 2018 update
    WINVER_10_19H1 = 0x0A06, // May 2019 update 19H1
    WINVER_10_19H2 = 0x0A07, // November 2019 update 19H2
    WINVER_10_20H1 = 0x0A08, // April 2020 update 20H1 -> 21H1

    WINVER_11_21H2 = 0x0B00 // 2021-09-13 update 21H2 (RTM) and Jun 2021 update Insider Preview 

} WinVer;
typedef struct _DYNAMIC_DATA
{
    WinVer  ver;            // OS version
    ULONG   buildNo;        // OS build revision
    BOOLEAN correctBuild;   // OS kernel build number is correct and supported

    ULONG KExecOpt;         // KPROCESS::ExecuteOptions 

    ULONG UniquePId;        // EPROCESS::UniqueProcessId
    ULONG ActiveProcessLink;// EPROCESS::ActiveProcessLinks (UniquePID + 0x8)
    ULONG Protection;       // EPROCESS::Protection
    ULONG EProcessFlags2;   // EPROCESS::Flags2
    ULONG ObjTable;         // EPROCESS::ObjectTable
    ULONG VadRoot;          // EPROCESS::VadRoot
    ULONG DebugPort;        // EPROCESS::DebugPort

    ULONG NtProtectIndex;   // NtProtectVirtualMemory SSDT index
    ULONG NtCreateThdExIndex; // NtCreateThreadEx SSDT index
    ULONG NtTermThdIndex;   // NtTerminateThread SSDT index
    ULONG PrevMode;         // KTHREAD::PreviousMode
    ULONG ExitStatus;       // ETHREAD::ExitStatus
    ULONG MiAllocPage;      // MiAllocateDriverPage offset
    ULONG ExRemoveTable;    // Ex(p)RemoveHandleTable offset

    ULONG_PTR DYN_PDE_BASE; // Win10 AU+ relocated PDE base VA
    ULONG_PTR DYN_PTE_BASE; // Win10 AU+ relocated PTE base VA
} DYNAMIC_DATA, * PDYNAMIC_DATA;


typedef enum _MI_VAD_TYPE
{
    VadNone,
    VadDevicePhysicalMemory,
    VadImageMap,
    VadAwe,
    VadWriteWatch,
    VadLargePages,
    VadRotatePhysical,
    VadLargePageSection
} MI_VAD_TYPE, * PMI_VAD_TYPE;

#define GET_VAD_ROOT(Table) Table->BalancedRoot

//
// PTE protection values
//
#define MM_ZERO_ACCESS         0
#define MM_READONLY            1
#define MM_EXECUTE             2
#define MM_EXECUTE_READ        3
#define MM_READWRITE           4
#define MM_WRITECOPY           5
#define MM_EXECUTE_READWRITE   6
#define MM_EXECUTE_WRITECOPY   7

struct _MMVAD_FLAGS // Size=4
{
    unsigned long VadType : 3; // Size=4 Offset=0 BitOffset=0 BitCount=3
    unsigned long Protection : 5; // Size=4 Offset=0 BitOffset=3 BitCount=5
    unsigned long PreferredNode : 6; // Size=4 Offset=0 BitOffset=8 BitCount=6
    unsigned long NoChange : 1; // Size=4 Offset=0 BitOffset=14 BitCount=1
    unsigned long PrivateMemory : 1; // Size=4 Offset=0 BitOffset=15 BitCount=1
    unsigned long Teb : 1; // Size=4 Offset=0 BitOffset=16 BitCount=1
    unsigned long PrivateFixup : 1; // Size=4 Offset=0 BitOffset=17 BitCount=1
    unsigned long ManySubsections : 1; // Size=4 Offset=0 BitOffset=18 BitCount=1
    unsigned long Spare : 12; // Size=4 Offset=0 BitOffset=19 BitCount=12
    unsigned long DeleteInProgress : 1; // Size=4 Offset=0 BitOffset=31 BitCount=1
};

struct _MMVAD_FLAGS1 // Size=4
{
    unsigned long CommitCharge : 31; // Size=4 Offset=0 BitOffset=0 BitCount=31
    unsigned long MemCommit : 1; // Size=4 Offset=0 BitOffset=31 BitCount=1
};

union ___unnamed1951 // Size=4
{
    unsigned long LongFlags; // Size=4 Offset=0
    struct _MMVAD_FLAGS VadFlags; // Size=4 Offset=0
};

union ___unnamed1952 // Size=4
{
    unsigned long LongFlags1; // Size=4 Offset=0
    struct _MMVAD_FLAGS1 VadFlags1; // Size=4 Offset=0

}; 
struct _MMVAD_FLAGS2 // Size=4
{
    unsigned long FileOffset : 24; // Size=4 Offset=0 BitOffset=0 BitCount=24
    unsigned long Large : 1; // Size=4 Offset=0 BitOffset=24 BitCount=1
    unsigned long TrimBehind : 1; // Size=4 Offset=0 BitOffset=25 BitCount=1
    unsigned long Inherit : 1; // Size=4 Offset=0 BitOffset=26 BitCount=1
    unsigned long CopyOnWrite : 1; // Size=4 Offset=0 BitOffset=27 BitCount=1
    unsigned long NoValidationNeeded : 1; // Size=4 Offset=0 BitOffset=28 BitCount=1
    unsigned long Spare : 3; // Size=4 Offset=0 BitOffset=29 BitCount=3
};
union ___unnamed2047 // Size=4
{
    unsigned long LongFlags2; // Size=4 Offset=0
    struct _MMVAD_FLAGS2 VadFlags2; // Size=4 Offset=0
};
struct _MI_VAD_SEQUENTIAL_INFO // Size=8
{
    unsigned __int64 Length : 12; // Size=8 Offset=0 BitOffset=0 BitCount=12
    unsigned __int64 Vpn : 52; // Size=8 Offset=0 BitOffset=12 BitCount=52
};

union ___unnamed2048 // Size=8
{
    struct _MI_VAD_SEQUENTIAL_INFO SequentialVa; // Size=8 Offset=0
    struct _MMEXTEND_INFO* ExtendedInfo; // Size=8 Offset=0
};
typedef union _EX_FAST_REF // Size=8
{
    void* Object;
    struct
    {
        unsigned __int64 RefCnt : 4;
    };
    unsigned __int64 Value;
} EX_FAST_REF, * PEX_FAST_REF;
typedef struct _CONTROL_AREA // Size=120
{
    struct _SEGMENT* Segment;
    struct _LIST_ENTRY ListHead;
    unsigned __int64 NumberOfSectionReferences;
    unsigned __int64 NumberOfPfnReferences;
    unsigned __int64 NumberOfMappedViews;
    unsigned __int64 NumberOfUserReferences;
    unsigned long f1;
    unsigned long f2;
    EX_FAST_REF FilePointer;
    // Other fields
} CONTROL_AREA, * PCONTROL_AREA;
typedef struct _SUBSECTION // Size=56
{
    PCONTROL_AREA ControlArea;
    // Other fields
} SUBSECTION, * PSUBSECTION;
union _EX_PUSH_LOCK // Size=8
{
    struct
    {
        unsigned __int64 Locked : 1; // Size=8 Offset=0 BitOffset=0 BitCount=1
        unsigned __int64 Waiting : 1; // Size=8 Offset=0 BitOffset=1 BitCount=1
        unsigned __int64 Waking : 1; // Size=8 Offset=0 BitOffset=2 BitCount=1
        unsigned __int64 MultipleShared : 1; // Size=8 Offset=0 BitOffset=3 BitCount=1
        unsigned __int64 Shared : 60; // Size=8 Offset=0 BitOffset=4 BitCount=60
    };
    unsigned __int64 Value; // Size=8 Offset=0
    void* Ptr; // Size=8 Offset=0
};
typedef struct _MMVAD_SHORT // Size=64
{
    union
    {
        struct _RTL_BALANCED_NODE VadNode; // Size=24 Offset=0
        struct _MMVAD_SHORT* NextVad; // Size=8 Offset=0
    };
    unsigned long StartingVpn; // Size=4 Offset=24
    unsigned long EndingVpn; // Size=4 Offset=28
    unsigned char StartingVpnHigh; // Size=1 Offset=32
    unsigned char EndingVpnHigh; // Size=1 Offset=33
    unsigned char CommitChargeHigh; // Size=1 Offset=34
    unsigned char SpareNT64VadUChar; // Size=1 Offset=35
    long ReferenceCount; // Size=4 Offset=36
    union _EX_PUSH_LOCK PushLock; // Size=8 Offset=40
    union ___unnamed1951 u; // Size=4 Offset=48
    union ___unnamed1952 u1; // Size=4 Offset=52
    struct _MI_VAD_EVENT_BLOCK* EventList; // Size=8 Offset=56
} MMVAD_SHORT, * PMMVAD_SHORT;
typedef struct _MMVAD // Size=128
{
    struct _MMVAD_SHORT Core; // Size=64 Offset=0
    union ___unnamed2047 u2; // Size=4 Offset=64
    unsigned long pad0;  // Size=4 Offset=68
    struct _SUBSECTION* Subsection; // Size=8 Offset=72
    struct _MMPTE* FirstPrototypePte; // Size=8 Offset=80
    struct _MMPTE* LastContiguousPte; // Size=8 Offset=88
    struct _LIST_ENTRY ViewLinks; // Size=16 Offset=96
    struct _EPROCESS* VadsProcess; // Size=8 Offset=112
    union ___unnamed2048 u4; // Size=8 Offset=120
    struct _FILE_OBJECT* FileObject; // Size=8 Offset=128
} MMVAD, * PMMVAD;

typedef struct _MM_AVL_NODE // Size=24
{
    struct _MM_AVL_NODE* LeftChild; // Size=8 Offset=0
    struct _MM_AVL_NODE* RightChild; // Size=8 Offset=8

    union ___unnamed1666 // Size=8
    {
        struct
        {
            __int64 Balance : 2; // Size=8 Offset=0 BitOffset=0 BitCount=2
        };
        struct _MM_AVL_NODE* Parent; // Size=8 Offset=0
    } u1;
} MM_AVL_NODE, * PMM_AVL_NODE, * PMMADDRESS_NODE;
typedef struct _RTL_AVL_TREE // Size=8
{
    PMM_AVL_NODE BalancedRoot;
    void* NodeHint;
    unsigned __int64 NumberGenericTableElements;
} RTL_AVL_TREE, * PRTL_AVL_TREE, MM_AVL_TABLE, * PMM_AVL_TABLE;





EXTERN_C NTSTATUS ZwQuerySystemInformation(
    ULONG SystemClass,
    PVOID SystemInformation,
    ULONG SystemInformationLength,
    PULONG RetLength
);



#define HANDLE_VALUE_INC 4

#define TABLE_PAGE_SIZE PAGE_SIZE
#define LOWLEVEL_COUNT (TABLE_PAGE_SIZE / sizeof(HANDLE_TABLE_ENTRY))
#define MIDLEVEL_COUNT (PAGE_SIZE / sizeof(PHANDLE_TABLE_ENTRY))

#define LEVEL_CODE_MASK 3

EXTERN_C NTKERNELAPI POBJECT_TYPE ObGetObjectType(PVOID Object);
typedef struct _EXHANDLE
{
    union
    {
        struct
        {
            ULONG32 TagBits : 2;
            ULONG32 Index : 30;
        };
        HANDLE GenericHandleOverlay;
        ULONG_PTR Value;
    };
} EXHANDLE, * PEXHANDLE;

typedef struct _HANDLE_TABLE_ENTRY
{
    union
    {
        LONG_PTR VolatileLowValue;
        LONG_PTR LowValue;
        PVOID InfoTable;
        LONG_PTR RefCountField;
        struct
        {
            ULONG_PTR Unlocked : 1;
            ULONG_PTR RefCnt : 16;
            ULONG_PTR Attributes : 3;
            ULONG_PTR ObjectPointerBits : 44;
        };
    };
    union
    {
        LONG_PTR HighValue;
        struct _HANDLE_TABLE_ENTRY* NextFreeHandleEntry;
        EXHANDLE LeafHandleValue;
        struct
        {
            ULONG32 GrantedAccessBits : 25;
            ULONG32 NoRightsUpgrade : 1;
            ULONG32 Spare1 : 6;
        };
        ULONG32 Spare2;
    };
} HANDLE_TABLE_ENTRY, * PHANDLE_TABLE_ENTRY;

typedef struct _HANDLE_TABLE
{
    ULONG NextHandleNeedingPool;
    long ExtraInfoPages;
    LONG_PTR TableCode;
    PEPROCESS QuotaProcess;
    LIST_ENTRY HandleTableList;
    ULONG UniqueProcessId;
    ULONG Flags;
    EX_PUSH_LOCK HandleContentionEvent;
    EX_PUSH_LOCK HandleTableLock;
    // More fields here...
} HANDLE_TABLE, * PHANDLE_TABLE;

typedef BOOLEAN(*EX_ENUMERATE_HANDLE_ROUTINE)(
#if !defined(_WIN7_)
    IN PHANDLE_TABLE HandleTable,
#endif
    IN PHANDLE_TABLE_ENTRY HandleTableEntry,
    IN HANDLE Handle,
    IN PVOID EnumParameter
    );



EXTERN_C
NTKERNELAPI
BOOLEAN
ExEnumHandleTable(
    IN PHANDLE_TABLE HandleTable,
    IN EX_ENUMERATE_HANDLE_ROUTINE EnumHandleProcedure,
    IN PVOID EnumParameter,
    OUT PHANDLE Handle
);

EXTERN_C
NTKERNELAPI
VOID
FASTCALL
ExfUnblockPushLock(
    IN OUT PEX_PUSH_LOCK PushLock,
    IN OUT PVOID WaitBlock
);
#define EX_ADDITIONAL_INFO_SIGNATURE (ULONG_PTR)(-2)
#define ExpIsValidObjectEntry(Entry) \
    ( (Entry != NULL) && (Entry->LowValue != 0) && (Entry->HighValue != EX_ADDITIONAL_INFO_SIGNATURE) )

EXTERN_C NTKERNELAPI UCHAR* PsGetProcessImageFileName(__in PEPROCESS Process);




#pragma region Input_Struct_Stuff

typedef struct _HANDLE_GRANT_ACCESS
{
    ULONGLONG  handle;      // Handle to modify
    ULONG      pid;         // Process ID
    ULONG      access;      // Access flags to grant
} HANDLE_GRANT_ACCESS, * PHANDLE_GRANT_ACCESS;



#pragma endregion





typedef struct _SYSTEM_THREAD_INFORMATION
{
    LARGE_INTEGER KernelTime;
    LARGE_INTEGER UserTime;
    LARGE_INTEGER CreateTime;
    ULONG WaitTime;
    PVOID StartAddress;
    CLIENT_ID ClientId;
    KPRIORITY Priority;
    LONG BasePriority;
    ULONG ContextSwitches;
    ULONG ThreadState;
    ULONG WaitReason;
    ULONG PadPadAlignment;
} SYSTEM_THREAD_INFORMATION, * PSYSTEM_THREAD_INFORMATION;
typedef struct _SYSTEM_PROCESS_INFORMATION
{
    ULONG NextEntryOffset;
    ULONG NumberOfThreads;
    LARGE_INTEGER WorkingSetPrivateSize;
    ULONG HardFaultCount;
    ULONG NumberOfThreadsHighWatermark;
    ULONGLONG CycleTime;
    LARGE_INTEGER CreateTime;
    LARGE_INTEGER UserTime;
    LARGE_INTEGER KernelTime;
    UNICODE_STRING ImageName;
    KPRIORITY BasePriority;
    HANDLE UniqueProcessId;
    HANDLE InheritedFromUniqueProcessId;
    ULONG HandleCount;
    ULONG SessionId;
    ULONG_PTR UniqueProcessKey;
    SIZE_T PeakVirtualSize;
    SIZE_T VirtualSize;
    ULONG PageFaultCount;
    SIZE_T PeakWorkingSetSize;
    SIZE_T WorkingSetSize;
    SIZE_T QuotaPeakPagedPoolUsage;
    SIZE_T QuotaPagedPoolUsage;
    SIZE_T QuotaPeakNonPagedPoolUsage;
    SIZE_T QuotaNonPagedPoolUsage;
    SIZE_T PagefileUsage;
    SIZE_T PeakPagefileUsage;
    SIZE_T PrivatePageCount;
    LARGE_INTEGER ReadOperationCount;
    LARGE_INTEGER WriteOperationCount;
    LARGE_INTEGER OtherOperationCount;
    LARGE_INTEGER ReadTransferCount;
    LARGE_INTEGER WriteTransferCount;
    LARGE_INTEGER OtherTransferCount;
    SYSTEM_THREAD_INFORMATION Threads[1];
}SYSTEM_PROCESS_INFORMATION, * PSYSTEM_PROCESS_INFORMATION;