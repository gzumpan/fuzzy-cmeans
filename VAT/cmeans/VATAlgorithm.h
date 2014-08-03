#pragma once

#include "define.h"
#include <deque>

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

	// Get VAT data set.
	MatrixPtr	GetOriginalSet() { return m_matOrigninal; }
 
private:
	void	PrintDissMatrix(MatrixPtr mat);
	void	PrintDissMatrix(MatrixPtr mat, CUIntArray& reordered);
	void	VATAlgorithm(MatrixPtr mat);
	int		FindMinDis(MatrixPtr mat, int* I, int* J);
	void	MakeGrayscaleImage(ATL::CImage& imgGray, CUIntArray& arrReordered, CString szFilePath);
	void	MakeGrayscaleImage(ATL::CImage& imgGray, MatrixPtr mat, CString szFilePath);
	void	iVATAlgorithm();
	int		FindMinOnRow(MatrixPtr mat, int row);

// Modify VAT
private:
	void	ModifyVAT(MatrixPtr mat);
	int		FindMinDis(MatrixPtr mat, int first, int last, int* J, BOOL& bInsFirst);

private:
	CLogger*				m_pLogger;

private:
	MatrixPtr				m_matOrigninal;
	MatrixPtr				m_matiVAT;
	int*					m_pReordered;
	Distance2Objects		m_MaxDis;
	Distance2Objects		m_MinDis;

	ATL::CImage				m_imgOriginal;
	ATL::CImage				m_imgVAT;
	ATL::CImage				m_imgiVAT;

	CString					m_szOriFilePath;
	CString					m_szVATFilePath;
	CString					m_sziVATFilePath;

// VAT modify
private:
//	boost::fusion::deque<int>		m_deVAT;
	std::deque<int>			m_deVAT;
};

