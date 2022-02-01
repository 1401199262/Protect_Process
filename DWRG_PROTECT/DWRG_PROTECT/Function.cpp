#include "Function.h"
#include "PipiDef.h"

DYNAMIC_DATA dynData;

#define PDE_BASE    0xFFFFF6FB40000000UI64
#define PTE_BASE    0xFFFFF68000000000UI64

#define BB_POOL_TAG 'enoB'
NTSTATUS BBGetBuildNO(OUT PULONG pBuildNo)
{
    ASSERT(pBuildNo != NULL);
    if (pBuildNo == NULL)
        return STATUS_INVALID_PARAMETER;

    NTSTATUS status = STATUS_SUCCESS;
    UNICODE_STRING strRegKey = RTL_CONSTANT_STRING(L"\\Registry\\Machine\\Software\\Microsoft\\Windows NT\\CurrentVersion");
    UNICODE_STRING strRegValue = RTL_CONSTANT_STRING(L"BuildLabEx");
    UNICODE_STRING strRegValue10 = RTL_CONSTANT_STRING(L"UBR");
    UNICODE_STRING strVerVal = { 0 };
    HANDLE hKey = NULL;
    OBJECT_ATTRIBUTES keyAttr = { 0 };

    InitializeObjectAttributes(&keyAttr, &strRegKey, OBJ_KERNEL_HANDLE | OBJ_CASE_INSENSITIVE, NULL, NULL);

    status = ZwOpenKey(&hKey, KEY_READ, &keyAttr);
    if (NT_SUCCESS(status))
    {
        PKEY_VALUE_FULL_INFORMATION pValueInfo = (PKEY_VALUE_FULL_INFORMATION)ExAllocatePoolWithTag(PagedPool, PAGE_SIZE, BB_POOL_TAG);
        ULONG bytes = 0;

        if (pValueInfo)
        {
            // Try query UBR value
            status = ZwQueryValueKey(hKey, &strRegValue10, KeyValueFullInformation, pValueInfo, PAGE_SIZE, &bytes);
            if (NT_SUCCESS(status))
            {
                *pBuildNo = *(PULONG)((PUCHAR)pValueInfo + pValueInfo->DataOffset);
                goto skip1;
            }

            status = ZwQueryValueKey(hKey, &strRegValue, KeyValueFullInformation, pValueInfo, PAGE_SIZE, &bytes);
            if (NT_SUCCESS(status))
            {
                PWCHAR pData = (PWCHAR)((PUCHAR)pValueInfo->Name + pValueInfo->NameLength);
                for (ULONG i = 0; i < pValueInfo->DataLength; i++)
                {
                    if (pData[i] == L'.')
                    {
                        for (ULONG j = i + 1; j < pValueInfo->DataLength; j++)
                        {
                            if (pData[j] == L'.')
                            {
                                strVerVal.Buffer = &pData[i] + 1;
                                strVerVal.Length = strVerVal.MaximumLength = (USHORT)((j - i) * sizeof(WCHAR));
                                status = RtlUnicodeStringToInteger(&strVerVal, 10, pBuildNo);

                                goto skip1;
                            }
                        }
                    }
                }

            skip1:;
            }

            ExFreePoolWithTag(pValueInfo, BB_POOL_TAG);
        }
        else
            status = STATUS_NO_MEMORY;

        ZwClose(hKey);
    }
    else
        DPRINT(" %s: ZwOpenKey failed with status 0x%X\n", __FUNCTION__, status);

    return status;

}


