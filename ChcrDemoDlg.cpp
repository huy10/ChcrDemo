/******************************************************************
/******************************************************************
* Function: A demo of handwritten mathematical calculator 
* Description: load data from PenDraw.cpp, call hmcInterface.cpp to analyse the input.
The output is displayed as a string which is stored and displayed in the demo
* Commit: Yejun Tang, Yang Hu
* Call:
* Call By:
******************************************************************/

#include "stdafx.h"
#include "ChcrDemo.h"
#include "ChcrDemoDlg.h"
#include "math.h"
#include "../HmcRecog/HmcInterface.h"
#include "../HmcRecog/hmcCommon.h"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <iomanip>
#include "../Calculator/overall.h"
using namespace std;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


#define WRITE_END_TIMER		1
#define COLOR_MODE_NUM		25

// temporary setting
#define ZOOM_POINT

// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

	// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CChcrDemoDlg dialog




CChcrDemoDlg::CChcrDemoDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CChcrDemoDlg::IDD, pParent)
	, m_pPenDraw(0)
	//, m_pHwrRecog(0) 
	//, m_nLang(1)
	, m_unRange(0x03)
	, m_usEndWaitTime(2000)
	, m_nRecogString(1)
	, m_csCurrentCandidate(_T(""))
	, m_pDatFileBuf(0)
	, m_pGlobalRam(0)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CChcrDemoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_DRAW_AREA, m_ctrDrawArea);
	DDX_Control(pDX, IDC_LIST_CANDIDATE, m_cbCandidate);
	//DDX_Control(pDX, IDC_LIST_SEGMENT, m_cbSegment);
	DDX_Control(pDX, IDC_EDIT_TEXT, m_cbText);
	//DDX_Control(pDX, IDC_COMBO_LANG, m_cbLang);
	DDX_Control(pDX, IDC_BUTTON1, m_pGoButton);
	DDX_Control(pDX, IDC_EDIT1, m_CalcTextOut);
	DDX_Control(pDX, IDC_EDIT_SAMFILENAME, m_SampleFileName);
	DDX_Control(pDX, IDC_EDIT_CORRLT, m_CorrectResult);
}

