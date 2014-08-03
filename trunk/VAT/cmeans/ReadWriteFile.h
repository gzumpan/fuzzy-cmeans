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
	int		GetxCoord() { return m_xCoord; }
	int		GetyCoord() { return m_yCoord; }
	
	double	GetMinX() { return m_dMinX; }
	double	GetMaxX() { return m_dMaxX; }
	double	GetMinY() { return m_dMinY; }
	double	GetMaxY() { return m_dMaxY; }

	bool	ParsingTextFile(CString delimiter = FCM_DELIMITER);
	Matrix*	GetMatrixData();
	DoubleArray& GetDataSet() { return m_data; }

private:
	void	ParsingLine(CString szLine, CString szDelimiter = FCM_DELIMITER);
	void	ParsingLineMulti(CString szLine, CString szDelimiter = FCM_DELIMITER);
	int		GetNumberColums(CString szDelimiter = FCM_DELIMITER);
	int		GetNumberColumsMulti(CString szDelimiter = FCM_DELIMITER);

private:
	DoubleArray					m_data;
	int							m_nRows;
	int							m_nCols;
	double						m_dMaxX, m_dMinX;
	double						m_dMaxY, m_dMinY;
	int							m_xCoord, m_yCoord;
};

