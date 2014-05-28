#pragma once

#include "textfile.h"
#include "FuzzyClustering.h"
#include "define.h"

using namespace Clustering;


class CFCMWriteFile : public CTextFileWrite
{
public:
	CFCMWriteFile(const FILENAMECHAR* filename, TEXTENCODING type = ASCII);
	CFCMWriteFile(CFile* file, TEXTENCODING type = ASCII);

	~CFCMWriteFile(void);

private:
	static CFCMWriteFile*	m_pInstance;
};

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

class CFCMReadFile : public CTextFileRead
{
public:
	CFCMReadFile(const FILENAMECHAR* filename);
	CFCMReadFile(CFile* file);
	~CFCMReadFile(void);

public:
	void	ResetData();
	int		GetColumns() { return m_nCols; }
	int		GetRows()	 { return m_nRows; }

	bool	ParsingTextFile(CString delimiter = FCM_DELIMITER);
	Matrix*	GetMatrixData();

private:
	void	ParsingLine(CString szLine, CString szDelimiter = FCM_DELIMITER);
	int		GetNumberColums(CString szDelimiter = FCM_DELIMITER);

private:
	CArray<double, double&>		m_data;
	int							m_nRows;
	int							m_nCols;
};