BEGIN_MESSAGE_MAP(CChcrDemoDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_WM_CLOSE()
	ON_WM_DESTROY()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_TIMER()
	ON_LBN_SELCHANGE(IDC_LIST_CANDIDATE, &CChcrDemoDlg::OnLbnSelchangeListCandidate)
	//ON_LBN_SELCHANGE(IDC_LIST_SEGMENT, &CChcrDemoDlg::OnLbnSelchangeListSegment)
	ON_EN_CHANGE(IDC_EDIT_TEXT, &CChcrDemoDlg::OnEnChangeEditText)
	//ON_BN_CLICKED(IDC_BUTTON1, &CChcrDemoDlg::OnBnClickedButtonStart)
	ON_BN_CLICKED(IDC_BUTTON2, &CChcrDemoDlg::OnBnClickedButtonClear)
	ON_COMMAND(IDM_LOAD_FILE, &CChcrDemoDlg::OnLoadFile)
	//ON_CBN_SELCHANGE(IDC_COMBO_LANG, &CChcrDemoDlg::OnCbnSelchangeComboLang)
	ON_BN_CLICKED(IDC_BUTTON1, &CChcrDemoDlg::OnBnClickedButtonGo)
	ON_BN_CLICKED(IDC_BUTTON_SAVETXT, &CChcrDemoDlg::OnBnClickedButtonSavetxt)
	ON_BN_CLICKED(IDC_BUTTON_SAVEPWT, &CChcrDemoDlg::OnBnClickedButtonSavepwt)
	ON_BN_CLICKED(IDC_BUTTON_NEXTPWT, &CChcrDemoDlg::OnBnClickedButtonNextpwt)
END_MESSAGE_MAP()


// CChcrDemoDlg message handlers

BOOL CChcrDemoDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here

	m_pPenDraw = new CPenDraw;
	ASSERT(m_pPenDraw);

	m_ctrDrawArea.SetWindowPos(&wndTopMost, 0, 0, 
		m_pPenDraw->m_unRight, m_pPenDraw->m_unBottom, 
		SWP_NOZORDER | SWP_NOMOVE);
	m_pDC = GetDC();

	// current directory
	GetCurrentDirectory(_MAX_PATH, m_szWorkPath);
	_tcscat(m_szWorkPath, _T("\\"));

	_stprintf(m_szSampleDir, _T("%ssamples"), m_szWorkPath);
	CreateDirectory(m_szSampleDir, NULL);

	GetCurrentDirectoryA(_MAX_PATH, m_szWorkPathA);
	strcat(m_szWorkPathA, "\\");

	////========================================
	//// Option
	//int nTmp;
	//FILE *fp = NULL;
	//if ((fp = _tfopen(_T("HwrDemo.option"), _T("rt"))) == NULL)
	//{
	//	m_unRange = 0x03;
	//	m_nLang = 1;
	//	if ((fp = _tfopen(_T("HwrDemo.option"), _T("wt"))) != NULL)
	//	{
	//		_ftprintf(fp, _T("language: %d\n"), m_nLang);
	//		_ftprintf(fp, _T("type: %d\n"), m_nRecogString);
	//		_ftprintf(fp, _T("range: %d\n"), (int)m_unRange);
	//		fclose(fp);
	//	}
	//}
	//else
	//{
	//	_ftscanf(fp, _T("language: %d\n"), &m_nLang);
	//	_ftscanf(fp, _T("type: %d\n"), &m_nRecogString);
	//	_ftscanf(fp, _T("range: %d\n"), &nTmp);
	//	m_unRange = (unsigned short)nTmp;
	//	fclose(fp);
	//}
	////========================================

	//// language selection
	//m_cbLang.ResetContent();
	//m_cbLang.InsertString(CHWRRECOG_LANG_CHINESE, _T("Chinese"));
	//m_cbLang.InsertString(CHWRRECOG_LANG_ENGLISH, _T("English"));
	//m_cbLang.InsertString(CHWRRECOG_LANG_FRENCH, _T("French"));
	//m_cbLang.InsertString(CHWRRECOG_LANG_GERMAN, _T("German"));
	//m_cbLang.InsertString(CHWRRECOG_LANG_ITALIAN, _T("Italian"));
	//m_cbLang.InsertString(CHWRRECOG_LANG_PORTUGUESE, _T("Portuguese"));
	//m_cbLang.InsertString(CHWRRECOG_LANG_SPANISH, _T("Spanish"));
	//m_cbLang.InsertString(CHWRRECOG_LANG_RUSSIAN, _T("Russian"));
	//m_cbLang.SetCurSel(m_nLang);

	// save file option
	((CButton *)GetDlgItem(IDC_CHECK_SAVEFILE))->SetCheck(0);

	CString csMsg;
	PowerOn(0);

	
	//m_pHwrRecog = new CHwrRecog(m_nLang);
	//ASSERT(m_pHwrRecog);

	//nErrCode = m_pHwrRecog->InitLib(m_unRange, m_nLang);
	//if (nErrCode)
	//{
	//	// disable dlg
	//	csMsg.Format(_T("InitLib() error: %d."), nErrCode);
	//	AfxMessageBox(csMsg);

	//	// release handwriting engine
	//	nErrCode = m_pHwrRecog->ReleaseLib();
	//	if (nErrCode)
	//	{
	//		csMsg.Format(_T("ReleaseLib() error: %d."), nErrCode);
	//		AfxMessageBox(csMsg);
	//	}
	//	delete m_pHwrRecog;
	//	m_pHwrRecog = 0;

	//	return FALSE;
	//}
	////========================================

	// engine initialization
	char szFileName[MAX_PATH];
	FILE *fp = 0;
	long lFileSize;

	// data file 
	sprintf(szFileName, "..\\WTPENpda.lib");//, m_szWorkPathA);

	if ((fp = fopen(szFileName, "rb")) == 0)
	{
		MessageBox(_T("data file haven't found\n"));
		return FALSE;
	}

	fseek(fp, 0, SEEK_END);
	lFileSize = ftell(fp);
	m_pDatFileBuf = new char [lFileSize];

	fseek(fp, 0, SEEK_SET);
	fread(m_pDatFileBuf, 1, lFileSize, fp);
	fclose(fp);
	fp = 0;

	// global RAM
	m_pGlobalRam = 0;
	m_pGlobalRam = new char [4096];

	// initialization interface
	HmcInit(m_pDatFileBuf, m_pGlobalRam);
	HmcSetRect(m_pPenDraw->m_unRight, m_pPenDraw->m_unBottom, m_pGlobalRam);

	m_szPwtFile[0] = _T('');

	PowerOn(1);
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CChcrDemoDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CChcrDemoDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}


	DisplayPoint(0);
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CChcrDemoDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CChcrDemoDlg::OnClose()
{
	CDialog::OnClose();
	DestroyWindow();
}

