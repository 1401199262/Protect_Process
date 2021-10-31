#pragma once
#include <ntifs.h>

typedef struct _HIDE_VAD
{
    ULONGLONG base;             // Region base address
    ULONGLONG size;             // Region size
    ULONG pid;                  // Target process ID
} HIDE_VAD, * PHIDE_VAD;
typedef struct _CHANGE_PID
{
    ULONG Oripid;
    ULONG Newpid;
} CHANGE_PID, * PCHANGE_PID;

void UninstallObHook();
void InstallObHook();
VOID SearchAndReduceAccessState(ULONG ulProtectPid);
NTSTATUS InitDynamicData();
NTSTATUS EmptyDebugPort(ULONG ulPid);
NTSTATUS HideVAD(IN PHIDE_VAD pData);
NTSTATUS SetForbiddenAccess(ULONG ulPid);
NTSTATUS UnSetForbiddenAccess(ULONG ulPid);
NTSTATUS SetProcessID(ULONG origPID, ULONG SetPID);
NTSTATUS HideProcess_BreakChain_NonPG(ULONG ulPID);