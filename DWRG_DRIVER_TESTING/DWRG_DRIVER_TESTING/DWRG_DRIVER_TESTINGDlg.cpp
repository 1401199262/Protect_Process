// DWRG_DRIVER_TESTINGDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "DWRG_DRIVER_TESTING.h"
#include "DWRG_DRIVER_TESTINGDlg.h"
#include "afxdialogex.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CDWRGDRIVERTESTINGDlg 对话框
#pragma warning(disable:4996)
#include <Winsvc.h>
#include <strsafe.h>
#include <winioctl.h>
#define DRIVER_IMAGE_BASE_NAME         L"DRIVER_PROTECT.sys"
#define DEVICE_LINK_NAME    L"\\\\.\\SecureSysLink"
#define DRIVER_NAME        L"SecureSys"



WCHAR drvFullPath[MAX_PATH];
BOOL isSetPID = FALSE;
VOID startDebugWindow()
{
	HINSTANCE g_hInstance = 0;
	HANDLE g_hOutput = 0;
	HWND hwnd = NULL;
	HMENU hmenu = NULL;
	CHAR title[] = "不要点我啊,会卡住IO";
	HANDLE hdlWrite = NULL;

	AllocConsole();

	freopen("CONIN$", "r+t", stdin);
	freopen("CONOUT$", "w+t", stdout);
	g_hOutput = GetStdHandle(STD_OUTPUT_HANDLE);
	// 设置控制台窗口的属性
	SetConsoleTitleA(title);
	SetConsoleTextAttribute((HANDLE)g_hOutput, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
	// 屏蔽掉控制台窗口的关闭按钮，以防窗口被误删除，会让游戏也关闭
	while (NULL == hwnd)
	{
		hwnd = FindWindowA("ConsoleWindowClass", title);
	}
	hmenu = ::GetSystemMenu(hwnd, FALSE);
	//DeleteMenu(hmenu, SC_CLOSE, MF_BYCOMMAND);
	hdlWrite = GetStdHandle(STD_OUTPUT_HANDLE);

	DWORD mode;
	GetConsoleMode(g_hOutput, &mode);
	mode &= ~ENABLE_QUICK_EDIT_MODE;  //移除快速编辑模式
	mode &= ~ENABLE_INSERT_MODE;      //移除插入模式
	mode &= ~ENABLE_MOUSE_INPUT;
	SetConsoleMode(g_hOutput, mode);
	//SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
	MoveWindow(hwnd, 50, 50, 550, 420, true);
	// 这里也可以使用STD_ERROR_HANDLE    TCHAR c[] = {"Hello world!"};WriteConsole(hdlWrite, c, sizeof(c), NULL, NULL);
}


CDWRGDRIVERTESTINGDlg::CDWRGDRIVERTESTINGDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DWRG_DRIVER_TESTING_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CDWRGDRIVERTESTINGDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);

    DDX_Control(pDX, IDC_TAB2, m_tab);
}

BEGIN_MESSAGE_MAP(CDWRGDRIVERTESTINGDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON1, &CDWRGDRIVERTESTINGDlg::OnBnClickedButton1)
    ON_BN_CLICKED(IDC_BUTTON2, &CDWRGDRIVERTESTINGDlg::OnBnClickedButton2)

    ON_BN_CLICKED(IDC_BUTTON9, &CDWRGDRIVERTESTINGDlg::OnBnClickedButton9)
END_MESSAGE_MAP()
HMODULE GetSelfModuleHandle()
{
    MEMORY_BASIC_INFORMATION mbi;
    return ((::VirtualQuery(GetSelfModuleHandle, &mbi, sizeof(mbi)) != 0) ? (HMODULE)mbi.AllocationBase : NULL);
}
inline bool CheckFileExists(LPCWSTR name) {
    //#include <sys\stat.h>
    struct _stat64 buffer;
    return (_wstat64(name, &buffer) == 0);
}
WCHAR* GetCurrentPath(void)
{
    WCHAR path[MAX_PATH] = { 0 };
    memset(path, 0, sizeof(path));
    GetModuleFileName(GetSelfModuleHandle(), path, sizeof(path));
    int l = _tcslen(path);
    TCHAR ch = 0;
    while ('\\' != (ch = *(path + --l)));
    *(path + l + 1) = 0;
    return path;
}