void CChcrDemoDlg::OnDestroy()
{
	CDialog::OnDestroy();

	ReleaseDC(m_pDC);

	if (m_pPenDraw)
	{
		delete m_pPenDraw;
	}
	if (m_pDatFileBuf)
	{
		delete [] m_pDatFileBuf;
		m_pDatFileBuf = 0;
	}
	if (m_pGlobalRam)
	{
		delete [] m_pGlobalRam;
		m_pGlobalRam = 0;
	}
}

void CChcrDemoDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	RECT rectDraw;
	m_ctrDrawArea.GetWindowRect(&rectDraw);
	ScreenToClient(&rectDraw); 

	if (/*m_pHwrRecog &&*/
		(point.y < rectDraw.bottom) && (point.y >= rectDraw.top) && 
		(point.x < rectDraw.right) && (point.x >= rectDraw.left))
	{
		KillTimer(WRITE_END_TIMER);
		if (!m_pPenDraw->m_bWriting)
		{
			ClearPoint();
		}
		m_pPenDraw->StartWriting();

		m_cbCandidate.ResetContent();
		m_csCurrentCandidate = "";
		//m_cbSegment.ResetContent();
		PowerOn(0);
		//SetCursor(m_hPenCursor); // ½«Êó±êÉèÎª±Ê

		m_pPenDraw->AddOnePoint((unsigned short)(point.x - rectDraw.left), 
			(unsigned char)(point.y - rectDraw.top));

		m_pDC->MoveTo(point);
		// ¼ÇÂ¼ÁÙÊ±µã
		m_tempPrePoint = point;
	}

	CDialog::OnLButtonDown(nFlags, point);
}

void CChcrDemoDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
	if (/*m_pHwrRecog && */m_pPenDraw->m_bPenDown) // Ð´×´Ì¬ÏÂ
	{
		//SetCursor(m_hPenCursor); // µ¯ÆðÒ²½«Êó±êÉèÎª±ÊÐÎ×´
		m_pPenDraw->EndOneStroke();

		SetTimer(WRITE_END_TIMER, (UINT)m_usEndWaitTime, NULL);

		//RECT rectWnd;
		//m_ctrDrawArea.GetWindowRect(&rectWnd);
		//ScreenToClient(&rectWnd);
	}

	CDialog::OnLButtonUp(nFlags, point);
}

void CChcrDemoDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	RECT rectDraw;
	m_ctrDrawArea.GetWindowRect(&rectDraw);
	ScreenToClient(&rectDraw);

	//if ((point.y < rectDraw.bottom) && (point.y >= rectDraw.top) &&
	//	(point.x < rectDraw.right) && (point.x >= rectDraw.left))
	//{
	//	SetCursor(m_hPenCursor); // ½«Êó±êÉèÎª±Ê
	//}

	if (/*m_pHwrRecog && */m_pPenDraw->m_bPenDown) 
	{

		if ((point.y < rectDraw.bottom) && (point.y >= rectDraw.top) &&
			(point.x < rectDraw.right) && (point.x >= rectDraw.left))
		{
			if ((m_tempPrePoint.y < rectDraw.bottom) && 
				(m_tempPrePoint.y >= rectDraw.top) &&
				(m_tempPrePoint.x < rectDraw.right) && 
				(m_tempPrePoint.x >= rectDraw.left))
			{

				m_pDC->LineTo(point);
			}

			m_pPenDraw->AddOnePoint((unsigned short)(point.x - rectDraw.left), 
				(unsigned short)(point.y - rectDraw.top));

			m_pDC->MoveTo(point);
		
			m_tempPrePoint = point;
		}
		else
		{
			m_pDC->MoveTo(point);

			m_tempPrePoint = point;
			OnLButtonUp(0, point);
		}
	}

	CDialog::OnMouseMove(nFlags, point);
}

