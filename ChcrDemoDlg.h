// ChcrDemoDlg.h : header file
//

#pragma once
#include "afxwin.h"
#include "PenDraw.h"
//#include "HwrRecog.h"

// CChcrDemoDlg dialog
class CChcrDemoDlg : public CDialog
{
// Construction
public:
	CChcrDemoDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_CHCRDEMO_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg void OnOK() {}
	afx_msg void OnCancel() {}
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnClose();

protected:
	// 用于指向手写区域
	CPenDraw *m_pPenDraw;
	CDC *m_pDC;
	// 设置临时点解决外边框有笔迹问题
	CPoint m_tempPrePoint;

	// 手写识别执行类
	//CHwrRecog *m_pHwrRecog;

	int m_nLang;
	unsigned short m_usEndWaitTime;
	unsigned short m_unRange;
	int m_nRecogString;
	CFileFind m_finder;

protected:
	CStatic m_ctrDrawArea;
	CListBox m_cbCandidate;
	CEdit m_cbText;

	CString m_csCurrentCandidate;

	// 切分候选
	//CListBox m_cbSegment;

	TCHAR m_szWorkPath[MAX_PATH];
	TCHAR m_szSampleDir[MAX_PATH];
	char m_szWorkPathA[MAX_PATH];
	
	TCHAR m_szPwtFile[MAX_PATH];

	char m_cCorrectRlt[256];//存储正确结果文件中的结果显示用

	char *m_pDatFileBuf;
	char *m_pGlobalRam;

protected:
	void PowerOn(BOOL on);
	void DisplayPoint(BOOL bMove);
	void UpdateEditText(CString csText);
	void ClearPoint();
	BOOL LoadFile();

public:
	afx_msg void OnDestroy();
public:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
public:
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
public:
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);

public:
	afx_msg void OnLbnSelchangeListCandidate();
public:
	afx_msg void OnLbnSelchangeListSegment();

public:
	afx_msg void OnEnChangeEditText();
//public:
//	afx_msg void OnBnClickedButtonStart();
public:
	afx_msg void OnBnClickedButtonClear();
public:
	afx_msg void OnLoadFile();
//protected:
//	CComboBox m_cbLang;
//public:
//	afx_msg void OnCbnSelchangeComboLang();
public:
	CButton m_pGoButton;
public:
	CEdit m_CalcTextOut;
public:
	afx_msg void OnBnClickedButtonGo();
public:
	afx_msg void OnBnClickedButtonSavetxt();
public:
	afx_msg void OnBnClickedButtonSavepwt();
public:
	afx_msg void OnBnClickedButtonNextpwt();
public:
	CEdit m_SampleFileName;
public:
	CEdit m_CorrectResult;
};