NTSTATUS BBInitDynamicData(IN OUT PDYNAMIC_DATA pData)
{
    NTSTATUS status = STATUS_SUCCESS;
    RTL_OSVERSIONINFOEXW verInfo = { 0 };

    if (pData == NULL)
        return STATUS_INVALID_ADDRESS;

    RtlZeroMemory(pData, sizeof(DYNAMIC_DATA));
    pData->DYN_PDE_BASE = PDE_BASE;
    pData->DYN_PTE_BASE = PTE_BASE;

    verInfo.dwOSVersionInfoSize = sizeof(verInfo);
    status = RtlGetVersion((PRTL_OSVERSIONINFOW)&verInfo);

    if (status == STATUS_SUCCESS)
    {
        ULONG ver_short = (verInfo.dwMajorVersion << 8) | (verInfo.dwMinorVersion << 4) | verInfo.wServicePackMajor;
        pData->ver = (WinVer)ver_short;

        // Get kernel build number
        status = BBGetBuildNO(&pData->buildNo);

        // Validate current driver version
        pData->correctBuild = TRUE;
#if defined(_WIN7_)
        if (ver_short != WINVER_7 && ver_short != WINVER_7_SP1)
            return STATUS_NOT_SUPPORTED;
#elif defined(_WIN8_)
        if (ver_short != WINVER_8)
            return STATUS_NOT_SUPPORTED;
#elif defined (_WIN81_)
        if (ver_short != WINVER_81)
            return STATUS_NOT_SUPPORTED;
#elif defined (_WIN10_)
        if (ver_short < WINVER_10 || WINVER_10_20H1 < ver_short)
            return STATUS_NOT_SUPPORTED;
#endif

        DPRINT(
            "OS version %d.%d.%d.%d.%d - 0x%x\n",
            verInfo.dwMajorVersion,
            verInfo.dwMinorVersion,
            verInfo.dwBuildNumber,
            verInfo.wServicePackMajor,
            pData->buildNo,
            ver_short
        );

        switch (ver_short)
        {
            // Windows 7
            // Windows 7 SP1
        case WINVER_7:
        case WINVER_7_SP1:
            pData->ProcessListEntry = 0xe0;

            pData->KExecOpt = 0x0D2;
            pData->UniquePId = 0x180;
            pData->ActiveProcessLink = pData->UniquePId + 0x8;
            pData->Protection = 0x43C;  // Bitfield, bit index - 0xB
            pData->ObjTable = 0x200;
            pData->VadRoot = 0x448;
            pData->NtProtectIndex = 0x04D;
            pData->NtCreateThdExIndex = 0x0A5;
            pData->NtTermThdIndex = 0x50;
            pData->DebugPort = 0x1f0;
            pData->PrevMode = 0x1F6;
            pData->ExitStatus = 0x380;
            pData->MiAllocPage = (ver_short == WINVER_7_SP1) ? 0 : 0;

            pData->ExRemoveTable = 0x32D404;
            break;

            // Windows 8
        case WINVER_8:
            pData->ProcessListEntry = 0x238;
            pData->DebugPort = 0x410;
            pData->KExecOpt = 0x1B7;
            pData->KExecOpt = 0x1BF;
            pData->UniquePId = 0x2e0;
            pData->ActiveProcessLink = pData->UniquePId + 0x8;
            pData->Protection = 0x648;
            pData->ObjTable = 0x408;
            pData->VadRoot = 0x590;
            pData->NtProtectIndex = 0x04E;
            pData->NtCreateThdExIndex = 0x0AF;
            pData->NtTermThdIndex = 0x51;
            pData->PrevMode = 0x232;
            pData->ExitStatus = 0x450;
            pData->MiAllocPage = 0x3AF374;
            pData->ExRemoveTable = 0x487518;
            break;

            // Windows 8.1
        case WINVER_81:
            pData->ProcessListEntry = 0x238;
            pData->DebugPort = 0x410;
            pData->KExecOpt = 0x1B7;
            pData->KExecOpt = 0x1BF;
            pData->UniquePId = 0x2e0;
            pData->ActiveProcessLink = pData->UniquePId + 0x8;
            pData->Protection = 0x67A;
            pData->EProcessFlags2 = 0x2F8;
            pData->ObjTable = 0x408;
            pData->VadRoot = 0x5D8;
            pData->NtCreateThdExIndex = 0xB0;
            pData->NtTermThdIndex = 0x52;
            pData->PrevMode = 0x232;
            pData->ExitStatus = 0x6D8;
            pData->MiAllocPage = 0;
            pData->ExRemoveTable = 0x432A88; // 0x38E320;
            break;

            // Windows 10, build 16299/15063/14393/10586
        case WINVER_10:
            if (verInfo.dwBuildNumber == 10586)
            {
                pData->ProcessListEntry = 0x240;
                pData->KExecOpt = 0x1BF;
                pData->UniquePId = 0x2e8;
                pData->ActiveProcessLink = pData->UniquePId + 0x8;
                pData->Protection = 0x6B2;
                pData->EProcessFlags2 = 0x300;
                pData->ObjTable = 0x418;
                pData->VadRoot = 0x610;
                pData->DebugPort = 0x420;
                pData->NtCreateThdExIndex = 0xB4;
                pData->NtTermThdIndex = 0x53;
                pData->PrevMode = 0x232;
                pData->ExitStatus = 0x6E0;
                pData->MiAllocPage = 0;
                break;
            }
            else if (verInfo.dwBuildNumber == 14393)
            {
                pData->ProcessListEntry = 0x240;
                pData->ver = WINVER_10_RS1;
                pData->KExecOpt = 0x1BF;
                pData->UniquePId = 0x2e8;
                pData->ActiveProcessLink = pData->UniquePId + 0x8;
                pData->Protection = pData->buildNo >= 447 ? 0x6CA : 0x6C2;
                pData->EProcessFlags2 = 0x300;
                pData->ObjTable = 0x418;
                pData->DebugPort = 0x420;
                pData->VadRoot = 0x620;
                pData->NtCreateThdExIndex = 0xB6;
                pData->NtTermThdIndex = 0x53;
                pData->PrevMode = 0x232;
                pData->ExitStatus = 0x6F0;
                pData->MiAllocPage = 0;
                break;
            }
            else if (verInfo.dwBuildNumber == 15063)
            {
                pData->ProcessListEntry = 0x240;
                pData->ver = WINVER_10_RS2;
                pData->KExecOpt = 0x1BF;
                pData->UniquePId = 0x2e0;
                pData->ActiveProcessLink = pData->UniquePId + 0x8;
                pData->Protection = 0x6CA;
                pData->EProcessFlags2 = 0x300;
                pData->ObjTable = 0x418;
                pData->DebugPort = 0x420;
                pData->VadRoot = 0x628;
                pData->NtCreateThdExIndex = 0xB9;
                pData->NtTermThdIndex = 0x53;
                pData->PrevMode = 0x232;
                pData->ExitStatus = 0x6F8;
                pData->MiAllocPage = 0;
                break;
            }
            else if (verInfo.dwBuildNumber == 16299)
            {
                pData->ProcessListEntry = 0x240;
                pData->ver = WINVER_10_RS3;
                pData->KExecOpt = 0x1BF;
                pData->UniquePId = 0x2e0;
                pData->ActiveProcessLink = pData->UniquePId + 0x8;
                pData->Protection = 0x6CA;
                pData->EProcessFlags2 = 0x828;    // MitigationFlags offset
                pData->DebugPort = 0x420;
                pData->ObjTable = 0x418;
                pData->VadRoot = 0x628;
                pData->NtCreateThdExIndex = 0xBA;
                pData->NtTermThdIndex = 0x53;
                pData->PrevMode = 0x232;
                pData->ExitStatus = 0x700;
                pData->MiAllocPage = 0;
                break;
            }
            else if (verInfo.dwBuildNumber == 17134)
            {
                pData->ProcessListEntry = 0x240;
                pData->ver = WINVER_10_RS4;
                pData->KExecOpt = 0x1BF;
                pData->UniquePId = 0x2e0;
                pData->ActiveProcessLink = pData->UniquePId + 0x8;
                pData->Protection = 0x6CA;
                pData->EProcessFlags2 = 0x828;    // MitigationFlags offset
                pData->ObjTable = 0x418;
                pData->DebugPort = 0x420;
                pData->VadRoot = 0x628;
                pData->NtCreateThdExIndex = 0xBB;
                pData->NtTermThdIndex = 0x53;
                pData->PrevMode = 0x232;
                pData->ExitStatus = 0x700;
                pData->MiAllocPage = 0;
                break;
            }
            else if (verInfo.dwBuildNumber == 17763)
            {
                pData->ProcessListEntry = 0x240;
                pData->ver = WINVER_10_RS5;
                pData->KExecOpt = 0x1BF;
                pData->UniquePId = 0x2e0;
                pData->ActiveProcessLink = pData->UniquePId + 0x8;
                pData->Protection = 0x6CA;
                pData->EProcessFlags2 = 0x820;    // MitigationFlags offset
                pData->ObjTable = 0x418;
                pData->DebugPort = 0x420;
                pData->VadRoot = 0x628;
                pData->NtCreateThdExIndex = 0xBC;
                pData->NtTermThdIndex = 0x53;
                pData->PrevMode = 0x232;
                pData->ExitStatus = 0x700;
                pData->MiAllocPage = 0;
                break;
            }
            else if (verInfo.dwBuildNumber == 18362 || verInfo.dwBuildNumber == 18363)
            {
                pData->ProcessListEntry = 0x248;

                pData->ver = verInfo.dwBuildNumber == 18362 ? WINVER_10_19H1 : WINVER_10_19H2;
                pData->KExecOpt = 0x1C3;
                pData->UniquePId = 0x2e8;
                pData->ActiveProcessLink = pData->UniquePId + 0x8;
                pData->Protection = 0x6FA;
                pData->EProcessFlags2 = 0x850;    // MitigationFlags offset
                pData->ObjTable = 0x418;
                pData->DebugPort = 0x420;
                pData->VadRoot = 0x658;
                pData->NtCreateThdExIndex = 0xBD;
                pData->NtTermThdIndex = 0x53;
                pData->PrevMode = 0x232;
                pData->ExitStatus = 0x710;
                pData->MiAllocPage = 0;
                break;
            }
            else if (verInfo.dwBuildNumber == 19041 || verInfo.dwBuildNumber == 19042 || verInfo.dwBuildNumber == 19043 || (verInfo.dwBuildNumber >= 22000 && verInfo.dwBuildNumber <= 22483))
            {
                pData->ver = verInfo.dwBuildNumber >= 22000 ? WINVER_11_21H2 : WINVER_10_20H1;
                // KP
                pData->KExecOpt = 0x283;
                pData->ProcessListEntry = 0x350;
                // EP
                pData->UniquePId = 0x440;
                pData->ActiveProcessLink = pData->UniquePId + 0x8;
                pData->Protection = 0x87A;
                pData->EProcessFlags2 = 0x9D4;    // MitigationFlags offset
                pData->ObjTable = 0x570;
                pData->VadRoot = 0x7D8;
                pData->DebugPort = 0x578;
                // KT
                pData->PrevMode = 0x232;
                // ET
                pData->ExitStatus = 0x548;
                // SSDT
                pData->NtCreateThdExIndex = 0xC1;
                pData->NtTermThdIndex = 0x53;
                pData->MiAllocPage = 0;
                break;
            }
            else
            {
                return STATUS_NOT_SUPPORTED;
            }
        default:
            break;
        }


        pData->correctBuild = TRUE;


        return (pData->VadRoot != 0 ? status : STATUS_INVALID_KERNEL_INFO_VERSION);
    }

    return status;
}

