#include "stdafx.h"
#include "VATAlgorithm.h"
#include "Logger.h"


CVATAlgorithm::CVATAlgorithm(void)
{
	m_pLogger = CLogger::Instance();
	m_szOriFilePath  = _T("D:\\Original.jpg");
	m_szVATFilePath  = _T("D:\\VAT.jpg");
	m_sziVATFilePath = _T("D:\\iVAT.jpg");
}


CVATAlgorithm::~CVATAlgorithm(void)
{
}

void CVATAlgorithm::Initialize(DoubleArray& data, int nRow, int nCol)
{
	double distance, tmp;
	int iObj, jObj;

	// Distance matrix.
	MatrixPtr pDisMat (new Matrix(nRow, nRow));
	MatrixPtr piVatMat (new Matrix(nRow, nRow));

	// reference
	m_pDissimilarityMatrix = pDisMat;
	m_pDissimilarityMatrix->clear();
	m_iVATMatrix = piVatMat;
	m_iVATMatrix->clear();
	
	// Init the first distance.
	// For dimension.
	distance = 0.0;
	for (int f = 0; f < nCol; ++f)
	{
		tmp		  = data[0+f] - data[nCol+f];
		distance += tmp*tmp;
	}
	distance = sqrt(distance);
	m_MaxDis.iObj = m_MinDis.iObj = 0;
	m_MaxDis.jObj = m_MinDis.jObj = 1;
	m_MaxDis.distance = m_MinDis.distance = distance;

	// Compute distance matrix - Euclidean.
	for (int i = 0; i < nRow; ++i)
	{
		for (int j = 0; j < i; ++j)
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

	// demo data.
// 	MatrixPtr pDisMatTest (new Matrix(5, 5));
// 	MatrixPtr piVATTest (new Matrix(5, 5));
// 
// 	m_pDissimilarityMatrix->clear();
// 	m_pDissimilarityMatrix.reset();
// 	m_pDissimilarityMatrix = pDisMatTest;
// 
// 	m_iVATMatrix->clear();
// 	m_iVATMatrix.reset();
// 	m_iVATMatrix = piVATTest;
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
	MakeGrayscaleImage(m_imgOriginal, m_pDissimilarityMatrix, m_szOriFilePath);

	// Reordered matrix
	ReorderedMatrix(m_pDissimilarityMatrix);
	MakeGrayscaleImage(m_imgVAT, m_iVATMatrix, m_szVATFilePath);
//	PrintDissMatrix(m_iVATMatrix);

	// iVAT
	iVATAlgorithm();
//	PrintDissMatrix(m_iVATMatrix);
	MakeGrayscaleImage(m_imgiVAT, m_iVATMatrix, m_sziVATFilePath);

	// Print reordered matrix.
//	PrintDissMatrix(m_pDissimilarityMatrix, m_arrReordered);

	// Save image
// 	CUIntArray arr;
// 	arr.SetSize(m_pDissimilarityMatrix->size1());
// 	for (int i = 0; i < arr.GetSize(); ++i) arr[i] = i;
// 	MakeGrayscaleImage(m_imgGrayscale, arr, m_szOriFilePath);
// 	MakeGrayscaleImage(m_imgGrayscaleReordered, m_arrReordered, m_szReorderedFilePath);
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
	CUIntArray J;
	int i, j;
	
	nsize = mat->size1();

	// Step 1: init data.
	m_arrReordered.RemoveAll();
	m_arrReordered.SetSize(nsize);
	J.SetSize(nsize);

	for (j = 0; j < nsize; ++j)
	{
		m_arrReordered[j] = -1;
		J[j] = j;
	}

	// Step 2: Select the first object.
	i = m_MaxDis.iObj;
	m_arrReordered[0] = i;
	J[i] = -1;

	// Step 3: loop find the min.
	for (int t = 1; t < nsize; ++t)
	{
		// find the min R_ij;
		j = FindMinDis(mat, m_arrReordered, J);
		ASSERT(j != -1);
		m_arrReordered[t] = j;
		J[j] = -1;
	}

	// Fill iVAT matrix.
	for (i = 0; i < nsize; ++i)
	{
		for (j = 0; j < nsize; ++j)
		{
			(*m_iVATMatrix)(i, j) = (*m_pDissimilarityMatrix)(m_arrReordered[i], m_arrReordered[j]);
		}
	}
}

int CVATAlgorithm::FindMinDis(MatrixPtr mat, CUIntArray& I, CUIntArray& J)
{
	int ret = -1;
	double dmin = m_MaxDis.distance;
	
	for (int i = 0; i < I.GetSize(); ++i)
	{
		if (I[i] == (UINT)(-1)) break;
		for (int j = 0; j < J.GetSize(); ++j)
		{
			if (J[j] == (UINT)(-1)) continue;
			if (dmin > (*mat)(I[i], J[j]))
			{
				dmin = (*mat)(I[i], J[j]);
				ret  = J[j];
			}
		}
	}

	return ret;
}

void CVATAlgorithm::MakeGrayscaleImage(ATL::CImage& imgGray, CUIntArray& arrReordered, CString szFilePath)
{
	int	width  = m_pDissimilarityMatrix->size1();
	int	height = width;	

	if(!imgGray.IsNull())
	{
		imgGray.Destroy();
	}
	imgGray.Create(width, height, 32);

	double gray;
	COLORREF c;
	double scale = 255.0 / m_MaxDis.distance;

	// Set pixel for image.
	for (int x=0; x<height; x++)
	{
		imgGray.SetPixel(x, x, RGB(0, 0, 0));
		for(int y=0; y < x; y++)
		{
			gray = scale * (*m_pDissimilarityMatrix)(arrReordered[x], arrReordered[y]);
			c = RGB(gray, gray, gray);
			imgGray.SetPixel(x, y, c);
			imgGray.SetPixel(y, x, c);
		}
	}

	// Save file
	imgGray.Save(szFilePath);
}

void CVATAlgorithm::MakeGrayscaleImage(ATL::CImage& imgGray, MatrixPtr mat, CString szFilePath)
{
	int	width  = mat->size1();
	int	height = width;	

	if(!imgGray.IsNull())
	{
		imgGray.Destroy();
	}
	imgGray.Create(width, height, 32);

	double gray;
	COLORREF c;
	double scale = 255.0 / m_MaxDis.distance;

	// Set pixel for image.
	for (int x=0; x<height; x++)
	{
		imgGray.SetPixel(x, x, RGB(0, 0, 0));
		for(int y=0; y < x; y++)
		{
			gray = scale * (*mat)(x, y);
			c = RGB(gray, gray, gray);
			imgGray.SetPixel(x, y, c);
			imgGray.SetPixel(y, x, c);
		}
	}

	// Save file
	imgGray.Save(szFilePath);
}

void CVATAlgorithm::InitFilePath(CString szFilePath)
{
	CString szFileName;
	CString szFolderPath;
	CString szTmp;
	
	szFileName = PathFindFileName(szFilePath);
	PathRemoveExtension((LPTSTR)(LPCTSTR)szFileName);
	szFolderPath = szFilePath.Left(szFilePath.ReverseFind('\\') + 1);

	m_szOriFilePath.Format(_T("%s%s%s"), szFolderPath, szFileName, _T("_Original.jpg"));
	m_szVATFilePath.Format(_T("%s%s%s"), szFolderPath, szFileName, _T("_VAT.jpg"));
	m_sziVATFilePath.Format(_T("%s%s%s"), szFolderPath, szFileName, _T("_iVAT.jpg"));
}

void CVATAlgorithm::iVATAlgorithm()
{
	int r, c, j;
	int nsize = m_iVATMatrix->size1();
	MatrixPtr iVAT(new Matrix(nsize, nsize));
	iVAT->clear();
	m_MaxDis.distance = 0.0;

	for (r = 1; r < nsize; ++r)
	{
		// 1. find min
		j = FindMinOnRow(m_iVATMatrix, r);

		// 2. 
		(*iVAT)(r, j) = (*m_iVATMatrix)(r, j);
		(*iVAT)(j, r) = (*iVAT)(r, j);
		if (m_MaxDis.distance < (*iVAT)(r, j)) m_MaxDis.distance = (*iVAT)(r, j);

		// 3. Find max.
		for (c = 0; c < r; c++)
		{
			if (c == j) continue;
			(*iVAT)(r, c) = max((*m_iVATMatrix)(r, j), (*iVAT)(j, c));
			(*iVAT)(c, r) = (*iVAT)(r, c);
			if (m_MaxDis.distance < (*iVAT)(r, c)) m_MaxDis.distance = (*iVAT)(r, c);
		}
	}

	m_iVATMatrix->clear();
	m_iVATMatrix.reset();
	m_iVATMatrix = iVAT;
}

int	CVATAlgorithm::FindMinOnRow(MatrixPtr mat, int row)
{
	int col;
	double dmin, dval;

	dmin = (*mat)(row, 0);
	col  = 0;

	for (int k = 1; k < row; k++)
	{
		if (dmin > (*mat)(row, k))
		{
			dmin = (*mat)(row, k);
			col  = k;
		}
	}

	return col;
}