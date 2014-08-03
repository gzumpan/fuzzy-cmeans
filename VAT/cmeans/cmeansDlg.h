
// cmeansDlg.h : header file
//

#pragma once

#include "FuzzyClustering.h"
#include "afxwin.h"
#include "ReadWriteFile.h"
#include "VATAlgorithm.h"

using namespace Clustering;

class CLogger;

// CcmeansDlg dialog
class CcmeansDlg : public CDialogEx
{
// Construction
public:
	CcmeansDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_CMEANS_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOpenFile();

public:
	void	UpdateStatus(BOOL bStart = FALSE, BOOL bStop = FALSE);

private:
	void	InitRenderData(Matrix& dataset, int width, int height);
	BOOL	InitFCM(CFCMReadFile& fcmReadFile);
	void	DrawImage();
	void	DrawGraph();

private:
	CButton m_btnStart;
	CButton m_btnStop;

private:
	CVATAlgorithm			m_pVATAlg;
	CFuzzy*					m_pFCM;
	CLogger*				m_pLogger;
	CFCMReadFile*			m_pfcmReadFile;
	CString					m_szFilePath;

	// Data for render.
	double*					m_pRenderData;
	int						m_nSizeRender;

public:
	afx_msg void OnBnClickedBtnStart();
	afx_msg void OnDestroy();
};
