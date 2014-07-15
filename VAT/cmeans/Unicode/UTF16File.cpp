<PRE>// UTF16File.cpp: implementation of the CUTF16File class.
//
// Version 5.0, 2 February 2004.
//
// Jeffrey Walton
//
//	Modified by Jordan Walters 27/04/2005 to work with non-Unicode
//	builds as well.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "UTF16File.h"
#include <ATLCONV.H>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CUTF16File::CUTF16File(): CStdioFile(),
	m_bIsUnicode(FALSE),
	m_bByteSwapped(FALSE),
	m_dwCurrentFilePointer(0)
{
}

CUTF16File::CUTF16File(LPCTSTR lpszFileName, UINT nOpenFlags) :
	CStdioFile(lpszFileName, nOpenFlags), 
	m_bIsUnicode(FALSE),
	m_bByteSwapped(FALSE),
	m_dwCurrentFilePointer(0)
{
	char uchBOM[2] = {0};

	// We only need the BOM check if reading.
	if(CFile::modeWrite == (nOpenFlags & CFile::modeWrite)) { return; }

	// BOM is two bytes
	if(CFile::GetLength() < 2) { return; }

	m_dwCurrentFilePointer += CStdioFile::Read(reinterpret_cast<LPVOID>(uchBOM), 2);

	if(uchBOM[0] == UNICODE_BOM[0] &&  uchBOM[1] == UNICODE_BOM[1])
	{
		m_bIsUnicode   = TRUE;
		m_bByteSwapped = FALSE;
	} 

	if(uchBOM[0] == UNICODE_RBOM[0] &&  uchBOM[1] == UNICODE_RBOM[1])
	{
		m_bIsUnicode   = TRUE;
		m_bByteSwapped = TRUE;
	}

	// Not a BOM mark - its an ANSI file
	//   so punt to CStdioFile...
	if(FALSE == m_bIsUnicode)
	{
			m_dwCurrentFilePointer = 0;
		CStdioFile::Seek(0, CFile::begin);
	}

	m_Accumulator.clear();
}

BOOL CUTF16File::Open(LPCTSTR lpszFileName, UINT nOpenFlags, CFileException* pError /*=NULL*/)
{
	BOOL bResult = FALSE;

	unsigned char uchBOM[3] = {0};

	bResult = CStdioFile::Open(lpszFileName, nOpenFlags, pError);
	if (bResult == FALSE) {
		return bResult;
	}

	// We only need the BOM check if reading.
	if(CFile::modeWrite == (nOpenFlags & CFile::modeWrite)) { return bResult; }

	// BOM is two bytes
	if(CFile::GetLength() < 2) { return bResult; }

	if(TRUE == bResult)
	{
		m_dwCurrentFilePointer += CStdioFile::Read(reinterpret_cast<LPVOID>(uchBOM), 2);

		if(uchBOM[0] == UNICODE_BOM[0] &&  uchBOM[1] == UNICODE_BOM[1])
		{
			m_bIsUnicode   = TRUE;
			m_bByteSwapped = FALSE;
		} 

		if(uchBOM[0] == UNICODE_RBOM[0] &&  uchBOM[1] == UNICODE_RBOM[1])
		{
			m_bIsUnicode   = TRUE;
			m_bByteSwapped = TRUE;
		}

		// Not a BOM mark - its an ANSI file
		//   so punt to CStdioFile...
		if(FALSE == m_bIsUnicode)
		{
			m_dwCurrentFilePointer = 0;
			CStdioFile::Seek( 0, CFile::begin );
		}
	}

	m_Accumulator.clear();

	return bResult;
}

BOOL CUTF16File::ReadString( CString& rString )
{
	if(TRUE == m_bIsUnicode)
	{
		return ReadUnicodeString(rString);
	}

	return CStdioFile::ReadString(rString);
}

LPTSTR CUTF16File::ReadString(LPTSTR lpsz, UINT nMax)
{
	if(TRUE == m_bIsUnicode)
	{
		return ReadUnicodeString(lpsz, nMax);
	}

	return CStdioFile::ReadString(lpsz, nMax);
}

BOOL CUTF16File::ReadUnicodeString(CString& rString)
{
	USES_CONVERSION;

	BOOL bRead = FALSE;

	WCHAR c[2] = {0};

	rString.Empty();

	LoadAccumulator();

	while(FALSE == m_Accumulator.empty())
	{
		bRead = TRUE;

		c[0] = m_Accumulator.front();

		m_Accumulator.pop_front();

		if(L'\r' == c[0] || L'\n' == c[0])
		{
			// Set the file pointer to the current position of this carriage return - 
			// or one after as it has been read.
			m_dwCurrentFilePointer += 2;

			c[0] = m_Accumulator.front();

			m_Accumulator.pop_front();

			if(L'\r' == c[0] || L'\n' == c[0])
			{
				m_dwCurrentFilePointer += 2;
				Seek(m_dwCurrentFilePointer, CFile::begin);
			}
			break;
		}

		m_dwCurrentFilePointer += 2;
		rString += W2T(c);

		if(TRUE == m_Accumulator.empty())
		{
			LoadAccumulator();
		}
	}

	return bRead;;
}

