#include "Function.h"

#include "PipiDef.h"
#define DEVICE_NAME  L"\\Device\\SecureSys"
//设备与设备之间通信
#define DEVICE_LINK_NAME    L"\\DosDevices\\SecureSysLink"
//设备与Ring3之间通信 link 和name 不能一样!!!(蓝屏)

#define Pipi_RegisterCallBack		CTL_CODE(FILE_DEVICE_UNKNOWN,0x800,METHOD_BUFFERED,FILE_ANY_ACCESS)
#define Pipi_EscapeFromDebugger		CTL_CODE(FILE_DEVICE_UNKNOWN,0x801,METHOD_BUFFERED,FILE_ANY_ACCESS)
#define Pipi_VadHideMemory     		CTL_CODE(FILE_DEVICE_UNKNOWN,0x802,METHOD_BUFFERED,FILE_ANY_ACCESS)
#define Pipi_SetForbidAccessState	CTL_CODE(FILE_DEVICE_UNKNOWN,0x803,METHOD_BUFFERED,FILE_ANY_ACCESS)
#define Pipi_UnSetForbidAccessState	CTL_CODE(FILE_DEVICE_UNKNOWN,0x804,METHOD_BUFFERED,FILE_ANY_ACCESS)
#define Pipi_LowerAllHandleAccess	CTL_CODE(FILE_DEVICE_UNKNOWN,0x805,METHOD_BUFFERED,FILE_ANY_ACCESS)
#define Pipi_ChangePid           	CTL_CODE(FILE_DEVICE_UNKNOWN,0x806,METHOD_BUFFERED,FILE_ANY_ACCESS)
#define Pipi_BreakChainHideProcess  CTL_CODE(FILE_DEVICE_UNKNOWN,0x808,METHOD_BUFFERED,FILE_ANY_ACCESS)


//#define CTL_HideProcess					CTL_CODE(FILE_DEVICE_UNKNOWN,0x801,METHOD_BUFFERED,FILE_ANY_ACCESS)
//#define CTL_SetProcessID				CTL_CODE(FILE_DEVICE_UNKNOWN,0x802,METHOD_BUFFERED,FILE_ANY_ACCESS)
//#define CTL_SetForbidAccessStatus		CTL_CODE(FILE_DEVICE_UNKNOWN,0x803,METHOD_BUFFERED,FILE_ANY_ACCESS)
//#define CTL_UnSetForbidAccessStatus		CTL_CODE(FILE_DEVICE_UNKNOWN,0x804,METHOD_BUFFERED,FILE_ANY_ACCESS)
//#define CTL_EmptyDebugPort				CTL_CODE(FILE_DEVICE_UNKNOWN,0x805,METHOD_BUFFERED,FILE_ANY_ACCESS)
//#define CTL_ReadWriteMem				CTL_CODE(FILE_DEVICE_UNKNOWN,0x806,METHOD_BUFFERED,FILE_ANY_ACCESS)

BOOLEAN KillThread = FALSE;
//ULONG CurrentPid = 0;
ULONG ProtectProcessPid = NULL;
PETHREAD pEThread_EmptyDebugPortThr;// , pEThread_ChangeAccessState;



//TODO 可能不需要写成线程
//VOID EmptyDebugPortThr(PVOID lparam)
//{
//    LARGE_INTEGER timeout;
//    timeout.QuadPart = -10 * 1000 * 300;//Sleep(500)
//    //EumProcessByQueyInformation();
//    while (1)
//    {
//
//        KeDelayExecutionThread(KernelMode, 0, &timeout);
//
//        if (CurrentPid != 0)
//        {
//            EmptyDebugPort(CurrentPid);
//        }
//
//        if (KillThread)
//        {
//            DPRINT("Thread1 Terminated\n");
//            PsTerminateSystemThread(STATUS_SUCCESS);
//        }
//    }
//    
//}

//VOID CreateMyThread()
//{
//    HANDLE hSystemThr;
//    CLIENT_ID cid;
//    PsCreateSystemThread(&hSystemThr, 0, NULL, NULL, &cid, EmptyDebugPortThr, NULL);
//    if (hSystemThr)
//    {
//        PsLookupThreadByThreadId(cid.UniqueThread, (PETHREAD*)&pEThread_EmptyDebugPortThr);
//        ZwClose(hSystemThr);
//    }
//
//}