// CDWRGDRIVERTESTINGDlg 消息处理程序
BOOL InitGlobals() {
    wcscpy(drvFullPath, GetCurrentPath());
    wcscat(drvFullPath, DRIVER_IMAGE_BASE_NAME);
    return TRUE;
}

BOOL CDWRGDRIVERTESTINGDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码

    //添加标签页
    m_tab.AddPage(_T("内存操作"), &dlg1, IDD_DIALOG1);
    m_tab.AddPage(_T("进程操作"), &dlg2, IDD_DIALOG2);
    m_tab.Show();

	startDebugWindow();
	InitGlobals();
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CDWRGDRIVERTESTINGDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CDWRGDRIVERTESTINGDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CDWRGDRIVERTESTINGDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


HANDLE PipiDriverHandle = INVALID_HANDLE_VALUE;
#pragma region Install_Uninstall_Driver_Stuff
// 安装驱动
BOOL installDvr() {
    if (!CheckFileExists(drvFullPath)) {
        MessageBoxW(0, L"未找到驱动", L"检测文件目录下有没有驱动", 0);
        return FALSE;
    }
    // 打开服务控制管理器数据库
    SC_HANDLE schSCManager = OpenSCManager(
        NULL,                   // 目标计算机的名称,NULL：连接本地计算机上的服务控制管理器
        NULL,                   // 服务控制管理器数据库的名称，NULL：打开 SERVICES_ACTIVE_DATABASE 数据库
        SC_MANAGER_ALL_ACCESS   // 所有权限
    );
    if (schSCManager == NULL)
        return FALSE;
    // 创建服务对象，添加至服务控制管理器数据库
    SC_HANDLE schService = CreateServiceW(
        schSCManager,               // 服务控件管理器数据库的句柄
        DRIVER_NAME,               // 要安装的服务的名称
        DRIVER_NAME,               // 用户界面程序用来标识服务的显示名称
        SERVICE_ALL_ACCESS,         // 对服务的访问权限：所有全权限
        SERVICE_KERNEL_DRIVER,      // 服务类型：驱动服务
        SERVICE_DEMAND_START,       // 服务启动选项：进程调用 StartService 时启动
        SERVICE_ERROR_NORMAL,       // 如果无法启动：忽略错误继续运行
        drvFullPath,                // 驱动文件绝对路径，如果包含空格需要多加双引号
        NULL,                       // 服务所属的负载订购组：服务不属于某个组
        NULL,                       // 接收订购组唯一标记值：不接收
        NULL,                       // 服务加载顺序数组：服务没有依赖项
        NULL,                       // 运行服务的账户名：使用 LocalSystem 账户
        NULL                        // LocalSystem 账户密码
    );
    if (schService == NULL) {
        CloseServiceHandle(schSCManager);
        return FALSE;
    }

    CloseServiceHandle(schService);
    CloseServiceHandle(schSCManager);
    return TRUE;
}

// 启动服务
BOOL startDvr() {

    // 打开服务控制管理器数据库
    SC_HANDLE schSCManager = OpenSCManager(
        NULL,                   // 目标计算机的名称,NULL：连接本地计算机上的服务控制管理器
        NULL,                   // 服务控制管理器数据库的名称，NULL：打开 SERVICES_ACTIVE_DATABASE 数据库
        SC_MANAGER_ALL_ACCESS   // 所有权限
    );
    if (schSCManager == NULL) {
        MessageBoxA(0, "打开服务控制管理器数据库Err", 0, 0);
        return FALSE;
    }

    // 打开服务
    SC_HANDLE hs = OpenService(
        schSCManager,           // 服务控件管理器数据库的句柄
        DRIVER_NAME,            // 要打开的服务名
        SERVICE_ALL_ACCESS      // 服务访问权限：所有权限
    );
    if (hs == NULL) {
        CloseServiceHandle(schSCManager);
        MessageBoxA(0, 0, "OpenServiceErr", 0);
        return FALSE;
    }
    if (!StartServiceW(hs, 0, 0)) {
        char lasterrchr[10] = "\0";
        DWORD lasterr = GetLastError();
        _itoa(lasterr, lasterrchr, 10);
        MessageBoxA(0, lasterrchr, "StartServiceErr", 0);
        CloseServiceHandle(hs);
        CloseServiceHandle(schSCManager);
        return FALSE;
    }


    CloseServiceHandle(hs);
    CloseServiceHandle(schSCManager);
    return TRUE;
}