NTSTATUS InitDynamicData()
{

    auto status = BBInitDynamicData(&dynData);

    DPRINT("DebugPort:0x%x,Protection:0x%x\n", dynData.DebugPort, dynData.Protection);

    return status;
}

PHANDLE_TABLE_ENTRY ExpLookupHandleTableEntry(IN PHANDLE_TABLE HandleTable, IN EXHANDLE tHandle)
{
    ULONG_PTR i, j, k;
    ULONG_PTR CapturedTable;
    ULONG TableLevel;
    PHANDLE_TABLE_ENTRY Entry = NULL;
    EXHANDLE Handle;

    PUCHAR TableLevel1;
    PUCHAR TableLevel2;
    PUCHAR TableLevel3;

    ULONG_PTR MaxHandle;

    PAGED_CODE();

    Handle = tHandle;
    Handle.TagBits = 0;

    MaxHandle = *(volatile ULONG*)&HandleTable->NextHandleNeedingPool;
    if (Handle.Value >= MaxHandle)
    {
        return NULL;
    }

    CapturedTable = *(volatile ULONG_PTR*)&HandleTable->TableCode;
    TableLevel = (ULONG)(CapturedTable & LEVEL_CODE_MASK);
    CapturedTable = CapturedTable - TableLevel;

    switch (TableLevel)
    {
    case 0:
    {
        TableLevel1 = (PUCHAR)CapturedTable;

        Entry = (PHANDLE_TABLE_ENTRY)&TableLevel1[Handle.Value *
            (sizeof(HANDLE_TABLE_ENTRY) / HANDLE_VALUE_INC)];

        break;
    }

    case 1:
    {
        TableLevel2 = (PUCHAR)CapturedTable;

        i = Handle.Value % (LOWLEVEL_COUNT * HANDLE_VALUE_INC);
        Handle.Value -= i;
        j = Handle.Value / ((LOWLEVEL_COUNT * HANDLE_VALUE_INC) / sizeof(PHANDLE_TABLE_ENTRY));

        TableLevel1 = (PUCHAR) * (PHANDLE_TABLE_ENTRY*)&TableLevel2[j];
        Entry = (PHANDLE_TABLE_ENTRY)&TableLevel1[i * (sizeof(HANDLE_TABLE_ENTRY) / HANDLE_VALUE_INC)];

        break;
    }

    case 2:
    {
        TableLevel3 = (PUCHAR)CapturedTable;

        i = Handle.Value % (LOWLEVEL_COUNT * HANDLE_VALUE_INC);
        Handle.Value -= i;
        k = Handle.Value / ((LOWLEVEL_COUNT * HANDLE_VALUE_INC) / sizeof(PHANDLE_TABLE_ENTRY));
        j = k % (MIDLEVEL_COUNT * sizeof(PHANDLE_TABLE_ENTRY));
        k -= j;
        k /= MIDLEVEL_COUNT;

        TableLevel2 = (PUCHAR) * (PHANDLE_TABLE_ENTRY*)&TableLevel3[k];
        TableLevel1 = (PUCHAR) * (PHANDLE_TABLE_ENTRY*)&TableLevel2[j];
        Entry = (PHANDLE_TABLE_ENTRY)&TableLevel1[i * (sizeof(HANDLE_TABLE_ENTRY) / HANDLE_VALUE_INC)];

        break;
    }

    default: _assume(0);
    }

    return Entry;
}

