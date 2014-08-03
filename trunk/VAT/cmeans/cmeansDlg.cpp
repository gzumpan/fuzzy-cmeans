
// cmeansDlg.cpp : implementation file
//

#include "stdafx.h"
#include "cmeans.h"
#include "cmeansDlg.h"
#include "afxdialogex.h"
#include "FCMUtil.h"
#include "define.h"
#include "Logger.h"
#include "SubtractiveClustering.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
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

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CcmeansDlg dialog



CcmeansDlg::CcmeansDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CcmeansDlg::IDD, pParent)
{
	m_hIcon			= AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_pFCM			= NULL;
	m_pfcmReadFile	= NULL;
	m_pRenderData	= NULL;
	m_nSizeRender	= 0;
	m_pLogger		= CLogger::Instance();
	m_pLogger->Initialize(FCM_LOGPATH.c_str(), DLogModeAll);
}

void CcmeansDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BTN_START, m_btnStart);
	DDX_Control(pDX, IDC_BTN_STOP, m_btnStop);
}

BEGIN_MESSAGE_MAP(CcmeansDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BTN_OPEN_FILE, &CcmeansDlg::OnBnClickedOpenFile)
	ON_BN_CLICKED(IDC_BTN_START, &CcmeansDlg::OnBnClickedBtnStart)
	ON_WM_DESTROY()
END_MESSAGE_MAP()


// CcmeansDlg message handlers

BOOL CcmeansDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
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

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	UpdateStatus(FALSE, FALSE);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CcmeansDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CcmeansDlg::OnPaint()
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
		CDialogEx::OnPaint();
		DrawImage();
		DrawGraph();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CcmeansDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CcmeansDlg::UpdateStatus(BOOL bStart, BOOL bStop)
{
	m_btnStart.EnableWindow(bStart);
	m_btnStop.EnableWindow(bStop);
}

void CcmeansDlg::OnDestroy()
{
	CDialogEx::OnDestroy();

	// TODO: Add your message handler code here
	if (m_pFCM)	   { delete m_pFCM; }
	if (m_pLogger) { delete m_pLogger; }
	if (m_pfcmReadFile) { delete m_pfcmReadFile; }
	if (m_pRenderData) { delete[] m_pRenderData; }
}

void CcmeansDlg::OnBnClickedOpenFile()
{
	CString szInfo;

	// Create an Open dialog; the default file name extension is ".my".
	CFileDialog fileDlg(TRUE, NULL, NULL,
						OFN_FILEMUSTEXIST | OFN_HIDEREADONLY, 
						g_szFilter);

	// Display the file dialog. When user clicks OK, fileDlg.DoModal()  
	// returns IDOK. 
	if(fileDlg.DoModal() == IDOK)
	{
		m_szFilePath = fileDlg.GetPathName();

		// Implement opening and reading file in here. 		
		if (m_pfcmReadFile)
		{
			delete m_pfcmReadFile;
			m_pfcmReadFile = NULL;
		}
		m_pfcmReadFile = new CFCMReadFile(m_szFilePath);
		
		ASSERT(m_pfcmReadFile->IsOpen());

		m_pfcmReadFile->ParsingTextFile();

		szInfo.Format(FILE_DATA_INFO, m_pfcmReadFile->GetRows(), m_pfcmReadFile->GetColumns());		
		::SetWindowText(GetDlgItem(IDC_EDIT_FILE_INFO)->GetSafeHwnd(), szInfo);
		UpdateStatus(TRUE, FALSE);
	}
}


void CcmeansDlg::OnBnClickedBtnStart()
{
	if (m_pfcmReadFile)
	{
		if (InitFCM(*m_pfcmReadFile))
		{
			//m_pFCM->FCMClustering();
		}
	}
}

void CcmeansDlg::InitRenderData(Matrix& dataset, int width, int height)
{
	int xCoord, yCoord;
	int nsize = 0;
	double dScaleX, dScaleY;
	double dMinX, dMaxX, dMinY, dMaxY;
	
	if (m_pRenderData) {
		delete[] m_pRenderData;
	}

	m_nSizeRender = dataset.size1()*2;
	m_pRenderData = new double[m_nSizeRender];
	xCoord = m_pfcmReadFile->GetxCoord();
	yCoord = m_pfcmReadFile->GetyCoord();
	dMinX = m_pfcmReadFile->GetMinX();
	dMaxX = m_pfcmReadFile->GetMaxX();
	dMinY = m_pfcmReadFile->GetMinY();
	dMaxY = m_pfcmReadFile->GetMaxY();

	dScaleX = width / (dMaxX - dMinX);
	dScaleY = height / (dMaxY - dMinY);

	for (int r = 0; r < dataset.size1(); ++r) {
		m_pRenderData[nsize++] = 0 + (dataset(r, xCoord) - dMinX) * dScaleX;
		m_pRenderData[nsize++] = 0 + (dataset(r, yCoord) - dMinY) * dScaleY;
	}
}

