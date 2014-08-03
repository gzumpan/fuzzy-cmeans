#include "stdafx.h"
#include "VATAlgorithm.h"
#include "Logger.h"


CVATAlgorithm::CVATAlgorithm(void)
{
	m_pLogger = CLogger::Instance();
	m_szOriFilePath  = _T("D:\\Original.jpg");
	m_szVATFilePath  = _T("D:\\VAT.jpg");
	m_sziVATFilePath = _T("D:\\iVAT.jpg");
	m_pReordered = NULL;
}


CVATAlgorithm::~CVATAlgorithm(void)
{
	if (m_pReordered) {
		delete m_pReordered;
	}

	m_matOrigninal.reset();
	m_matiVAT.reset();
}

void CVATAlgorithm::Initialize(DoubleArray& data, int nRow, int nCol)
{
	double distance, tmp;
	int iObj, jObj;

	// Distance matrix.
	MatrixPtr pDisMat (new Matrix(nRow, nRow));
	MatrixPtr piVatMat (new Matrix(nRow, nRow));

	// reference
	m_matOrigninal.reset();
	m_matOrigninal = pDisMat;
	m_matOrigninal->clear();

	m_matiVAT.reset();
	m_matiVAT = piVatMat;
	m_matiVAT->clear();
	
	// Init the first distance.
	// For dimension.
	distance = 0.0;
	for (int f = 0; f < nCol; ++f) {
		tmp		  = data[0+f] - data[nCol+f];
		distance += tmp*tmp;
	}

	distance = sqrt(distance);
	m_MaxDis.iObj = m_MinDis.iObj = 0;
	m_MaxDis.jObj = m_MinDis.jObj = 1;
	m_MaxDis.distance = m_MinDis.distance = distance;

	// Compute distance matrix - Euclidean.
	for (int i = 0; i < nRow; ++i) {
		for (int j = 0; j < i; ++j)	{
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
			(*m_matOrigninal)(i, j) = (*m_matOrigninal)(j, i) = distance;
			
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

	// Original image.
//	MakeGrayscaleImage(m_imgOriginal, m_matOrigninal, m_szOriFilePath);

	// VAT algorithm
	VATAlgorithm(m_matOrigninal);
	MakeGrayscaleImage(m_imgVAT, m_matiVAT, m_szVATFilePath);

	// VAT modify algorithm.
	ModifyVAT(m_matOrigninal);
	MakeGrayscaleImage(m_imgOriginal, m_matiVAT, m_szOriFilePath);
#ifdef _DEBUG
	// Print VAT matrix
	PrintDissMatrix(m_matiVAT);
#endif // _DEBUG

	// iVAT algorithm
//	iVATAlgorithm();
//	MakeGrayscaleImage(m_imgiVAT, m_matiVAT, m_sziVATFilePath);
#ifdef _DEBUG
	// Print iVAT matrix.
	PrintDissMatrix(m_matiVAT);
#endif // _DEBUG
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

void CVATAlgorithm::VATAlgorithm(MatrixPtr mat)
{
	int nsize;
	int i, j;
	int* J;

	nsize = mat->size1();
	J = new int[nsize];

	// Step 1: init data.
	if (m_pReordered) {
		delete[] m_pReordered;
	}

	m_pReordered = new int[nsize];

	for (j = 0; j < nsize; ++j)	{
		m_pReordered[j] = -1;
		J[j] = j;
	}

	// Step 2: Select the first object.
	i = m_MaxDis.iObj;
	m_pReordered[0] = i;
	J[i] = -1;

	// Step 3: loop find the min.
	for (int t = 1; t < nsize; ++t)
	{
		// find the min R_ij;
		j = FindMinDis(mat, m_pReordered, J);
		ASSERT(j != -1);
		m_pReordered[t] = j;
		J[j] = -1;
	}

	// Fill iVAT matrix.
	m_matiVAT->clear();
	for (i = 0; i < nsize; ++i)
	{
		for (j = 0; j < i; ++j)
		{
			(*m_matiVAT)(i, j) = (*m_matiVAT)(j, i) = (*mat)(m_pReordered[i], m_pReordered[j]);
		}
	}
	delete[] J;
}

int CVATAlgorithm::FindMinDis(MatrixPtr mat, int* I, int* J)
{
	int ret = -1;
	int nsize = mat->size1();
	double dmin = m_MaxDis.distance;
	
	for (int i = 0; i < nsize; ++i)
	{
		if (I[i] == -1) break;
		for (int j = 0; j < nsize; ++j)
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


void CVATAlgorithm::MakeGrayscaleImage(ATL::CImage& imgGray, CUIntArray& arrReordered, CString szFilePath)
{
	int	width  = m_matOrigninal->size1();
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
			gray = scale * (*m_matOrigninal)(arrReordered[x], arrReordered[y]);
			c = RGB(gray, gray, gray);
			imgGray.SetPixel(x, y, c);
			imgGray.SetPixel(y, x, c);
		}
	}

#ifdef _DEBUG
	// Save file
	imgGray.Save(szFilePath);
#endif // _DEBUG
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
#ifdef _DEBUG
	CString szFileName;
	CString szFolderPath;
	CString szTmp;
	
	szFileName = PathFindFileName(szFilePath);
	PathRemoveExtension((LPTSTR)(LPCTSTR)szFileName);
	szFolderPath = szFilePath.Left(szFilePath.ReverseFind('\\') + 1);

	m_szOriFilePath.Format(_T("%s%s%s"), szFolderPath, szFileName, _T("_Original.jpg"));
	m_szVATFilePath.Format(_T("%s%s%s"), szFolderPath, szFileName, _T("_VAT.jpg"));
	m_sziVATFilePath.Format(_T("%s%s%s"), szFolderPath, szFileName, _T("_iVAT.jpg"));
#endif // _DEBUG
}

void CVATAlgorithm::iVATAlgorithm()
{
	int r, c, j;
	int nsize = m_matiVAT->size1();
	MatrixPtr iVAT(new Matrix(nsize, nsize));
	iVAT->clear();
	m_MaxDis.distance = 0.0;

	for (r = 1; r < nsize; ++r)
	{
		// 1. find min
		j = FindMinOnRow(m_matiVAT, r);

		// 2. 
		(*iVAT)(r, j) = (*m_matiVAT)(r, j);
		(*iVAT)(j, r) = (*iVAT)(r, j);
		if (m_MaxDis.distance < (*iVAT)(r, j)) m_MaxDis.distance = (*iVAT)(r, j);

		// 3. Find max.
		for (c = 0; c < r; c++)
		{
			if (c == j) continue;
			(*iVAT)(r, c) = max((*m_matiVAT)(r, j), (*iVAT)(j, c));
			(*iVAT)(c, r) = (*iVAT)(r, c);
			if (m_MaxDis.distance < (*iVAT)(r, c)) m_MaxDis.distance = (*iVAT)(r, c);
		}
	}

	m_matiVAT->clear();
	m_matiVAT.reset();
	m_matiVAT = iVAT;
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

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

void CVATAlgorithm::ModifyVAT(MatrixPtr mat)
{
	int nsize;
	int i, j;
	int* J;
	BOOL bInsFirst = FALSE;
	int first, last;

	nsize = mat->size1();
	J = new int[nsize];
	
	for (j = 0; j < nsize; ++j)	{
		J[j] = j;
	}
	m_deVAT.clear();

	// Step 2: Select the first object.
	i = m_MaxDis.iObj;
	m_deVAT.push_back(i);
	J[i] = -1;

	first = last = i;
	// Step 3: loop find the min.
	for (int t = 1; t < nsize; ++t)
	{
		// find the min R_ij;
		j = FindMinDis(mat, first, last, J, bInsFirst);
		ASSERT(j != -1);
		if (bInsFirst) {
			m_deVAT.push_front(j);
			first = j;
		} else {
			m_deVAT.push_back(j);
			last = j;
		}

		J[j] = -1;
	}

	// Fill iVAT matrix.
	m_matiVAT->clear();
	for (i = 0; i < nsize; ++i)
	{
		for (j = 0; j < i; ++j)
		{
			(*m_matiVAT)(i, j) = (*m_matiVAT)(j, i) = (*mat)(m_deVAT[i], m_deVAT[j]);
		}
	}

	delete[] J;
}

int CVATAlgorithm::FindMinDis(MatrixPtr mat, int first, int last, int* J, BOOL& bInsFirst)
{
	int ret = -1;
	int nsize = mat->size1();
	double dmin = m_MaxDis.distance;

	for (int j = 0; j < nsize; j++)	{
		if (J[j] == -1) continue;

		if (dmin > (*mat)(first, j)) {
			dmin = (*mat)(first, j);
			ret  = j;
			bInsFirst = TRUE;
		}

		if (dmin > (*mat)(last, j)) {
			dmin = (*mat)(last, j);
			ret  = j;
			bInsFirst = FALSE;
		}
	}

	return ret;
}