//指定进程里所有指向目标PID的句柄全部降权 
NTSTATUS ChangeHandleAccessState(ULONG ulProcessId, ULONG ulProtectPid)
{

    PEPROCESS EProcess = NULL, ProtectEProcess = NULL;
    ULONG_PTR Handle = 0;
    PHANDLE_TABLE_ENTRY Entry = NULL;
    //HANDLE pointed addr
    PVOID Object = NULL;
    POBJECT_TYPE ObjectType = NULL;
    PHANDLE_TABLE HandleTable = NULL;

    if (!(NT_SUCCESS(PsLookupProcessByProcessId((HANDLE)ulProcessId, &EProcess)) && NT_SUCCESS(PsLookupProcessByProcessId((HANDLE)ulProtectPid, &ProtectEProcess))))
    {
        return STATUS_UNSUCCESSFUL;
    }

    for (Handle = 0;; Handle += HANDLE_VALUE_INC)
    {
        Entry = ExpLookupHandleTableEntry(*(PHANDLE_TABLE*)((PUCHAR)EProcess + dynData.ObjTable), *(PEXHANDLE)&Handle);

        HandleTable = *(PHANDLE_TABLE*)((PUCHAR)EProcess + dynData.ObjTable);


        if (Entry == NULL)
        {
            break;
        }

        *(ULONG_PTR*)&Object = Entry->ObjectPointerBits;
        *(ULONG_PTR*)&Object <<= 4;
        if (Object == NULL)
        {
            continue;
        }

        *(ULONG_PTR*)&Object |= 0xFFFF000000000000;
        *(ULONG_PTR*)&Object += 0x30;
        ObjectType = ObGetObjectType(Object);//Object=%llx, Entry=%llx, ObjectType=%llx\n======,Object, Entry, ObjectType
        //DbgPrint("Addr=%llx, Entry=%llX, Handle=%llX, QuotaProcess=%llX, HandleTable=%llX\n ", Object, Entry, Handle, HandleTable->QuotaProcess, HandleTable);
        //DbgPrint("HandleEprocess=%llx,ProtectEProcess=%llx\n", Object, ProtectEProcess);

        if (ObjectType == NULL)
        {
            continue;
        }



        //    //wcscpy(Buffer->szTypeName, *(PCWSTR*)((PUCHAR)ObjectType + 0x18));
        //    //Buffer->Handle = (HANDLE)Handle;
        //    //Buffer->AccessMask = Entry->GrantedAccessBits;
        //    //Buffer->Address = Object;//句柄指向的地址
        //    //Buffer++;

        if ((DWORD64)Object == (DWORD64)ProtectEProcess)
        {
            //Entry->GrantedAccessBits = NewAccessState;
            DPRINT("Found HANDLE, OriAccess:%x\n", Entry->GrantedAccessBits);
            if ((Entry->GrantedAccessBits & PROCESS_TERMINATE) == PROCESS_TERMINATE)
            {
                Entry->GrantedAccessBits &= ~PROCESS_TERMINATE;
            }
            if ((Entry->GrantedAccessBits & PROCESS_VM_OPERATION) == PROCESS_VM_OPERATION)
            {
                Entry->GrantedAccessBits &= ~PROCESS_VM_OPERATION;
            }
            if ((Entry->GrantedAccessBits & PROCESS_VM_READ) == PROCESS_VM_READ)
            {
                Entry->GrantedAccessBits &= ~PROCESS_VM_READ;
            }
            if ((Entry->GrantedAccessBits & PROCESS_VM_WRITE) == PROCESS_VM_WRITE)
            {
                Entry->GrantedAccessBits &= ~PROCESS_VM_WRITE;
            }
            DPRINT("New Access:%x\n", Entry->GrantedAccessBits);
        }

    }



    ObDereferenceObject(EProcess);
    ObDereferenceObject(ProtectEProcess);

    return STATUS_SUCCESS;
}