BOOL CcmeansDlg::InitFCM(CFCMReadFile& fcmReadFile)
{
	int c = 2; // number cluster.
	double m = 2; // fuzziness exponent.
	double eps = 0.01; // termination tolerance.
	Matrix* pdataset;
	CString sztmp;

	// Testing VAT algorithm
	CWaitCursor wait;
	DoubleArray& arrDataSet = fcmReadFile.GetDataSet();
	m_pVATAlg.InitFilePath(m_szFilePath);
	m_pVATAlg.Initialize(arrDataSet, fcmReadFile.GetRows(), fcmReadFile.GetColumns());

	// Init render data.
	pdataset = fcmReadFile.GetMatrixData();
	InitRenderData(*pdataset, 150, 150);

	wait.Restore();
	Invalidate();
	return FALSE;


	if (m_pFCM)	{ delete m_pFCM; }

	pdataset = fcmReadFile.GetMatrixData();

	// cluster
//	m_editCluster.GetWindowText(sztmp);
	if (!sztmp.IsEmpty())
	{
		//c = FCMUtil::toInt(sztmp);
		double radii = FCMUtil::toDouble(sztmp);
		CSubtractiveClustering* pSubClus = new CSubtractiveClustering(pdataset, radii);
		pSubClus->Processing();
		delete pSubClus;
		return FALSE;
	}

	// fuzziness exponent.
//	m_editFuzziness.GetWindowText(sztmp);
	if (!sztmp.IsEmpty())
	{
		m = FCMUtil::toDouble(sztmp);
	}

	// Termination tolerance
//	m_editTermination.GetWindowText(sztmp);
	if (sztmp.IsEmpty())
	{
		eps = FCMUtil::toDouble(sztmp);
	}

	// allocation memory.
	m_pFCM = new CFuzzy(pdataset, c, m, eps);
	if (!m_pFCM)
	{
		return FALSE;
	}

	return TRUE;
}

void CcmeansDlg::DrawImage()
{
	ATL::CImage* pOriginalImage = NULL;
	ATL::CImage* pVATImage = NULL;
	ATL::CImage* piVATImage = NULL;
	CWnd* pWnd = NULL;
	CDC* pDC = NULL;
	int newWidth, newHeight;
	CRect rect;
	
	// Draw original image
	pOriginalImage  = m_pVATAlg.GetOriginalImage();
	if (!pOriginalImage->IsNull()) {
		pWnd = GetDlgItem( IDC_PC_ORIGINAL_IMG );
		pDC = pWnd->GetDC();
		pDC->SetStretchBltMode(COLORONCOLOR);

		pWnd->GetClientRect( &rect );
		ClientToScreen( &rect );

		newWidth = rect.Width();//(pOriginalImage->GetWidth()*rect.Height()) / pOriginalImage->GetHeight();
		newHeight = rect.Height();

		pOriginalImage->Draw( pDC->GetSafeHdc(), 0, 0, newWidth, newHeight );

		pWnd->ReleaseDC(pDC);
	}


	// Draw VAT image.
	pVATImage = m_pVATAlg.GetVATImage();
	if (!pVATImage->IsNull()) {
		pWnd = GetDlgItem( IDC_PC_VAT_IMG );
		pDC = pWnd->GetDC();
		pDC->SetStretchBltMode(COLORONCOLOR);
		pWnd->GetClientRect( &rect );
		ClientToScreen( &rect );
		pVATImage->Draw( pDC->GetSafeHdc(), 0, 0, newWidth, newHeight );
		pWnd->ReleaseDC(pDC);
	}

	// iVAT image.
	piVATImage = m_pVATAlg.GetiVATImage();
	if (!piVATImage->IsNull()){
		// Draw iVAT image.
		pWnd = GetDlgItem( IDC_PC_IVAT_IMG );
		pDC = pWnd->GetDC();
		pDC->SetStretchBltMode(COLORONCOLOR);
		pWnd->GetClientRect( &rect );
		ClientToScreen( &rect );
		piVATImage->Draw( pDC->GetSafeHdc(), 0, 0, newWidth, newHeight );
		pWnd->ReleaseDC(pDC);
	}
}

void CcmeansDlg::DrawGraph()
{
	CRect rect;
	CWnd* pWnd = NULL;
	CDC* pDC = NULL;
	int newWidth, newHeight;

	if (m_pRenderData) {
		pWnd = GetDlgItem( IDC_PC_VAT_GRAPH_IMG);
		pDC = pWnd->GetDC();
		pDC->SetStretchBltMode(COLORONCOLOR);
		pWnd->GetClientRect( &rect );
		ClientToScreen( &rect );
		CPen pPen(PS_SOLID, 1, RGB(255, 0, 0));
		CPen* pOldPen = pDC->SelectObject(&pPen);

		CBrush brush;
		brush.CreateStockObject(NULL_BRUSH);
		CBrush* oldBrush =  (CBrush*)pDC->SelectObject(&brush);

		for (int i = 0; i < m_nSizeRender; i += 2) {
			pDC->Ellipse(m_pRenderData[i], m_pRenderData[i+1], m_pRenderData[i] + 2, m_pRenderData[i+1] + 2);
		}

		pDC->SelectObject(pOldPen);
		pDC->SelectObject(oldBrush);
		pWnd->ReleaseDC(pDC);
	}
}