void CChcrDemoDlg::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == WRITE_END_TIMER/* && m_pHwrRecog*/)
	{
		
		if (!m_pPenDraw->m_bPenDown) 
		{
			int i;
			short nErrCode;

			KillTimer(WRITE_END_TIMER);

			int nSave = ((CButton*)GetDlgItem(IDC_CHECK_SAVEFILE))->GetCheck();
			CTime t = CTime::GetCurrentTime();
			_stprintf(m_szPwtFile, _T("%s\\%04d%02d%02d%02d%02d%02d.pwt"), m_szSampleDir, 
				(int)t.GetYear(), (int)t.GetMonth(), (int)t.GetDay(), 
				(int)t.GetHour(), (int)t.GetMinute(), (int)t.GetSecond());
			m_pPenDraw->EndWriting(nSave, m_szPwtFile);

		
			ASSERT(m_pPenDraw->m_unPntNum <= CHCRDEMO_MAX_POINT_NUM);

			unsigned short uOriginalResult[512] = {0};
			//nErrCode = (short)HcRecog(szSampleFile, szCandResult, &nFactWordNum);
			nErrCode = HmcRecog(m_pPenDraw->m_pPntData, 
				m_pPenDraw->m_unPntNum, 5, 1, m_pGlobalRam, uOriginalResult);

			PowerOn(1);

		
			TCHAR *pszResult = (TCHAR *)&uOriginalResult[0];
			int nFactWordNum = 5;

			if (nErrCode)
			{
				nFactWordNum = 0;
				m_csCurrentCandidate = _T("");
				m_cbCandidate.ResetContent();
			}
			else
			{
				i = 0;
				while (pszResult[i])
				{
					if (pszResult[i] == 0xFFFF)
					{
						pszResult[i] = 0x0000;
						nFactWordNum++;
					}
					i++;
				}

				m_csCurrentCandidate = pszResult;
				for (i = 0; i < nFactWordNum; i++)
				{
					m_cbCandidate.InsertString(i, pszResult);
					pszResult += _tcslen(pszResult) + 1;
				}
			}
		
			m_cCorrectRlt[0] = 0;
			unsigned short uRlt[1] = {0};
			CString csText;
			csText.Format(_T("%s"),uRlt);
			m_CorrectResult.SetWindowText(csText);

			// ½«Ê×Ñ¡Êä³öµ½±à¼­¿ò
			//CString csOldText;
			//m_cbText.GetWindowText(csOldText);
			//csOldText += L" ";
			UpdateEditText(m_csCurrentCandidate);

			OnBnClickedButtonGo();
			//// ¸ø³öÇÐ·ÖºòÑ¡
			//if (nErrCode)
			//{
			//	m_cbSegment.ResetContent();
			//}
			//else
			//{
			//	CString csSeg;
			//	for (i = 0; i < m_pHwrRecog->m_nSegCandNum; i++)
			//	{
			//		csSeg.Format(_T("%d"), i+1);
			//		m_cbSegment.InsertString(i, csSeg);
			//	}
			//	m_cbSegment.SetCurSel(-1);
			//}

			// error message
			if (nErrCode)
			{
				CString csMsg;
				csMsg.Format(_T("Recognize() error: %d."), nErrCode);
				AfxMessageBox(csMsg);
			}

			UpdateData(FALSE);
			Invalidate(1);
		}
	}

	CDialog::OnTimer(nIDEvent);
}

void CChcrDemoDlg::ClearPoint()
{
	m_pPenDraw->Reset();
//	m_pHwrRecog->m_nSegCandNum = 0;

	m_ctrDrawArea.RedrawWindow();
	//((CButton*)GetDlgItem(IDC_DRAW_CLS))->EnableWindow(FALSE);
	UpdateData(FALSE);
	Invalidate();
}

void CChcrDemoDlg::PowerOn(BOOL on)
{
	m_cbCandidate.EnableWindow(on);
	//m_cbSegment.EnableWindow(on);
	//m_cbText.EnableWindow(on);

}