VOID SearchAndReduceAccessState(ULONG ulProtectPid)
{
    if (ulProtectPid == 0 )
        return;
    

    if(dynData.ObjTable == 0)
        DPRINT("%s: dynData.ObjTable = 0\n",__FUNCTION__);


    //这个函数弄了半个月 =_=
    NTSTATUS status;
    ULONG Retlength;
    PVOID Buffer = NULL;
    PSYSTEM_PROCESS_INFORMATION SystemProcess = NULL;
    status = ZwQuerySystemInformation(5, NULL, 0, &Retlength);
    if (status == STATUS_INFO_LENGTH_MISMATCH)
    {
        Buffer = ExAllocatePool(PagedPool, Retlength);
        if (Buffer)
        {
            RtlZeroMemory(Buffer, Retlength);
            status = ZwQuerySystemInformation(5, Buffer, Retlength, &Retlength);
            if (NT_SUCCESS(status))
            {
                SystemProcess = (PSYSTEM_PROCESS_INFORMATION)Buffer;
                do {
                    ChangeHandleAccessState((ULONG)SystemProcess->UniqueProcessId, ulProtectPid);
                    //DPRINT("%lld\n", SystemProcess->UniqueProcessId);
                    SystemProcess = (PSYSTEM_PROCESS_INFORMATION)(((ULONG64)SystemProcess) + SystemProcess->NextEntryOffset);

                } while (SystemProcess->NextEntryOffset);
            }
            ExFreePool(Buffer);
        }
    }

}


