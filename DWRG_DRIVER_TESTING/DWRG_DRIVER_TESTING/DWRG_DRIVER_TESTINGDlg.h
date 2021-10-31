
// DWRG_DRIVER_TESTINGDlg.h: 头文件
//
#include"TabSheet.h"
#include"CDlg1.h"
#include"CDlg2.h"
#pragma once


// CDWRGDRIVERTESTINGDlg 对话框
class CDWRGDRIVERTESTINGDlg : public CDialogEx
{
// 构造
public:
	CDWRGDRIVERTESTINGDlg(CWnd* pParent = nullptr);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DWRG_DRIVER_TESTING_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton2();

private:
	CTabSheet m_tab;

	CDlg1 dlg1;
	CDlg2 dlg2;
public:
	afx_msg void OnBnClickedButton9();
};