void CChcrDemoDlg::DisplayPoint(BOOL bMove)
{
	if (m_pPenDraw->m_unPntNum <= 2)
	{
		return;
	}

	short i, j;
	RECT rectWnd;
	CPoint point;
	m_ctrDrawArea.GetWindowRect(&rectWnd);
	ScreenToClient(&rectWnd); 	


	m_ctrDrawArea.RedrawWindow();


	unsigned short *pPointData = m_pPenDraw->m_pPntData;
	BOOL bEndOfStroke = TRUE;
	CPen *pOldPen;

	int nCharIdPerStroke[HCR_MAX_STROKE];
	int nStrokeNum = 0;

	if (!m_pPenDraw->m_bWriting)
	{
		// it just means that stroke_number > 1
		nStrokeNum = 1;

		
		PSPLITPATHHCR psp = (PSPLITPATHHCR)(m_pGlobalRam + HCR_RAM_OFFSET_FOR_SEG);
		for (i = 0; i < psp->nGroupNum; i++)
		{
			for (j = psp->group[i].nStrokeBe; j <= psp->group[i].nStrokeEn; j++)
			{
				nCharIdPerStroke[j] = i;
			}
		}
	}

#ifdef ZOOM_POINT
	unsigned short uLeft = 0xffff;
	unsigned short uTop = 0xffff;
	while (*(pPointData + 1) != 0xffff)
	{
		if (*pPointData == 0xffff)
		{
			pPointData += 2;
			continue;
		}
		if (*pPointData < uLeft)
		{
			uLeft = *pPointData;
		}
		if (*(pPointData+1) < uTop)
		{
			uTop = *(pPointData+1);
		}
		pPointData += 2;
	}
	pPointData = m_pPenDraw->m_pPntData;
#endif

	if (nStrokeNum <= 0)
	{

		CPen pntPen(PS_SOLID, 1, RGB(255,0,0));
		pOldPen = m_pDC->SelectObject(&pntPen);

		while (*(pPointData + 1) != 0xffff)
		{
			if (*pPointData == 0xffff)
			{
	
				pPointData += 2;
				bEndOfStroke = TRUE;

				if (bMove)
				{
					
					Sleep(400);
				}

				continue;
			}

#ifdef ZOOM_POINT
			point.x = (long)(*pPointData - uLeft) / 2 + rectWnd.left + 2;
			point.y = (long)(*(pPointData+1) - uTop) / 2 + rectWnd.top + 2;
#else
			point.x = (long)(*pPointData) + rectWnd.left + 2;
			point.y = (long)(*(pPointData + 1)) + rectWnd.top + 2;
#endif

			if (bMove)
			{
	
				Sleep(20);
			}

			if (bEndOfStroke)
			{
				m_pDC->MoveTo(point);
				bEndOfStroke = FALSE;
			}
			else
			{
				m_pDC->LineTo(point);
				m_pDC->MoveTo(point);
			}
			pPointData += 2;
		}

		m_pDC->LineTo(point);
		m_pDC->SelectObject(pOldPen);
		pntPen.DeleteObject();
	}
	else
	{
		
		const COLORREF ColorIndex[] = {
			RGB(255,0,0),
			RGB(0,0,255),
			RGB(0,255,255),
			RGB(255,0,255),
			RGB(0,0,0),
			RGB(128,0,255),
			RGB(0,255,0),
			RGB(128,64,0)
		};

	
		int i = 0;
		int j = nCharIdPerStroke[i];
		CPen pntPen(PS_SOLID, 1, ColorIndex[j%8]);
		pOldPen = m_pDC->SelectObject(&pntPen);

		while (*(pPointData + 1) != 0xffff)
		{
			if (*pPointData == 0xffff)
			{
				pPointData += 2;
				bEndOfStroke = TRUE;

				//m_pDC->LineTo(point);
				m_pDC->SelectObject(pOldPen);
				pntPen.DeleteObject();

				i++;
				j = nCharIdPerStroke[i];
				pntPen.CreatePen(PS_SOLID, 1, ColorIndex[j%8]);
				pOldPen = m_pDC->SelectObject(&pntPen);

				if (bMove)
				{
					
					Sleep(400);
				}

				continue;
			}

#ifdef ZOOM_POINT
			point.x = (long)(*pPointData - uLeft) / 2 + rectWnd.left + 2;
			point.y = (long)(*(pPointData+1) - uTop) / 2 + rectWnd.top + 2;
#else
			point.x = (long)(*pPointData) + rectWnd.left + 2;
			point.y = (long)(*(pPointData + 1)) + rectWnd.top + 2;
#endif

			if (bMove)
			{
				
				Sleep(20);
			}

			if (bEndOfStroke)
			{
				m_pDC->MoveTo(point);
				bEndOfStroke = FALSE;
			}
			else
			{
				m_pDC->LineTo(point);
				m_pDC->MoveTo(point);
			}
			pPointData += 2;
		}

		m_pDC->LineTo(point);
		m_pDC->SelectObject(pOldPen);
		pntPen.DeleteObject();
	}
}

