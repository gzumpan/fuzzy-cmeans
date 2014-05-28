// UnicodeUtil.cpp: implementation of the UnicodeUtil class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "UnicodeUtil.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

UnicodeUtil::UnicodeUtil()
{

}

UnicodeUtil::~UnicodeUtil()
{

}

BOOL UnicodeUtil::Utf8ToWstring(const std::string utf8string, std::wstring &resultstring)
{
	ConversionResult res;
	size_t widesize;

	widesize = utf8string.length();
	if (sizeof(wchar_t) == 2)
	{
		UTF16 *targetstart, *targetend;
		const UTF8* sourcestart = reinterpret_cast<const UTF8*>(utf8string.c_str());
		const UTF8* sourceend = sourcestart + widesize;

		resultstring.resize(widesize + 1, L'\0');
		targetstart = reinterpret_cast<UTF16*>(&resultstring[0]);
		targetend = targetstart + widesize;

		res = ConvertUTF8toUTF16(&sourcestart, sourceend, &targetstart, targetend, strictConversion);
		if (res != conversionOK) {
			return (FALSE);
		}
		*targetstart = 0;

		return (TRUE);
	}
	else if (sizeof(wchar_t) == 4)
	{
		UTF32 *targetstart, *targetend;
		const UTF8* sourcestart = reinterpret_cast<const UTF8*>(utf8string.c_str());
		const UTF8* sourceend = sourcestart + widesize;

		resultstring.resize(widesize + 1, L'\0');
		targetstart = reinterpret_cast<UTF32*>(&resultstring[0]);
		targetend = targetstart + widesize;
		
		res = ConvertUTF8toUTF32(&sourcestart, sourceend, &targetstart, targetend, strictConversion);
		if (res != conversionOK) {
			return (FALSE);
		}
		*targetstart = 0;

		return (TRUE);
	}

	return (FALSE);
}

BOOL UnicodeUtil::WstringToUtf8(const std::wstring widestring, std::string &resultstring)
{
	ConversionResult res;
	size_t widesize, utf8size;
	size_t endPos;

	widesize = widestring.length();
	if (sizeof(wchar_t) == 2)
	{
		UTF8 *targetstart, *targetend;
		const UTF16* sourcestart = reinterpret_cast<const UTF16*>(widestring.c_str());
		const UTF16* sourceend = sourcestart + widesize;

		utf8size = 3 * widesize + 1;
		resultstring.resize(utf8size, '\0');

		targetstart = reinterpret_cast<UTF8*>(&resultstring[0]);
		targetend = targetstart + utf8size;

		res = ConvertUTF16toUTF8(&sourcestart, sourceend, &targetstart, targetend, strictConversion);
		if (res != conversionOK) {
			return (FALSE);
		}
		*targetstart = 0;
		
		// Ninh change: erase trailings
		endPos = resultstring.find('\0');
		resultstring.erase(endPos, utf8size - widesize);

		return (TRUE);
	}
	else if (sizeof(wchar_t) == 4)
	{
		UTF8 *targetstart, *targetend;
		const UTF32* sourcestart = reinterpret_cast<const UTF32*>(widestring.c_str());
		const UTF32* sourceend = sourcestart + widesize;

		utf8size = 4 * widesize + 1;
		resultstring.resize(utf8size, '\0');

		targetstart = reinterpret_cast<UTF8*>(&resultstring[0]);
		targetend = targetstart + utf8size;

		res = ConvertUTF32toUTF8(&sourcestart, sourceend, &targetstart, targetend, strictConversion);
		if (res != conversionOK) {
			return (FALSE);
		}
		*targetstart = 0;
		
		// Ninh change: erase trailings
		endPos = resultstring.find('\0');
		resultstring.erase(endPos, utf8size - widesize);

		return (TRUE);
	}

	return (FALSE);
}