// 停止服务
//BOOL stopDvr() {
//
//    // 打开服务控制管理器数据库
//    SC_HANDLE schSCManager = OpenSCManager(
//        NULL,                   // 目标计算机的名称,NULL：连接本地计算机上的服务控制管理器
//        NULL,                   // 服务控制管理器数据库的名称，NULL：打开 SERVICES_ACTIVE_DATABASE 数据库
//        SC_MANAGER_ALL_ACCESS   // 所有权限
//    );
//    if (schSCManager == NULL) {
//        MessageBoxA(0, "打开服务控制管理器数据库Err", 0, 0);
//        return FALSE;
//    }
//
//    // 打开服务
//    SC_HANDLE hs = OpenService(
//        schSCManager,           // 服务控件管理器数据库的句柄
//        DRIVER_NAME,            // 要打开的服务名
//        SERVICE_ALL_ACCESS      // 服务访问权限：所有权限
//    );
//    if (hs == NULL) {
//        MessageBoxA(0, "OpenServiceErr", 0, 0);
//        CloseServiceHandle(schSCManager);
//        return FALSE;
//    }
//
//    // 如果服务正在运行
//    SERVICE_STATUS status;
//    QueryServiceStatus(hs, &status);
//
//    if (status.dwCurrentState != SERVICE_STOPPED &&
//        status.dwCurrentState != SERVICE_STOP_PENDING
//        ) {
//        // 发送关闭服务请求
//        if (ControlService(
//            hs,                         // 服务句柄
//            SERVICE_CONTROL_STOP,       // 控制码：通知服务应该停止
//            &status                     // 接收最新的服务状态信息
//        ) == 0) {
//            CloseServiceHandle(hs);
//            CloseServiceHandle(schSCManager);
//            MessageBoxA(0, "ControlServiceErr", 0, 0);
//            return FALSE;
//        }
//
//        // 判断超时
//        int timeOut = 0;
//        while (status.dwCurrentState != SERVICE_STOPPED) {
//            timeOut++;
//            QueryServiceStatus(hs, &status);
//
//            Sleep(50);
//        }
//        if (timeOut > 10) {
//            CloseServiceHandle(hs);
//            CloseServiceHandle(schSCManager);
//            MessageBoxA(0, "KillDriverTimeout", 0, 0);
//            return FALSE;
//        }
//    }
//
//    CloseServiceHandle(hs);
//    CloseServiceHandle(schSCManager);
//    return TRUE;
//}
BOOL DrvCtl_Stop()
{
    CloseHandle(PipiDriverHandle);
    PipiDriverHandle = INVALID_HANDLE_VALUE;

    SC_HANDLE schSCManager;
    SC_HANDLE schService;
    SERVICE_STATUS ss;

    schSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
    if (schSCManager == NULL)
    {
        printf("OpenSCManager:%d\n", GetLastError());
        return FALSE;
    }

    schService = OpenService(schSCManager, DRIVER_NAME,
        SERVICE_STOP | SERVICE_QUERY_STATUS | SERVICE_ENUMERATE_DEPENDENTS);
    if (schService == NULL)
    {
        printf("OpenService:%d\n", GetLastError());
        CloseServiceHandle(schSCManager);
        return FALSE;
    }

    if (!ControlService(schService, SERVICE_CONTROL_STOP, &ss))
    {
        printf("ControlService:%d\n", GetLastError());
        printf("可能是驱动没卸载掉,可以尝试按卸载按钮\n这个有时候卸载不掉...关闭程序就能卸载了\n");

        CloseServiceHandle(schService);
        CloseServiceHandle(schSCManager);
        return FALSE;
    }

    //auto ControlServiceState = ControlService(schService, SERVICE_CONTROL_STOP, &ss);
    //int FailedCount = 0;
    //while (!ControlServiceState)
    //{
    //    FailedCount++;
    //    ControlServiceState=ControlService(schService, SERVICE_CONTROL_STOP, &ss);
    //    if (FailedCount > 50)
    //    {
    //        printf("ControlService:%d\n", GetLastError());
    //        CloseServiceHandle(schService);
    //        CloseServiceHandle(schSCManager);
    //        return FALSE;
    //    }
    //}

   

    CloseServiceHandle(schService);
    CloseServiceHandle(schSCManager);

    return TRUE;
}
// 卸载驱动
BOOL unloadDvr() {

    // 打开服务控制管理器数据库
    SC_HANDLE schSCManager = OpenSCManager(
        NULL,                   // 目标计算机的名称,NULL：连接本地计算机上的服务控制管理器
        NULL,                   // 服务控制管理器数据库的名称，NULL：打开 SERVICES_ACTIVE_DATABASE 数据库
        SC_MANAGER_ALL_ACCESS   // 所有权限
    );
    if (schSCManager == NULL) {
        return FALSE;
    }

    // 打开服务
    SC_HANDLE hs = OpenService(
        schSCManager,           // 服务控件管理器数据库的句柄
        DRIVER_NAME,            // 要打开的服务名
        SERVICE_ALL_ACCESS      // 服务访问权限：所有权限
    );
    if (hs == NULL) {
        CloseServiceHandle(schSCManager);
        return FALSE;
    }

    // 删除服务
    if (DeleteService(hs) == 0) {
        CloseServiceHandle(hs);
        CloseServiceHandle(schSCManager);
        return FALSE;
    }

    CloseServiceHandle(hs);
    CloseServiceHandle(schSCManager);

    return TRUE;
}
#pragma endregion



