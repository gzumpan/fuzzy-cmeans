#include "stdafx.h"
#include "VATAlgorithm.h"
#include "Logger.h"


CVATAlgorithm::CVATAlgorithm(void)
{
	m_pLogger = CLogger::Instance();
}


CVATAlgorithm::~CVATAlgorithm(void)
{
}

void CVATAlgorithm::Initialize(DoubleArray& data, int nRow, int nCol)
{
	double distance, tmp;
	int iObj, jObj;
	BOOL bInit = TRUE;

	// Distance matrix.
	MatrixPtr pDisMat (new Matrix(nRow, nRow));
	
	// reference
	m_pDissimilarityMatrix = pDisMat;
	m_pDissimilarityMatrix->clear();
	
	// Compute distance matrix - Euclidean.
	for (int i = 0; i < nRow - 1; ++i)
	{
		for (int j = i + 1; j < nRow; ++j)
		{
			distance = 0.0;
			iObj = i*nCol;
			jObj = j*nCol;

			// For dimension.
			for (int f = 0; f < nCol; ++f)
			{
				tmp		  = data[iObj+f] - data[jObj+f];
				distance += tmp*tmp;
			}

			// Hold the distance.
			distance = sqrt(distance);
			(*m_pDissimilarityMatrix)(i, j) = (*m_pDissimilarityMatrix)(j, i) = distance;

			// Find max distance and min distance.
			if (bInit)
			{
				m_MaxDis.iObj = i;
				m_MaxDis.jObj = j;
				m_MinDis.iObj = i;
				m_MinDis.jObj = j;
				m_MaxDis.distance = m_MinDis.distance = distance;
				bInit = FALSE;
			}
			else
			{
				if (m_MaxDis.distance < distance)
				{
					m_MaxDis.iObj = i;
					m_MaxDis.jObj = j;
					m_MaxDis.distance = distance;
				}

				if (m_MinDis.distance > distance) 
				{
					m_MinDis.iObj = i;
					m_MinDis.jObj = j;
					m_MinDis.distance = distance;
				}
			}
		}
	}

	// demo data.
//	MatrixPtr pDisMatTest (new Matrix(5, 5));
// 	m_pDissimilarityMatrix = pDisMatTest;
// 	m_MaxDis.distance = 0.78;
// 	m_MaxDis.iObj = 1;
// 	m_MaxDis.jObj = 4;
// 	m_MinDis.distance = 0.12;
// 	m_MinDis.iObj = 1;
// 	m_MinDis.jObj = 3;
// 
// 	(*m_pDissimilarityMatrix)(0, 0) = 0;
// 	(*m_pDissimilarityMatrix)(0, 1) = (*m_pDissimilarityMatrix)(1, 0) = 0.73;
// 	(*m_pDissimilarityMatrix)(0, 2) = (*m_pDissimilarityMatrix)(2, 0) = 0.19;
// 	(*m_pDissimilarityMatrix)(0, 3) = (*m_pDissimilarityMatrix)(3, 0) = 0.71;
// 	(*m_pDissimilarityMatrix)(0, 4) = (*m_pDissimilarityMatrix)(4, 0) = 0.16;
// 
// 	(*m_pDissimilarityMatrix)(1, 0) = (*m_pDissimilarityMatrix)(0, 1) = 0.73;
// 	(*m_pDissimilarityMatrix)(1, 1) = (*m_pDissimilarityMatrix)(1, 1) = 0.0;
// 	(*m_pDissimilarityMatrix)(1, 2) = (*m_pDissimilarityMatrix)(2, 1) = 0.59;
// 	(*m_pDissimilarityMatrix)(1, 3) = (*m_pDissimilarityMatrix)(3, 1) = 0.12;
// 	(*m_pDissimilarityMatrix)(1, 4) = (*m_pDissimilarityMatrix)(4, 1) = 0.78;
// 
// 	(*m_pDissimilarityMatrix)(2, 0) = (*m_pDissimilarityMatrix)(0, 2) = 0.19;
// 	(*m_pDissimilarityMatrix)(2, 1) = (*m_pDissimilarityMatrix)(1, 2) = 0.59;
// 	(*m_pDissimilarityMatrix)(2, 2) = (*m_pDissimilarityMatrix)(2, 2) = 0.0;
// 	(*m_pDissimilarityMatrix)(2, 3) = (*m_pDissimilarityMatrix)(3, 2) = 0.55;
// 	(*m_pDissimilarityMatrix)(2, 4) = (*m_pDissimilarityMatrix)(4, 2) = 0.19;
// 
// 	(*m_pDissimilarityMatrix)(3, 0) = (*m_pDissimilarityMatrix)(0, 3) = 0.71;
// 	(*m_pDissimilarityMatrix)(3, 1) = (*m_pDissimilarityMatrix)(1, 3) = 0.12;
// 	(*m_pDissimilarityMatrix)(3, 2) = (*m_pDissimilarityMatrix)(2, 3) = 0.55;
// 	(*m_pDissimilarityMatrix)(3, 3) = (*m_pDissimilarityMatrix)(3, 3) = 0.0;
// 	(*m_pDissimilarityMatrix)(3, 4) = (*m_pDissimilarityMatrix)(4, 3) = 0.74;
// 
// 	(*m_pDissimilarityMatrix)(4, 0) = (*m_pDissimilarityMatrix)(0, 4) = 0.16;
// 	(*m_pDissimilarityMatrix)(4, 1) = (*m_pDissimilarityMatrix)(1, 4) = 0.78;
// 	(*m_pDissimilarityMatrix)(4, 2) = (*m_pDissimilarityMatrix)(2, 4) = 0.19;
// 	(*m_pDissimilarityMatrix)(4, 3) = (*m_pDissimilarityMatrix)(3, 4) = 0.74;
// 	(*m_pDissimilarityMatrix)(4, 4) = (*m_pDissimilarityMatrix)(4, 4) = 0.0;

	// Print matrix.
//	PrintDissMatrix(m_pDissimilarityMatrix);

	// Reordered matrix
	ReorderedMatrix(m_pDissimilarityMatrix);

	// Print reordered matrix.
//	PrintDissMatrix(m_pDissimilarityMatrix, m_arrReordered);

	// Save image
	MakeGrayscaleImage(m_imgGrayscale, FALSE);
	MakeGrayscaleImage(m_imgGrayscaleReordered, TRUE);
}