NTSTATUS EmptyDebugPort(ULONG ulPid)
{

    if (dynData.DebugPort == 0)
    {
        DPRINT("%s: Invalid DebugPort address\n", __FUNCTION__);
        return STATUS_INVALID_ADDRESS;
    }


    PEPROCESS pepro;
    if (NT_SUCCESS(PsLookupProcessByProcessId((HANDLE)ulPid, &pepro)))
    {
        //*(ULONG*)((DWORD64)pepro + dynData.Protection) = 0x40000051;//设置为r3不可访问

        __try
        {
            if (*(DWORD64*)((DWORD64)pepro + dynData.DebugPort) != 0)
            {
                //有人在调试!!!
                //memset(pepro, 0, 0x10000);//直接蓝屏

                *(DWORD64*)((DWORD64)pepro + dynData.DebugPort) = 0i64;
            }
        }
        __except (1)
        {
            DPRINT("%s: Exception\n", __FUNCTION__);
        }


    }

    if (pepro)
        ObDereferenceObject(pepro);
    return STATUS_SUCCESS;
}

NTSTATUS SetForbiddenAccess(ULONG ulPid)
{
    if (dynData.Protection == 0)
    {
        DPRINT("%s: Invalid psProtection address\n", __FUNCTION__);
        return STATUS_INVALID_ADDRESS;
    }

    PEPROCESS pepro;
    if (NT_SUCCESS(PsLookupProcessByProcessId((HANDLE)ulPid, &pepro)))
    {
        __try
        {
            *(ULONG*)((DWORD64)pepro + dynData.Protection) = 0x40000051;//设置为r3不可访问
        }
        __except (1)
        {
            DPRINT("%s: Exception\n", __FUNCTION__);
        }
    }

    if (pepro)
        ObDereferenceObject(pepro);
    return STATUS_SUCCESS;
}

NTSTATUS UnSetForbiddenAccess(ULONG ulPid)
{
    if (dynData.Protection == 0)
    {
        DPRINT("%s: Invalid psProtection address\n", __FUNCTION__);
        return STATUS_INVALID_ADDRESS;
    }

    PEPROCESS pepro;
    if (NT_SUCCESS(PsLookupProcessByProcessId((HANDLE)ulPid, &pepro)))
    {
        __try
        {
            *(ULONG*)((DWORD64)pepro + dynData.Protection) = 0x40000000;//设置为r3可访问
        }
        __except (1)
        {
            DPRINT("%s: Exception\n", __FUNCTION__);
        }
    }

    if (pepro)
        ObDereferenceObject(pepro);
    return STATUS_SUCCESS;
}

