// UTF16File.h: interface for the CUTF16File class.
//
// Version 5.0, 2 February 2004.
//
// Jeffrey Walton
//
//	Modified by Jordan Walters 27/04/2005 to work with non-Unicode
//	builds as well.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_UTF16File_H__32BEF8AC_25E0_482F_8B00_C40775BCDB81__INCLUDED_)
#define AFX_UTF16File_H__32BEF8AC_25E0_482F_8B00_C40775BCDB81__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#pragma warning(push, 3)
#include <LIST>
#pragma warning(pop)


//
// Under a hex editor, file[0] = 0xFF
//                     file[1] = 0xFE
//
// for a proper UTF-16 BOM
//
// This is different than the in-memory
//   representation of: mem[0] = 0xFE
//                      mem[1] = 0xFF
//
// on an Intel CPU
//
const unsigned char UNICODE_BOM[2]				= {unsigned char(0xFF), unsigned char(0xFE)};
const unsigned char UNICODE_RBOM[2]				= {unsigned char(0xFE), unsigned char(0xFF)};

const INT ACCUMULATOR_CHAR_COUNT				= 2048;

class CUTF16File: public CStdioFile
{
public:
	
	CUTF16File();
	CUTF16File(LPCTSTR lpszFileName, UINT nOpenFlags);

	virtual BOOL	Open(LPCTSTR lpszFileName, UINT nOpenFlags, CFileException* pError = NULL);
	virtual BOOL	ReadString(CString& rString);
    virtual LPTSTR  ReadString(LPTSTR lpsz, UINT nMax);
	virtual VOID	WriteString(LPCTSTR lpsz, BOOL bAsUnicode = FALSE);

    virtual LONG    Seek(LONG lOff, UINT nFrom);

    BOOL            IsUnicodeFile() { return m_bIsUnicode; }

protected:

	BOOL            ReadUnicodeString(CString& szString);
    LPTSTR          ReadUnicodeString(LPTSTR lpsz, UINT nMax);

    virtual VOID    WriteANSIString(LPCSTR lpsz);
    virtual VOID    WriteUnicodeString(LPCWSTR lpsz);

	BOOL m_bIsUnicode;
    BOOL m_bByteSwapped;

private:

	BOOL LoadAccumulator();

    std::list<WCHAR> m_Accumulator;
	DWORD	m_dwCurrentFilePointer;
};

#endif // !defined(AFX_UTF16File_H__32BEF8AC_25E0_482F_8B00_C40775BCDB81__INCLUDED_)