void CVATAlgorithm::PrintDissMatrix(MatrixPtr mat)
{
	int nIndex;

	for (int i = 0; i < mat->size1(); ++i)
	{
		for (int j = 0; j < mat->size2(); ++j)
		{
			m_pLogger->LogNoPrefix("%.2f\t", (*mat)(i, j));
		}

		m_pLogger->LogNoPrefix("\n");
	}

	m_pLogger->LogNoPrefix("==========================================\n");
}

void CVATAlgorithm::PrintDissMatrix(MatrixPtr mat, CUIntArray& reordered)
{
	int nIndex;

	for (int i = 0; i < mat->size1(); ++i)
	{
		for (int j = 0; j < mat->size2(); ++j)
		{
			m_pLogger->LogNoPrefix("%.2f\t", (*mat)(reordered[i], reordered[j]));
		}

		m_pLogger->LogNoPrefix("\n");
	}

	m_pLogger->LogNoPrefix("==========================================\n");
}

void CVATAlgorithm::ReorderedMatrix(MatrixPtr mat)
{
	int nsize;
	CUIntArray I, J;
	int i, j;
	
	nsize = mat->size1();

	// Step 1: init data.
	m_arrReordered.RemoveAll();
	m_arrReordered.SetSize(nsize);
	J.SetSize(nsize);
	for (j = 0; j < nsize; ++j)
	{
		m_arrReordered[j] = 0;
		J[j] = j;
	}

	// Step 2: Select the first object.
	i = m_MaxDis.iObj;
	m_arrReordered[0] = i;
	I.Add(i);
	J.SetAt(i, -1);

	// Step 3: loop find the min.
	for (int t = 1; t < nsize; ++t)
	{
		// find the min R_ij;
		j = FindMinDis(mat, I, J);
		ASSERT(j != -1);
		m_arrReordered[t] = j;
		I.Add(j);
		J.SetAt(j, -1);
	}
}

int CVATAlgorithm::FindMinDis(MatrixPtr mat, CUIntArray& I, CUIntArray& J)
{
	int ret = -1;
	double dmin = m_MaxDis.distance;
	
	for (int i = 0; i < I.GetSize(); ++i)
	{
		for (int j = 0; j < J.GetSize(); ++j)
		{
			if (J[j] == -1) continue;
			if (dmin > (*mat)(I[i], J[j]))
			{
				dmin = (*mat)(I[i], J[j]);
				ret  = J[j];
			}
		}
	}

	return ret;
}

void CVATAlgorithm::MakeGrayscaleImage(ATL::CImage& imgGray, BOOL bOrdered/* = TRUE*/)
{
	int	width  = m_pDissimilarityMatrix->size1();
	int	height = width;	

	if(!imgGray.IsNull())
	{
		imgGray.Destroy();
	}
	BOOL bret = imgGray.Create(width, height, 32);

	double gray;
	COLORREF c;
	double scale = 255.0 / m_MaxDis.distance;

	// Reordered array.
	CUIntArray arrReoredered;
	arrReoredered.SetSize(m_arrReordered.GetSize());
	arrReoredered.Copy(m_arrReordered);
	if (!bOrdered)
	{
		for (int i = 0; i < arrReoredered.GetSize(); ++i)
			arrReoredered[i] = i;
	}

	// Set pixel for image.
	for (int x=0; x<height-1; x++)
	{
		imgGray.SetPixel(x, x, RGB(0, 0, 0));
		for(int y=x+1; y < width; y++)
		{
			gray = scale * (*m_pDissimilarityMatrix)(arrReoredered[x], arrReoredered[y]);
			c = RGB(gray, gray, gray);
			imgGray.SetPixel(x, y, c);
			imgGray.SetPixel(y, x, c);
		}
	}

	if (bOrdered)
		imgGray.Save(m_szOriFilePath);
	else
		imgGray.Save(m_szReorderedFilePath);
}

void CVATAlgorithm::InitFilePath(CString szFilePath)
{
	CString szFileName;
	CString szFolderPath;
	CString szTmp;
	
	szFileName = PathFindFileName(szFilePath);
	PathRemoveExtension((LPTSTR)(LPCTSTR)szFileName);
	szFolderPath = szFilePath.Left(szFilePath.ReverseFind('\\') + 1);

	szTmp = szFileName + _T("_Original");
	szTmp.Append(_T("_Original"));
	m_szOriFilePath = szFolderPath + szTmp;

	szTmp = szFileName + _T("_Reordered");
	m_szReorderedFilePath = szFolderPath + szTmp;
}