extern ULONG ProtectProcessPid;

OB_PREOP_CALLBACK_STATUS ObjectPreCallback(__in PVOID  RegistrationContext, __in POB_PRE_OPERATION_INFORMATION  OperationInformation)
{

    PEPROCESS Process;
    //UCHAR* pszImageName = NULL;
    HANDLE pid;

    Process = (PEPROCESS)OperationInformation->Object;
    //pszImageName = PsGetProcessImageFileName(Process);
    pid = PsGetProcessId(Process);

    if (ProtectProcessPid == NULL)
    {
        return OB_PREOP_SUCCESS;
    }

    //if (strstr((char*)pszImageName, ProtectProcessName) != NULL)
    if (pid == (HANDLE)ProtectProcessPid)
    {
        //DbgPrint("Operation:%d", OperationInformation->Operation);


        if (OperationInformation->Operation == OB_OPERATION_HANDLE_CREATE)
        {
            if ((OperationInformation->Parameters->CreateHandleInformation.OriginalDesiredAccess & PROCESS_TERMINATE) == PROCESS_TERMINATE)
            {
                OperationInformation->Parameters->CreateHandleInformation.DesiredAccess &= ~PROCESS_TERMINATE;
            }
            if ((OperationInformation->Parameters->CreateHandleInformation.OriginalDesiredAccess & PROCESS_VM_OPERATION) == PROCESS_VM_OPERATION)
            {
                OperationInformation->Parameters->CreateHandleInformation.DesiredAccess &= ~PROCESS_VM_OPERATION;
            }
            if ((OperationInformation->Parameters->CreateHandleInformation.OriginalDesiredAccess & PROCESS_VM_READ) == PROCESS_VM_READ)
            {
                OperationInformation->Parameters->CreateHandleInformation.DesiredAccess &= ~PROCESS_VM_READ;
            }
            if ((OperationInformation->Parameters->CreateHandleInformation.OriginalDesiredAccess & PROCESS_VM_WRITE) == PROCESS_VM_WRITE)
            {
                OperationInformation->Parameters->CreateHandleInformation.DesiredAccess &= ~PROCESS_VM_WRITE;
            }
        }

        if (OperationInformation->Operation == OB_OPERATION_HANDLE_DUPLICATE)
        {
            if ((OperationInformation->Parameters->CreateHandleInformation.OriginalDesiredAccess & PROCESS_TERMINATE) == PROCESS_TERMINATE)
            {
                OperationInformation->Parameters->CreateHandleInformation.DesiredAccess &= ~PROCESS_TERMINATE;
            }
            if ((OperationInformation->Parameters->CreateHandleInformation.OriginalDesiredAccess & PROCESS_VM_OPERATION) == PROCESS_VM_OPERATION)
            {
                OperationInformation->Parameters->CreateHandleInformation.DesiredAccess &= ~PROCESS_VM_OPERATION;
            }
            if ((OperationInformation->Parameters->CreateHandleInformation.OriginalDesiredAccess & PROCESS_VM_READ) == PROCESS_VM_READ)
            {
                OperationInformation->Parameters->CreateHandleInformation.DesiredAccess &= ~PROCESS_VM_READ;
            }
            if ((OperationInformation->Parameters->CreateHandleInformation.OriginalDesiredAccess & PROCESS_VM_WRITE) == PROCESS_VM_WRITE)
            {
                OperationInformation->Parameters->CreateHandleInformation.DesiredAccess &= ~PROCESS_VM_WRITE;
            }
        }

        DPRINT("New Access:%x\n", OperationInformation->Parameters->CreateHandleInformation.DesiredAccess);

    }
    return OB_PREOP_SUCCESS;
}