//派遣历程
NTSTATUS PassThroughDispatch(PDEVICE_OBJECT  DeviceObject, PIRP Irp)
{
    Irp->IoStatus.Status = STATUS_SUCCESS;     //LastError()
    Irp->IoStatus.Information = 0;             //ReturnLength
    IoCompleteRequest(Irp, IO_NO_INCREMENT);   //将Irp返回给Io管理器
    return STATUS_SUCCESS;
}
NTSTATUS ControlThroughDispatch(PDEVICE_OBJECT DeviceObject, PIRP Irp)
{
    NTSTATUS status = STATUS_SUCCESS;
    PIO_STACK_LOCATION irpStack;
    PVOID ioBuffer = NULL;
    ULONG inputBufferLength = 0;
    ULONG outputBufferLength = 0;
    ULONG ioControlCode = 0;

    UNREFERENCED_PARAMETER(DeviceObject);

    Irp->IoStatus.Status = STATUS_SUCCESS;
    Irp->IoStatus.Information = 0;

    irpStack = IoGetCurrentIrpStackLocation(Irp);
    ioBuffer = Irp->AssociatedIrp.SystemBuffer;
    inputBufferLength = irpStack->Parameters.DeviceIoControl.InputBufferLength;
    outputBufferLength = irpStack->Parameters.DeviceIoControl.OutputBufferLength;

    switch (irpStack->MajorFunction)
    {
        case IRP_MJ_DEVICE_CONTROL:
        {
            ioControlCode = irpStack->Parameters.DeviceIoControl.IoControlCode;

            switch (ioControlCode)
            {
                case Pipi_RegisterCallBack:
                    {
                        if (inputBufferLength >= sizeof( ULONG ) && ioBuffer)
                        {
                            __try
                            {
                                ProtectProcessPid = *(ULONG*)ioBuffer;
                            }
                            __except (1)
                            {
                                DPRINT("赋值ProtectProcessPid失败 Line=%d", __LINE__);
                                Irp->IoStatus.Status = STATUS_UNSUCCESSFUL;
                                break;
                            }

                            Irp->IoStatus.Status = STATUS_SUCCESS;
                        }  
                        else
                            Irp->IoStatus.Status = STATUS_INFO_LENGTH_MISMATCH;
                    }
                    break;

                case Pipi_EscapeFromDebugger:
                    {
                        if (inputBufferLength >= sizeof( ULONG ) && ioBuffer)
                        {                          
                            EmptyDebugPort(*(ULONG*)ioBuffer);
                            Irp->IoStatus.Status = STATUS_SUCCESS;
                        }  
                        else
                            Irp->IoStatus.Status = STATUS_INFO_LENGTH_MISMATCH;
                    }
                    break;

                case Pipi_VadHideMemory:
                    {
                        if (inputBufferLength >= sizeof(HIDE_VAD) && ioBuffer)
                        {
                            //HideVAD((PHIDE_VAD)ioBuffer);
                            Irp->IoStatus.Status = STATUS_SUCCESS;
                        }
                        else {
                            DPRINT("Pipi_VadHideMemory: STATUS_INFO_LENGTH_MISMATCH\n");
                            Irp->IoStatus.Status = STATUS_INFO_LENGTH_MISMATCH;
                        }
                    }
                    break;

                case Pipi_SetForbidAccessState:
                    {
                        if (inputBufferLength >= sizeof(ULONG) && ioBuffer)
                        {
                            SetForbiddenAccess(*(ULONG*)ioBuffer);
                            Irp->IoStatus.Status = STATUS_SUCCESS;
                        }
                        else
                            Irp->IoStatus.Status = STATUS_INFO_LENGTH_MISMATCH;
                    }
                    break;

                case Pipi_UnSetForbidAccessState:
                    {
                        if (inputBufferLength >= sizeof(ULONG) && ioBuffer)
                        {
                            UnSetForbiddenAccess(*(ULONG*)ioBuffer);
                            Irp->IoStatus.Status = STATUS_SUCCESS;
                        }
                        else
                            Irp->IoStatus.Status = STATUS_INFO_LENGTH_MISMATCH;
                    }
                    break; 

                case Pipi_LowerAllHandleAccess:
                    {
                        if (inputBufferLength >= sizeof(ULONG) && ioBuffer)
                        {
                            SearchAndReduceAccessState(*(ULONG*)ioBuffer);
                            Irp->IoStatus.Status = STATUS_SUCCESS;
                        }
                        else
                            Irp->IoStatus.Status = STATUS_INFO_LENGTH_MISMATCH;
                    }
                    break;

                case Pipi_ChangePid:
                    {
                        if (inputBufferLength >= sizeof(CHANGE_PID) && ioBuffer)
                        {
                            CHANGE_PID changes = *(CHANGE_PID*)ioBuffer;
                            SetProcessID(changes.Oripid, changes.Newpid);
                            Irp->IoStatus.Status = STATUS_SUCCESS;
                        }
                        else
                            Irp->IoStatus.Status = STATUS_INFO_LENGTH_MISMATCH;
                    }
                    break;

                case Pipi_BreakChainHideProcess:
                    {
                        if (inputBufferLength >= sizeof(ULONG) && ioBuffer)
                        {
                            HideProcess_BreakChain_NonPG(*(ULONG*)ioBuffer);
                            Irp->IoStatus.Status = STATUS_SUCCESS;
                        }
                        else
                            Irp->IoStatus.Status = STATUS_INFO_LENGTH_MISMATCH;
                    }
                    break;

                default:
                    DPRINT("%s: Unknown IRP_MJ_DEVICE_CONTROL 0x%X\n", __FUNCTION__, ioControlCode);
                    Irp->IoStatus.Status = STATUS_INVALID_PARAMETER;
                    break;
            }
        }
        break;
    }
    status = Irp->IoStatus.Status;
    IoCompleteRequest(Irp, IO_NO_INCREMENT);

    return status;
}