void CChcrDemoDlg::OnLbnSelchangeListCandidate()
{
	CString csNewCandidate;
	if (m_cbCandidate.GetCurSel() == LB_ERR)
	{
		return;
	}
	m_cbCandidate.GetText(m_cbCandidate.GetCurSel(), csNewCandidate);
	int oldCandLen = m_csCurrentCandidate.GetLength();
	CString csText;
	m_cbText.GetWindowText(csText);
	if (csText.GetLength() >= oldCandLen && 
		csText.Right(oldCandLen) == m_csCurrentCandidate)
	{
		csText.Delete(csText.GetLength() - oldCandLen, oldCandLen);
		m_csCurrentCandidate = csNewCandidate;
		UpdateEditText(csText + m_csCurrentCandidate);
	}
}

void CChcrDemoDlg::OnLbnSelchangeListSegment()
{
	//m_pHwrRecog->m_unCurrentWordCand = m_cbSegment.GetCurSel();
	//Invalidate(1);
}

void CChcrDemoDlg::UpdateEditText(CString csText)
{
	m_cbText.SetWindowText(csText);
	//m_cbText.SetFocus();
	int len = csText.GetLength();
	//m_cbText.SetSel(len, len);
}

void CChcrDemoDlg::OnEnChangeEditText()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
}

void CChcrDemoDlg::OnBnClickedButtonClear()
{
	// TODO: Add your control notification handler code here
	KillTimer(WRITE_END_TIMER);
	ClearPoint();
}

void CChcrDemoDlg::OnLoadFile()
{
	CFileDialog fileDlg(TRUE, _T("pwt"), _T("*.pwt"), NULL, 
		_T("Point Data Files(*.pwt) | All Files (*.*) ||"), NULL);
	fileDlg.m_ofn.lpstrTitle = _T("Please select pwt file...");
	if (fileDlg.DoModal() != IDOK)
	{
		return;
	}

	CString csFileName = fileDlg.GetPathName();
	_tcscpy(m_szPwtFile, csFileName.GetBuffer());
	LoadFile();

	//for next sample
	BOOL bWorking = m_finder.FindFile(_T("*.pwt"));
   	while (bWorking)
	{
		bWorking = m_finder.FindNextFile();
		CString a = m_finder.GetFilePath();

		if (a == csFileName)
		{
			break;
		}
	}
	((CButton*)(GetDlgItem(IDC_BUTTON_NEXTPWT)))->EnableWindow(1);

//for correct result
	TCHAR ResultFile[MAX_PATH];
	
	_stprintf(ResultFile, _T("%stxt"),csFileName.Left(csFileName.GetLength()-3));
	FILE *fp = _tfopen(ResultFile, _T("rb"));
	if (fp == NULL)
	{
		m_cCorrectRlt[0] = 0;
	}else
	{
		fscanf(fp, "%s\n", m_cCorrectRlt);
		fclose(fp);
	}
//end

	int npos=0, nLastPos[2]={0,0};
	while(npos != -1)
	{
		nLastPos[1] = nLastPos[0];
		nLastPos[0] = npos;
		npos = csFileName.Find(_T("\\"), npos+1);
	}

	csFileName = csFileName.Right(csFileName.GetLength()-nLastPos[1]-1);

	m_SampleFileName.SetWindowText(csFileName);
//for list correct result
	CString csText;
	unsigned short uRlt[256] = {0,};
	int i = 0;
	while (m_cCorrectRlt[i]!= 0)
	{
		uRlt[i] = (unsigned short)(m_cCorrectRlt[i]);
		i++;
	}
	uRlt[i] = 0; 
	csText.Format(_T("%s"),uRlt);
	m_CorrectResult.SetWindowText(csText);

	


}

