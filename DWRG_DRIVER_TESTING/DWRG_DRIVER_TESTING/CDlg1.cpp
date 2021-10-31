// CDlg1.cpp: 实现文件
//

#include "pch.h"
#include "CDlg1.h"
#include "afxdialogex.h"
#include "resource.h"

// CDlg1 对话框

IMPLEMENT_DYNAMIC(CDlg1, CDialogEx)

CDlg1::CDlg1(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG1, pParent)
{

}

CDlg1::~CDlg1()
{
}

void CDlg1::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlg1, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON3, &CDlg1::OnBnClickedButton3)
    ON_BN_CLICKED(IDC_BUTTON4, &CDlg1::OnBnClickedButton4)
    ON_BN_CLICKED(IDC_BUTTON5, &CDlg1::OnBnClickedButton5)
    ON_BN_CLICKED(IDC_BUTTON6, &CDlg1::OnBnClickedButton6)
    ON_BN_CLICKED(IDC_BUTTON7, &CDlg1::OnBnClickedButton7)
    ON_BN_CLICKED(IDC_BUTTON8, &CDlg1::OnBnClickedButton8)
END_MESSAGE_MAP()


// CDlg1 消息处理程序

extern HANDLE PipiDriverHandle;

void CDlg1::OnBnClickedButton3()//保护内存不被读写
{
    DWORD pid = GetDlgItemInt(IDC_EDIT2);
    DWORD bytes = 0;

    if (PipiDriverHandle == INVALID_HANDLE_VALUE)
    {
        printf("[-]请先加载驱动!!! Line=%d\n", __LINE__);
        return;
    }

    if (!DeviceIoControl(PipiDriverHandle, Pipi_RegisterCallBack, &pid, sizeof DWORD, nullptr, 0, &bytes, NULL))
    {
        int LastErr = GetLastError();
        printf("[-]驱动通讯:DeviceIoControl失败 Code=%d Line=%d\n", LastErr, __LINE__);
        if (LastErr == 0x6)
        {
            printf("[-]你是不是没加载驱动???!!!\n");
        }
    }
    else
        printf("[+]操作成功\n");
    
}


void CDlg1::OnBnClickedButton4()//取消保护内存不被读写
{
    DWORD pid = NULL;
    DWORD bytes = 0;

    if (PipiDriverHandle == INVALID_HANDLE_VALUE)
    {
        printf("[-]请先加载驱动!!! Line=%d\n", __LINE__);
        return;
    }

    if (!DeviceIoControl(PipiDriverHandle, Pipi_RegisterCallBack, &pid, sizeof DWORD, nullptr, 0, &bytes, NULL))
    {
        int LastErr = GetLastError();
        printf("[-]驱动通讯:DeviceIoControl失败 Code=%d Line=%d\n", LastErr, __LINE__);
        if (LastErr == 0x6)
        {
            printf("[-]你是不是没加载驱动???!!!\n");
        }
    }
    else
    {
        printf("[+]操作成功\n");

    }
}


void CDlg1::OnBnClickedButton5()
{
    DWORD pid = GetDlgItemInt(IDC_EDIT2);
    DWORD bytes = 0;

    if (PipiDriverHandle == INVALID_HANDLE_VALUE)
    {
        printf("[-]请先加载驱动!!! Line=%d\n", __LINE__);
        return;
    }

    if (!DeviceIoControl(PipiDriverHandle, Pipi_EscapeFromDebugger, &pid, sizeof DWORD, nullptr, 0, &bytes, NULL))
    {
        int LastErr = GetLastError();
        printf("[-]驱动通讯:DeviceIoControl失败 Code=%d Line=%d\n", LastErr, __LINE__);
        if (LastErr == 0x6)
        {
            printf("[-]你是不是没加载驱动???!!!\n");
        }
    }
    else
        printf("[+]操作成功\n");
    
}

typedef struct _HIDE_VAD
{
    ULONGLONG base;             // Region base address
    ULONGLONG size;             // Region size
    ULONG pid;                  // Target process ID
} HIDE_VAD, * PHIDE_VAD;
void CDlg1::OnBnClickedButton6()//隐藏内存区域
{
    //::MessageBoxW(0, L"未完成", L"未完成", 0);
    printf("[-]未完成!\n");
    return;

    HIDE_VAD hide;
    WCHAR basestr[30], sizestr[30];
    hide.pid = GetDlgItemInt(IDC_EDIT2);
    GetDlgItemTextW(IDC_EDIT1, basestr, 20);
    GetDlgItemTextW(IDC_EDIT3, sizestr, 20);
    hide.base = wcstoull(basestr, NULL, 16);
    hide.size = wcstoull(sizestr, NULL, 16);

    printf("Base:%llX size:%llX\n", hide.base, hide.size);

    DWORD bytes = 0;

    if (PipiDriverHandle == INVALID_HANDLE_VALUE)
    {
        printf("[-]请先加载驱动!!! Line=%d\n", __LINE__);
        return;
    }

    if (!DeviceIoControl(PipiDriverHandle, Pipi_VadHideMemory, &hide, sizeof HIDE_VAD, nullptr, 0, &bytes, NULL))
    {
        int LastErr = GetLastError();
        printf("[-]驱动通讯:DeviceIoControl失败 Code=%d Line=%d\n", LastErr, __LINE__);
        if (LastErr == 0x6)
        {
            printf("[-]你是不是没加载驱动???!!!\n");
        }
    }
    else
        printf("[+]操作成功\n");
}


void CDlg1::OnBnClickedButton7()//指定降权
{
    DWORD pid = GetDlgItemInt(IDC_EDIT2);
    DWORD bytes = 0;

    if (PipiDriverHandle == INVALID_HANDLE_VALUE)
    {
        printf("[-]请先加载驱动!!! Line=%d\n", __LINE__);
        return;
    }

    if (!DeviceIoControl(PipiDriverHandle, Pipi_LowerAllHandleAccess, &pid, sizeof DWORD, nullptr, 0, &bytes, NULL))
    {
        int LastErr = GetLastError();
        printf("[-]驱动通讯:DeviceIoControl失败 Code=%d Line=%d\n", LastErr, __LINE__);
        if (LastErr == 0x6)
        {
            printf("[-]你是不是没加载驱动???!!!\n");
        }
    }
    else
        printf("[+]操作成功\n");
}


void CDlg1::OnBnClickedButton8()//枚举所有句柄
{
    // TODO: 在此添加控件通知处理程序代码
    printf("[-]未完成!\n");
    return;
}