/***
*char *fgets(string, count, stream) - input string from a stream
*
*Purpose:
*       get a string, up to count-1 chars or '\n', whichever comes first,
*       append '\0' and put the whole thing into string. the '\n' IS included
*       in the string. if count<=1 no input is requested. if EOF is found
*       immediately, return NULL. if EOF found after chars read, let EOF
*       finish the string as '\n' would.
*
***/

LPTSTR CUTF16File::ReadUnicodeString( LPTSTR lpsz, UINT nMax )
{
	USES_CONVERSION;

	BOOL bRead = FALSE;

	LPTSTR p = lpsz;
	WCHAR c[2] = {0};

	ASSERT(lpsz != NULL);
	ASSERT(AfxIsValidAddress(lpsz, nMax));
	ASSERT(m_pStream != NULL);

	if(nMax <= 1) { return lpsz; }

	LoadAccumulator();

	while(FALSE == m_Accumulator.empty() && --nMax)
	{
		bRead = TRUE;        

		c[0] = m_Accumulator.front();
		m_dwCurrentFilePointer += 2;
		*p++ = *(W2T(c));

		m_Accumulator.pop_front();

		if(L'\r' == c[0] || L'\n' == c[0])
		{
			// Set the file pointer to the current position of this carriage return - 
			// or one after as it has been read.
			m_dwCurrentFilePointer += 2;

			c[0] = m_Accumulator.front();

			m_Accumulator.pop_front();

			if(L'\r' == c[0] || L'\n' == c[0])
			{
				m_dwCurrentFilePointer += 2;
				Seek(m_dwCurrentFilePointer, CFile::begin);
			}
			break;
		}

		if(TRUE == m_Accumulator.empty())
		{
			LoadAccumulator();
		}
	}

	*p = L'\0';

	return TRUE == bRead ? lpsz : NULL;
}

VOID CUTF16File::WriteString( LPCTSTR lpsz, BOOL bAsUnicode /*= FALSE */ )
{
	USES_CONVERSION;
	
	if(TRUE == bAsUnicode)
	{

		// Cast to (LPTSTR) by JW (C2664 Error)
		WriteUnicodeString(T2W((LPTSTR)lpsz));
	}
	else
	{
		// Cast to (LPCSTR) by JW (C2664 Error)
		WriteANSIString((LPCSTR)lpsz);
	}
}

BOOL CUTF16File::LoadAccumulator()
{
	BYTE cbBuffer[ACCUMULATOR_CHAR_COUNT * sizeof(WCHAR)];

	UINT uCount = CStdioFile::Read(cbBuffer, ACCUMULATOR_CHAR_COUNT * sizeof(WCHAR));

	WCHAR* pwszBuffer = reinterpret_cast<WCHAR*>(cbBuffer);

	for(UINT i = 0; i < uCount / 2; i++)
	{
		WCHAR c = *pwszBuffer++;

		if(TRUE == m_bByteSwapped)
		{
			BYTE b1 = BYTE(c >> 8);   // high order
			BYTE b2 = BYTE(c & 0xFF); // low order

			c = WCHAR(b1 | (b2 << 8));
		}

		m_Accumulator.push_back(c);
	}

	return 0 == uCount;
}

LONG CUTF16File::Seek(LONG lOff, UINT nFrom)
{
	LONG lResult = CStdioFile::Seek(lOff, nFrom);
	m_dwCurrentFilePointer = CStdioFile::Seek(0, CFile::current);

	m_Accumulator.clear();

	//	LoadAccumulator();
	// Should there be a test here to set fp = 2 if Unicode,
	//  and the user asks for CFile::begin???

	return lResult;
}

VOID CUTF16File::WriteANSIString( LPCSTR lpsz )
{
	// Cast to (LPCSTR) by JW (C2664 Error)
	CStdioFile::WriteString( (LPCTSTR)lpsz);
}

VOID CUTF16File::WriteUnicodeString(LPCWSTR lpsz)
{
	if(0 == CFile::GetPosition())
	{
		CFile::Write(static_cast<LPVOID>(LPVOID(UNICODE_BOM)), sizeof(UNICODE_BOM));
	}

	CFile::Write(lpsz, wcslen(lpsz) * sizeof(WCHAR));
}