BOOL CChcrDemoDlg::LoadFile()
{
	int i;
	short nErrCode;

	FILE *fp = _tfopen(m_szPwtFile, _T("rb"));
	if (!fp)
	{
		m_szPwtFile[0] = 0;
		return FALSE;
	}

	ClearPoint();
	m_cbCandidate.ResetContent();
	m_csCurrentCandidate = "";
	//m_cbSegment.ResetContent();
	PowerOn(0);


	pwtheader pwtHeader;
	fread(&pwtHeader, sizeof(pwtheader), 1, fp);
	ASSERT(pwtHeader.patternlen > sizeof(pwtheader) + 4);
	m_pPenDraw->m_unPntNum = 
		(pwtHeader.patternlen - sizeof(pwtheader)) / 
		(sizeof(unsigned short) * 2);
	ASSERT(m_pPenDraw->m_unPntNum <= CHCRDEMO_MAX_POINT_NUM);

	fread(m_pPenDraw->m_pPntData, sizeof(unsigned short), 
		m_pPenDraw->m_unPntNum * 2, fp);
	ASSERT(m_pPenDraw->m_pPntData[m_pPenDraw->m_unPntNum * 2 - 2] == 0xffff);
	ASSERT(m_pPenDraw->m_pPntData[m_pPenDraw->m_unPntNum * 2 - 1] == 0xffff);

	fclose(fp);

	unsigned short uOriginalResult[512] = {0};
	nErrCode = HmcRecog(m_pPenDraw->m_pPntData,
		m_pPenDraw->m_unPntNum, 5, 1, m_pGlobalRam, uOriginalResult);

	PowerOn(1);

	TCHAR *pszResult = (TCHAR *)&uOriginalResult[0];
	int nFactWordNum = 5;

	if (nErrCode)
	{
		nFactWordNum = 0;
		m_csCurrentCandidate = _T("");
		m_cbCandidate.ResetContent();
	}
	else
	{
		i = 0;
		while (pszResult[i])
		{
			if (pszResult[i] == 0xFFFF)
			{
				pszResult[i] = 0x0000;
				nFactWordNum++;
			}
			i++;
		}

		m_csCurrentCandidate = pszResult;
		for (i = 0; i < nFactWordNum; i++)
		{
			m_cbCandidate.InsertString(i, pszResult);
			pszResult += _tcslen(pszResult) + 1;
		}
	}


	//CString csOldText;
	//m_cbText.GetWindowText(csOldText);
	//csOldText += L" ";
	UpdateEditText(m_csCurrentCandidate);
    OnBnClickedButtonGo();
	// error message
	if (nErrCode)
	{
		CString csMsg;
		csMsg.Format(_T("Recognize() error: %d."), nErrCode);
		AfxMessageBox(csMsg);
	}

	UpdateData(FALSE);
	Invalidate(1);
	return TRUE;
}

void CChcrDemoDlg::OnBnClickedButtonGo()
{
	//m_CalcTextOut.SetWindowText(_T("Hello!"));
	CString cstrExpr;
	m_cbText.GetWindowText(cstrExpr);
	string strExpr;
	strExpr.resize(cstrExpr.GetLength());
	for (int i = 0; i < cstrExpr.GetLength(); ++i)
	{
		strExpr[i] = (char)cstrExpr[i];
	}
	double ans(0.0);
	ofstream ofs("errMsg.txt");
	streambuf *errBuf = cerr.rdbuf(ofs.rdbuf());
	if (overall(strExpr, ans))
	{
		string strErrMsg;
		TCHAR tszErrMsg[BUFSIZ] = {0};
		ifstream ifs("errMsg.txt");
		getline(ifs, strErrMsg);
		for (int i = 0; i < (int)strErrMsg.length(); ++i)
		{
			tszErrMsg[i] = (TCHAR)strErrMsg[i];
		}
		m_CalcTextOut.SetWindowText(tszErrMsg);
	}
	else
	{
		ostringstream ossAns;
		ossAns << setprecision(10) << ans;
		string strAns = ossAns.str();
		TCHAR tszAns[BUFSIZ] = {0};
		for (int i = 0; i < (int)strAns.length(); ++i)
		{
			tszAns[i] = (TCHAR)strAns[i];
		}
		m_CalcTextOut.SetWindowText(tszAns);
	}
	cerr.rdbuf(errBuf);
	m_CalcTextOut.UpdateData();
}