PVOID g_pRegistrationHandle;
NTSTATUS InstallObHookStatus;
void InstallObHook()
{

    OB_CALLBACK_REGISTRATION obReg;
    OB_OPERATION_REGISTRATION obOper;

    obOper.ObjectType = PsProcessType;
    obOper.Operations = OB_OPERATION_HANDLE_CREATE | OB_OPERATION_HANDLE_DUPLICATE;
    obOper.PreOperation = NULL;
    obOper.PostOperation = NULL;

    RtlInitUnicodeString(&obReg.Altitude, L"60000");
    obReg.Version = OB_FLT_REGISTRATION_VERSION;
    obReg.OperationRegistrationCount = 1;
    obReg.RegistrationContext = NULL;
    obOper.PreOperation = (POB_PRE_OPERATION_CALLBACK)&ObjectPreCallback;
    obReg.OperationRegistration = &obOper;

    InstallObHookStatus = ObRegisterCallbacks(&obReg, &g_pRegistrationHandle);
    if (!NT_SUCCESS(InstallObHookStatus))
        DPRINT("ObRegisterCallbacks failed%d\n", InstallObHookStatus);
}

void UninstallObHook()
{
    if (NT_SUCCESS(InstallObHookStatus))
    {
        ObUnRegisterCallbacks(g_pRegistrationHandle);
        DPRINT("ObRegisterCallbacks Unregistered\n");
    }
    else
        DPRINT("No CallBack Is Registered\n");
    
}


NTSTATUS SetProcessID(ULONG origPID, ULONG SetPID) {//用PsLookupProcessByProcessId会导致改不回来
    PEPROCESS pCurProcess = NULL;
    ULONG CurPID = 0;
    
    pCurProcess = PsGetCurrentProcess();//system process
    PLIST_ENTRY pActiveProcessLinks = (PLIST_ENTRY)((DWORD_PTR)pCurProcess + dynData.ActiveProcessLink);
    PLIST_ENTRY pNextLinks = pActiveProcessLinks->Flink;
    while (pNextLinks->Flink != pActiveProcessLinks->Flink)
    {        
        pCurProcess = (PEPROCESS)((DWORD_PTR)pNextLinks - dynData.ActiveProcessLink);
        CurPID = *(ULONG*)((DWORD_PTR)pCurProcess + dynData.UniquePId);
        if (CurPID == origPID) {
            *(ULONG*)((DWORD_PTR)pCurProcess + dynData.UniquePId) = SetPID;
            //DbgPrint("Found Pid and Change to[%d]\n", *(ULONG*)((DWORD_PTR)pCurProcess + PIDOffset));
            return STATUS_SUCCESS;
        }
        pNextLinks = pNextLinks->Flink;
    }
    //DbgPrint("Pid Not Found\n");
    return STATUS_NOT_FOUND;
}

VOID UnLinkProcesssList(PEPROCESS PE) {

    //ULONG KiProcessList = 0x240;
    //ULONG ExitTimeOffset = 0;


    LIST_ENTRY* ActiveProcessLink = (LIST_ENTRY*)((ULONG64)PE + dynData.ActiveProcessLink);
    if (ActiveProcessLink->Blink != 0 && ActiveProcessLink->Flink != 0) {
        ActiveProcessLink->Blink->Flink = ActiveProcessLink->Flink;
        ActiveProcessLink->Flink->Blink = ActiveProcessLink->Blink;
        ActiveProcessLink->Blink = 0;
        ActiveProcessLink->Flink = 0;
    }


    //switch (Version.dwBuildNumber)
    //{

    //case 17134: KiProcessList = 0x0240; break;
    //case 17763: KiProcessList = 0x0240; break;

    //case 18362:KiProcessList = 0x0248; break;
    //case 18363:KiProcessList = 0x0248; break;

    //default:KiProcessList = 0x350; break;

    //}

    if (dynData.ProcessListEntry!= 0)
    {
        LIST_ENTRY* KiProcessListHead = (LIST_ENTRY*)((ULONG64)PE + dynData.ProcessListEntry);
        if (KiProcessListHead->Blink != 0 && KiProcessListHead->Flink != 0) {
            KiProcessListHead->Blink->Flink = KiProcessListHead->Flink;
            KiProcessListHead->Flink->Blink = KiProcessListHead->Blink;
            KiProcessListHead->Blink = 0;
            KiProcessListHead->Flink = 0;
        }
    }

}

NTSTATUS HideProcess_BreakChain_NonPG(ULONG ulPID) {
    PEPROCESS Eprocess;
    if (NT_SUCCESS(PsLookupProcessByProcessId((HANDLE)ulPID, &Eprocess)))
    {
        UnLinkProcesssList(Eprocess);
        *(UCHAR*)((ULONG64)Eprocess - 0x15) = 0x4;//防PG

        return STATUS_SUCCESS;
    }

     return STATUS_NOT_FOUND;
}

