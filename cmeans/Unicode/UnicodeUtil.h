// UnicodeUtil.h: interface for the UnicodeUtil class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_UNICODEUTIL_H__CA2FBAB3_B3DF_42EE_A791_CA63E9279089__INCLUDED_)
#define AFX_UNICODEUTIL_H__CA2FBAB3_B3DF_42EE_A791_CA63E9279089__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ConvertUTF.h"
#include <string>

class UnicodeUtil  
{
public:
	UnicodeUtil();
	virtual ~UnicodeUtil();

	/**
	 * Convert from UTF8 string to wide string (UTF16, UTF32)
	 *
	 * @author: Ninh
	 * @param: const std::string utf8string
	 * @param: std::wstring &resultstring
	 * @return: BOOL
	 */
	static BOOL Utf8ToWstring(const std::string utf8string, std::wstring &resultstring);

	/**
	 * Convert from wide string (UTF16, UTF32)to UTF8 string
	 *
	 * @author: Ninh
	 * @param: const std::wstring widestring
	 * @param: std::string &resultstring
	 * @return: BOOL
	 */
	static BOOL WstringToUtf8(const std::wstring widestring, std::string &resultstring);
};

#endif // !defined(AFX_UNICODEUTIL_H__CA2FBAB3_B3DF_42EE_A791_CA63E9279089__INCLUDED_)