void CDWRGDRIVERTESTINGDlg::OnBnClickedButton1()//驱动启动
{
    if (installDvr() && startDvr())
    {
        printf("[+]驱动启动成功\n");
    }
    else
    {
        printf("[-]驱动启动失败\n");

    }
    CloseHandle(PipiDriverHandle);
    PipiDriverHandle = INVALID_HANDLE_VALUE;
    PipiDriverHandle = CreateFileW(DEVICE_LINK_NAME,
        GENERIC_READ | GENERIC_WRITE,
        FILE_SHARE_READ | FILE_SHARE_WRITE,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL);

    if (PipiDriverHandle == INVALID_HANDLE_VALUE)
    {

        int LastErr = GetLastError();
        printf("[-]获取驱动句柄:CreateFile失败 Code=%d Line=%d\n", LastErr, __LINE__);
        DrvCtl_Stop(); unloadDvr();
        if (LastErr == 0x5)
        {
            printf("[-]你是不是没管理员运行???!!!\n");
        }
        if (LastErr == 0x0)
        {
            printf("[-]点一下卸载驱动,可能是上次没卸载\n");
        }


    }

}


void CDWRGDRIVERTESTINGDlg::OnBnClickedButton2()//驱动关闭
{
    if (DrvCtl_Stop() && unloadDvr())
    {
        printf("[+]驱动关闭成功\n");
    }
    else
    {
        printf("[-]驱动关闭失败\n");

    }
}




void CDWRGDRIVERTESTINGDlg::OnBnClickedButton9()
{
    printf("进程和内存小工具--by Pipi\n");
    printf("第一页的功能,请输入原来的pid,即进程真正的pid\n");
    printf("第二页的功能,请输入更改过的pid,即进程被我们改过之后的pid\n");

    printf("功能还不完善,欢迎指出错误\n");
    printf("摸鱼群 875067223\n");

}
