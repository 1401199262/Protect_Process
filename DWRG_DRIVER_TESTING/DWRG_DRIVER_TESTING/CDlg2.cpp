// CDlg2.cpp: 实现文件
//

#include "pch.h"
#include "CDlg2.h"
#include "afxdialogex.h"
#include "resource.h"

// CDlg2 对话框

IMPLEMENT_DYNAMIC(CDlg2, CDialogEx)

CDlg2::CDlg2(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG2, pParent)
{

}

CDlg2::~CDlg2()
{
}

void CDlg2::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlg2, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON3, &CDlg2::OnBnClickedButton3)
	ON_BN_CLICKED(IDC_BUTTON4, &CDlg2::OnBnClickedButton4)
	ON_BN_CLICKED(IDC_BUTTON5, &CDlg2::OnBnClickedButton5)
	ON_BN_CLICKED(IDC_BUTTON7, &CDlg2::OnBnClickedButton7)
    ON_BN_CLICKED(IDC_BUTTON6, &CDlg2::OnBnClickedButton6)
END_MESSAGE_MAP()


// CDlg2 消息处理程序

extern HANDLE PipiDriverHandle;

typedef struct _CHANGE_PID
{
    ULONG Oripid;
    ULONG Newpid;
} CHANGE_PID, * PCHANGE_PID;
void CDlg2::OnBnClickedButton3()//改pid
{
    CHANGE_PID changes = { 0 };
    changes.Oripid = GetDlgItemInt(IDC_EDIT2);
    changes.Newpid = GetDlgItemInt(IDC_EDIT3);

    DWORD bytes = 0;

    if (PipiDriverHandle == INVALID_HANDLE_VALUE)
    {
        printf("[-]请先加载驱动!!! Line=%d\n", __LINE__);
        return;
    }

    if (!DeviceIoControl(PipiDriverHandle, Pipi_ChangePid, &changes, sizeof CHANGE_PID, nullptr, 0, &bytes, NULL))
    {
        int LastErr = GetLastError();
        printf("[-]驱动通讯:DeviceIoControl失败 Code=%d Line=%d\n", LastErr, __LINE__);
        if (LastErr == 0x6)
        {
            printf("[-]你是不是没加载驱动???!!!\n");
        }
    }
    else
        printf("[+]操作成功 PID: %d --> %d\n", changes.Oripid, changes.Newpid);
}


void CDlg2::OnBnClickedButton4()//改pid隐藏进程
{
    CHANGE_PID changes = { 0 };
    changes.Oripid = GetDlgItemInt(IDC_EDIT2);
    changes.Newpid = 4;

    DWORD bytes = 0;

    if (PipiDriverHandle == INVALID_HANDLE_VALUE)
    {
        printf("[-]请先加载驱动!!! Line=%d\n", __LINE__);
        return;
    }

    if (!DeviceIoControl(PipiDriverHandle, Pipi_ChangePid, &changes, sizeof CHANGE_PID, nullptr, 0, &bytes, NULL))
    {
        int LastErr = GetLastError();
        printf("[-]驱动通讯:DeviceIoControl失败 Code=%d Line=%d\n", LastErr, __LINE__);
        if (LastErr == 0x6)
        {
            printf("[-]你是不是没加载驱动???!!!\n");
        }
    }
    else
        printf("[+]操作成功 PID: %d --> %d\n", changes.Oripid, changes.Newpid);
}


void CDlg2::OnBnClickedButton5()//断链
{
    ULONG pid = GetDlgItemInt(IDC_EDIT2);
    DWORD bytes = 0;

    if (PipiDriverHandle == INVALID_HANDLE_VALUE)
    {
        printf("[-]请先加载驱动!!! Line=%d\n", __LINE__);
        return;
    }

    if (!DeviceIoControl(PipiDriverHandle, Pipi_BreakChainHideProcess, &pid, sizeof ULONG, nullptr, 0, &bytes, NULL))
    {
        int LastErr = GetLastError();
        printf("[-]驱动通讯:DeviceIoControl失败 Code=%d Line=%d\n", LastErr, __LINE__);
        if (LastErr == 0x6)
        {
            printf("[-]你是不是没加载驱动???!!!\n");
        }
    }
    else
        printf("[+]操作成功 PID:%d\n", pid);
}

void CDlg2::OnBnClickedButton6()//应用层访问拒绝
{
    ULONG pid = GetDlgItemInt(IDC_EDIT2);
    DWORD bytes = 0;

    if (PipiDriverHandle == INVALID_HANDLE_VALUE)
    {
        printf("[-]请先加载驱动!!! Line=%d\n", __LINE__);
        return;
    }

    if (!DeviceIoControl(PipiDriverHandle, Pipi_SetForbidAccessState, &pid, sizeof ULONG, nullptr, 0, &bytes, NULL))
    {
        int LastErr = GetLastError();
        printf("[-]驱动通讯:DeviceIoControl失败 Code=%d Line=%d\n", LastErr, __LINE__);
        if (LastErr == 0x6)
        {
            printf("[-]你是不是没加载驱动???!!!\n");
        }
    }
    else
        printf("[+]操作成功 PID:%d\n", pid);
}

void CDlg2::OnBnClickedButton7()//取消应用层访问拒绝
{
    ULONG pid = GetDlgItemInt(IDC_EDIT2);
    DWORD bytes = 0;

    if (PipiDriverHandle == INVALID_HANDLE_VALUE)
    {
        printf("[-]请先加载驱动!!! Line=%d\n", __LINE__);
        return;
    }

    if (!DeviceIoControl(PipiDriverHandle, Pipi_UnSetForbidAccessState, &pid, sizeof ULONG, nullptr, 0, &bytes, NULL))
    {
        int LastErr = GetLastError();
        printf("[-]驱动通讯:DeviceIoControl失败 Code=%d Line=%d\n", LastErr, __LINE__);
        if (LastErr == 0x6)
        {
            printf("[-]你是不是没加载驱动???!!!\n");
        }
    }
    else
        printf("[+]操作成功 PID:%d\n", pid);
}



