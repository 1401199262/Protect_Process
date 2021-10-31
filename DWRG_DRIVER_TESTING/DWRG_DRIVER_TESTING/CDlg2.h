#pragma once


// CDlg2 对话框

class CDlg2 : public CDialogEx
{
	DECLARE_DYNAMIC(CDlg2)

public:
	CDlg2(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CDlg2();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG2 };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButton3();
	afx_msg void OnBnClickedButton4();
	afx_msg void OnBnClickedButton5();
	afx_msg void OnBnClickedButton7();
	afx_msg void OnBnClickedButton6();
};
