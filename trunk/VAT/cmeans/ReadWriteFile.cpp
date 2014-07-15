#include "stdafx.h"
#include "ReadWriteFile.h"
#include "FCMUtil.h"

//////////////////////////////////////////////////////////////////////////
// File write

CFCMWriteFile::CFCMWriteFile(const FILENAMECHAR* filename, TEXTENCODING type)
	: CTextFileWrite(filename,type)
{

}

CFCMWriteFile::CFCMWriteFile(CFile* file, TEXTENCODING encoding)
	: CTextFileWrite(file, encoding)
{

}

CFCMWriteFile::~CFCMWriteFile(void)
{

}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// File read
CFCMReadFile::CFCMReadFile(const FILENAMECHAR* filename)
	: CTextFileRead(filename)
{
	ResetData();
}

CFCMReadFile::CFCMReadFile(CFile* file)
	: CTextFileRead(file)
{
	ResetData();
}

CFCMReadFile::~CFCMReadFile(void)
{
	
}

void CFCMReadFile::ResetData()
{
	m_nCols  = 0;
	m_nRows  = 0;
	m_data.RemoveAll();
}

void CFCMReadFile::ParsingLine(CString szLine, CString szDelimiter)
{
	// Format data: 1.123, 2.123, 3.123

	int iPos(0), iFound;
	CString sz;
	double d;
	
	while ((iFound = szLine.Find(szDelimiter, iPos)) != -1)
	{
		sz	 = szLine.Mid(iPos, iFound - iPos);
		d	 = FCMUtil::toDouble(sz);
		m_data.Add(d);
		iPos = iFound + 1;
	}

	// The number after the last comma. -> 3.123
	if (iPos < szLine.GetLength())
	{
		sz = szLine.Right(szLine.GetLength() - iPos);
		d  = FCMUtil::toDouble(sz);
		m_data.Add(d);
	}
}

void CFCMReadFile::ParsingLineMulti(CString szLine, CString szDelimiter)
{
	// Format data: 1.123, 2.123, 3.123

	int iPos(0);
	CString sz, szTmp;
	double d;

	szTmp = szLine;

	while ((iPos = szTmp.FindOneOf(szDelimiter)) != -1)
	{
		sz	  = szTmp.Left(iPos);
		szTmp = szTmp.Right(szTmp.GetLength() - iPos - 1);

		if (iPos) {
			d	 = FCMUtil::toDouble(sz);
			m_data.Add(d);
		}
	}

	if (!szTmp.IsEmpty())
	{
		d  = FCMUtil::toDouble(szTmp);
		m_data.Add(d);
	}
}

bool CFCMReadFile::ParsingTextFile(CString szDelimiter)
{
	if(!IsOpen())
	{
		return false;
	}

	CString szLine;

	ResetData();
	// m_nCols = GetNumberColums(szDelimiter);
	m_nCols = GetNumberColumsMulti(szDelimiter);

	while(!Eof())
	{
		if(ReadLine(szLine))
		{
			if (!szLine.IsEmpty()) {
				++m_nRows;
				//ParsingLine(szLine, szDelimiter);
				ParsingLineMulti(szLine, szDelimiter);
			}
		}
	};

	return true;
}

int CFCMReadFile::GetNumberColums(CString szDelimiter)
{
	int nCol = 0;
	int iPos(0), iFound;
	CString szline;

	if (ReadLine(szline))
	{
		ResetFilePointer();

		// Format data: A, B, C
	
		if (szline.IsEmpty()) { return 0; }

		while ((iFound = szline.Find(szDelimiter, iPos)) != -1)
		{
			++nCol;
			iPos = iFound + 1;
		}
	}

	return (nCol + 1);
}

int CFCMReadFile::GetNumberColumsMulti(CString szDelimiter)
{
	int nCol = 0;
	int iPos(0);
	CString szline, szTmp;
	
	if (ReadLine(szline))
	{
		ResetFilePointer();

		// Format data: A, B, C

		if (szline.IsEmpty()) { return 0; }

		szTmp = szline;

		while ((iPos = szTmp.FindOneOf(szDelimiter)) != -1)
		{
			if (iPos) ++nCol;
			szTmp = szTmp.Right(szTmp.GetLength() - iPos - 1);
		}
	}

	return (nCol + 1);
}

Matrix* CFCMReadFile::GetMatrixData()
{
	Matrix* pData = NULL;
	int nsize;

	if (m_nRows == 0 || m_nCols == 0 || m_data.GetSize() == 0)
	{
		return NULL;
	}

	if (m_nRows*m_nCols == m_data.GetSize())
	{
		// allocation memory.
		pData = new Matrix(m_nRows, m_nCols);

		for (int i = 0; i < m_nRows; i++)
		{
			nsize = i*m_nCols;
			for (int j = 0; j < m_nCols; j++)
			{
				(*pData)(i, j) = m_data.GetAt(nsize + j);
			}
		}
	}

	return pData;
}