void CChcrDemoDlg::OnBnClickedButtonSavetxt()
{
	if (m_szPwtFile[0] <= 4)
	{
		return;
	}

	CString csText;
	m_cbText.GetWindowText(csText);
	if (csText.GetLength() <= 0)
	{
		return;
	}

	size_t nFileNameLen = _tcslen(m_szPwtFile);
	if (_tcscmp(&m_szPwtFile[nFileNameLen-4], _T(".pwt")) != 0)
	{
		return;
	}

	TCHAR szResFile[MAX_PATH];
	_tcscpy(szResFile, m_szPwtFile);
	_tcscpy(&szResFile[nFileNameLen-4], _T(".txt"));

	FILE *fpRes = _tfopen(szResFile, _T("wt"));
	char szText[256];
	int i;
	for (i = 0; i < csText.GetLength(); i++)
	{
		szText[i] = (char)csText[i];
	}
	szText[i] = 0;
	fprintf(fpRes, "%s\n", szText);
	fclose(fpRes);
}

void CChcrDemoDlg::OnBnClickedButtonSavepwt()
{
	CTime t = CTime::GetCurrentTime();
	_stprintf(m_szPwtFile, _T("%s\\%04d%02d%02d%02d%02d%02d.pwt"), m_szSampleDir, 
		(int)t.GetYear(), (int)t.GetMonth(), (int)t.GetDay(), 
		(int)t.GetHour(), (int)t.GetMinute(), (int)t.GetSecond());
	m_pPenDraw->SavePwtFile(m_szPwtFile);
}

void CChcrDemoDlg::OnBnClickedButtonNextpwt()
{
	
	BOOL bWorking = m_finder.FindNextFile();

	
	{
		CString csCurFile = m_finder.GetFilePath();
		TCHAR *a ;
		a = csCurFile.GetBuffer();
		_tcscpy(m_szPwtFile, a);
		csCurFile.ReleaseBuffer( );
	}

    if(!bWorking)	((CButton*)(GetDlgItem(IDC_BUTTON_NEXTPWT)))->EnableWindow(0);

	//for correct result
	TCHAR ResultFile[MAX_PATH];
	CString csFileName;
	csFileName = m_szPwtFile;
	_stprintf(ResultFile, _T("%stxt"),csFileName.Left(csFileName.GetLength()-3));

	
	FILE *fp = _tfopen(ResultFile, _T("rb"));
	if (fp == NULL)
	{
		m_cCorrectRlt[0] = 0;
	}else
	{
		fscanf(fp, "%s\n", m_cCorrectRlt);
		fclose(fp);
	}
	
	
	//end

	if (!LoadFile())
	{
		AfxMessageBox(_T("end of pwt file"));
	}
//	CString csFileName;
	csFileName = ResultFile;
	int npos=0, nLastPos[2]={0,0};
	while(npos != -1)
	{
		nLastPos[1] = nLastPos[0];
		nLastPos[0] = npos;
		npos = csFileName.Find(_T("\\"), npos+1);
	}

	csFileName = csFileName.Right(csFileName.GetLength()-nLastPos[1]-1);

	m_SampleFileName.SetWindowText(csFileName);
	//for list correct result
	CString csText;
	unsigned short uRlt[256] = {0,};
	int i = 0;
	while (m_cCorrectRlt[i]!= 0)
	{
		uRlt[i] = (unsigned short)(m_cCorrectRlt[i]);
		i++;
	}
	uRlt[i] = 0; 
	csText.Format(_T("%s"),uRlt);
	m_CorrectResult.SetWindowText(csText);
}
