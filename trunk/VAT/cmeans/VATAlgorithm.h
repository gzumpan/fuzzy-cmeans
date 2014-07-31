#pragma once

#include "define.h"

class CLogger;

class CVATAlgorithm
{
public:
	CVATAlgorithm(void);
	~CVATAlgorithm(void);

public:
	void	Initialize(DoubleArray& data, int nRow, int nCol);
	ATL::CImage* GetOriginalImage() { return &m_imgOriginal; }
	ATL::CImage* GetVATImage() { return &m_imgVAT; }
	ATL::CImage* GetiVATImage() { return &m_imgiVAT; }
	void	InitFilePath(CString szFilePath);
 
private:
	void	PrintDissMatrix(MatrixPtr mat);
	void	PrintDissMatrix(MatrixPtr mat, CUIntArray& reordered);
	void	ReorderedMatrix(MatrixPtr mat);
	int		FindMinDis(MatrixPtr mat, CUIntArray& I, CUIntArray& J);
	void	MakeGrayscaleImage(ATL::CImage& imgGray, CUIntArray& arrReordered, CString szFilePath);
	void	MakeGrayscaleImage(ATL::CImage& imgGray, MatrixPtr mat, CString szFilePath);
	void	iVATAlgorithm();
	int		FindMinOnRow(MatrixPtr mat, int row);

private:
	CLogger*				m_pLogger;

private:
	MatrixPtr				m_pDissimilarityMatrix;
	MatrixPtr				m_iVATMatrix;
	CUIntArray				m_arrReordered;
	Distance2Objects		m_MaxDis;
	Distance2Objects		m_MinDis;

	ATL::CImage				m_imgOriginal;
	ATL::CImage				m_imgVAT;
	ATL::CImage				m_imgiVAT;

	CString					m_szOriFilePath;
	CString					m_szVATFilePath;
	CString					m_sziVATFilePath;
};