VOID DriverUnload(PDRIVER_OBJECT DriverObject) {

    UninstallObHook();
    //KillThread = TRUE;
    //KeWaitForSingleObject(pEThread_EmptyDebugPortThr, Executive, KernelMode, FALSE, NULL);
    //KeWaitForSingleObject(pEThread_ChangeAccessState, Executive, KernelMode, FALSE, NULL);

    UNICODE_STRING DeviceLinkName;
    PDEVICE_OBJECT NextDriver = NULL;
    PDEVICE_OBJECT DeleteDeviceObject = NULL;

    RtlInitUnicodeString(&DeviceLinkName, DEVICE_LINK_NAME);
    IoDeleteSymbolicLink(&DeviceLinkName);

    DeleteDeviceObject = DriverObject->DeviceObject;
    while (DeleteDeviceObject != NULL)
    {
        NextDriver = DeleteDeviceObject->NextDevice;
        IoDeleteDevice(DeleteDeviceObject);
        DeleteDeviceObject = NextDriver;
    }
    DPRINT("Driver Unload\n");
}


EXTERN_C NTSTATUS DriverEntry(PDRIVER_OBJECT DriverObject, PUNICODE_STRING pRegPath) {

    DPRINT("Driver Loaded\n");
    DriverObject->DriverUnload = DriverUnload;
    *((ULONG*)DriverObject->DriverSection + 26) |= 0x20ul;
    InstallObHook();
    if (!NT_SUCCESS(InitDynamicData()))
        DPRINT("Failed On Init!!!\n");

    //CreateMyThread();


    NTSTATUS Status = STATUS_SUCCESS;

    PDEVICE_OBJECT DeviceObject = NULL;
    UNICODE_STRING DeviceObjectName;
    UNICODE_STRING DeviceLinkName;

    RtlInitUnicodeString(&DeviceObjectName, DEVICE_NAME);

    Status = IoCreateDevice(DriverObject, NULL,
        &DeviceObjectName,
        FILE_DEVICE_UNKNOWN,
        0, FALSE,
        &DeviceObject);
    if (!NT_SUCCESS(Status))
        return Status;

    RtlInitUnicodeString(&DeviceLinkName, DEVICE_LINK_NAME);
    Status = IoCreateSymbolicLink(&DeviceLinkName, &DeviceObjectName);
    if (!NT_SUCCESS(Status))
    {
        IoDeleteDevice(DeviceObject);
        return Status;
    }


    for (ULONG i = 0; i < IRP_MJ_MAXIMUM_FUNCTION; i++)
    {
        DriverObject->MajorFunction[i] = PassThroughDispatch;
    }
    DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = ControlThroughDispatch;

    return Status;
}


