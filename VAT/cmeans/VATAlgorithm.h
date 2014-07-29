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
	ATL::CImage* GetOriginalImage() { return &m_imgGrayscale; }
	ATL::CImage* GetReorderedImage() { return &m_imgGrayscaleReordered; }
	void	InitFilePath(CString szFilePath);
 
private:
	void	PrintDissMatrix(MatrixPtr mat);
	void	PrintDissMatrix(MatrixPtr mat, CUIntArray& reordered);
	void	ReorderedMatrix(MatrixPtr mat);
	int		FindMinDis(MatrixPtr mat, CUIntArray& I, CUIntArray& J);
	void	MakeGrayscaleImage(ATL::CImage& imgGray, BOOL bOrdered = TRUE);

private:
	CLogger*				m_pLogger;

private:
	MatrixPtr				m_pDissimilarityMatrix;
	CUIntArray				m_arrReordered;
	Distance2Objects		m_MaxDis;
	Distance2Objects		m_MinDis;

	ATL::CImage				m_imgGrayscale;
	ATL::CImage				m_imgGrayscaleReordered;
	CString					m_szOriFilePath;
	CString					m_szReorderedFilePath;
};

