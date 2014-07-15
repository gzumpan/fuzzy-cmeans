#if !defined(AFX_DMCUTIL_H__9CF8288E_90BE_4EA7_962C_9E92C61F3D13__INCLUDED_)
#define AFX_DMCUTIL_H__9CF8288E_90BE_4EA7_962C_9E92C61F3D13__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DMCUtil.h : header file
//

#include "Utility/Logger.h"

using namespace std;

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

#define DMCUTIL_ASSERT_LOG(f)										if (!(f)) { \
	\
	LPVOID lpMsgBuf; \
	\
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, \
	NULL, GetLastError(), 0, (LPTSTR) &lpMsgBuf, 0, NULL); \
	\
	m_pLogger->LogError(FILEINFO, "DMCUTIL_ASSERT: Failed, bInitStatus: %d, bClosing: %d.\n", m_bInitializedStatus, m_bClosing); \
	m_pLogger->LogError(FILEINFO, "DMCUTIL_ASSERT: Error: Code: %d, Msg: %s.\n", GetLastError(), DMCUtil::ToString(CString((LPCTSTR)lpMsgBuf)).c_str()); \
	\
	LocalFree(lpMsgBuf); \
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

#define DEF_MAX_PATH												4096

// Date time format
#define DTIME_APM_HHMMSS											_T("%p %I:%M:%S")
#define DTIME_YYMMDD_APM_HHMMSS										_T("%Y-%m-%d %p %I:%M:%S")
#define DTIME_APM_HHMM												_T("%p %I:%M")
#define DTIME_YYMMDD_APM_HHMM										_T("%Y-%m-%d %p %I:%M")
#define DTIME_YYMMDD												_T("%Y-%m-%d")
#define DTIME_MMDDYY												_T("%m-%d-%Y")
#define DTIME_SQL													_T("%Y-%m-%d %H:%M:%S")

// http://coding.derkeiler.com/Archive/PHP/alt.php/2006-09/msg00307.html
// FILETIME is a 64-bit unsigned integer representing
// the number of 100-nanosecond intervals since January 1, 1601
// UNIX timestamp is number of seconds since January 1, 1970
// 116444736000000000 = 10000000 * 60 * 60 * 24 * 365 * 369 + 89 leap days
#define SECONDS_DIFF_BETWEEN_UNIXUTC_AND_FILETIME					11644473600

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

namespace DMCUtil
{
	/**
	 * Convert some type data to CString.
	 *
	 * @author: Giang Nguyen
	 * @param: INT val
	 * @return: CString
	 */
	CString	ToCString(const INT val);
	CString	ToCString(const UINT val);
	CString	ToCString(const ULONG val);
	CString	ToCString(const LONGLONG val);
	CString	ToCString(const std::string val);	
	CString	ToCStringFromDateTime(const LONGLONG llDateTime, BOOL bCompact, BOOL bSecond, BOOL bUnix, CString szAMText, CString szPMText);
	CString ToCStringFromDateTimeFormat(int year, int month, int day, CString szFormat);
	CString ToCStringFromChar(char* val);
		
	/**
	 * Convert some type data to std::string (ANSI string).
	 * 
	 * @author: Giang Nguyen
	 * @param: const INT val
	 * @return: std::string
	 */
	std::string ToString(const INT val);
	std::string ToString(const UINT val);
	std::string ToString(const ULONG val);
	std::string	ToString(const LONGLONG val);
	std::string ToString(const CString val);
	std::string ToString(const WCHAR* val);
	std::string ToLower(std::string val);

	/**
	 * Convert std::string/CString to INT/UINT.
	 *
	 * @author: Giang Nguyen
	 * @param: std::string val
	 * @return: INT
	 */
	INT		ToINT(const CString val);
	INT		ToINT(const std::string val);

	UINT	ToUINT(const CString val);
	UINT	ToUINT(const std::string val);

	ULONG	ToULONG(const CString val);
	ULONG	ToULONG(const std::string val);

	/**
	 * Convert char buffer to long long.
	 *
	 * @author: Giang Nguyen
	 * @param: const char * str
	 * @return: LONGLONG
	 */
	LONGLONG ToLLONG(const std::string val);

	BOOL	ToBOOL(const CString val);
	BOOL	ToBOOL(const std::string val);
	
	//////////////////////////////////////////////////////////////////////////
	// SOME USEFULL TEMPLATE
	//////////////////////////////////////////////////////////////////////////

	/**
	 * Delete all elements from a list of pointers.
	 * @param L List of pointers to delete.
	 */
	template<typename T>
	inline void ClearList(std::list<T*>& L)
	{
		typename std::list<T*>::iterator it = L.begin();
		typename std::list<T*>::iterator it2;
		while (it != L.end()) {
			it2 = it++;
			delete (*it2);
			L.erase(it2);
		}
	}

	template<typename T>
	inline void ClearList(CList<T*, T*>& L)
	{
		T* t = 0;
		POSITION pos;

		pos = L.GetHeadPosition();	
		while (pos) {

			t = L.GetNext(pos);
			if (t) {
				delete (t);
				t = 0;
			}
		}
		L.RemoveAll();
	}

	template<typename T>
	inline void ClearPtrList(CTypedPtrList<CObList, T*>& L)
	{
		T* t = 0;
		POSITION pos;

		pos = L.GetHeadPosition();	
		while (pos) {

			t = L.GetNext(pos);
			if (t) {
				delete (t);
				t = 0;
			}
		}
		L.RemoveAll();
	}

	/**
	 * Delete all elements from a array of pointers.
	 * @param A Array of pointers to delete.
	 */
	template<typename T>
	inline void ClearArray(CArray<T*, T*>& A)
	{
		T* t = 0;
		
		for (int i = 0; i < A.GetSize(); i++) {

			t = A.GetAt(i);
			if (t) {
				delete (t);
			}
		}

		A.RemoveAll();
	}

	/**
	 * Copy all elements from an array of pointers.
	 * @param D Destination array of pointers.
	 * @param S Source array of pointers.
	 */
	template<typename T>
	inline void CopyArray(CArray<T*, T*>& D, CArray<T*, T*>& S, BOOL bClearDes = TRUE)
	{
		T* t;

		// Clear destination.
		if (bClearDes) {
			DMCUtil::ClearArray(D);
		}

		// Clone array.
		for (int i = 0; i < S.GetSize(); i++) {
			t = new T(*S.GetAt(i));
			if (t) {
				D.Add(t);
			}
		}
	}

	/**
	 * Copy all elements from a map of pointers.
	 * @param D Destination array of pointers.
	 * @param S Source array of pointers.
	 */
	template<typename Key, typename ArgKey, typename Value, typename ArgValue>
	inline void CopyMap(CMap<Key, ArgKey, Value*, ArgValue*>& D, CMap<Key, ArgKey, Value*, ArgValue*>& S, BOOL bClearDes = TRUE)
	{
		Value *v = 0, *v1;
		Key k;
		POSITION pos;

		// Clear destination.
		if (bClearDes) {
			DMCUtil::ClearMap(D);
		}

		pos = S.GetStartPosition();
		while (pos) {
			S.GetNextAssoc(pos, k, v);
			if (v) {
				v1 = new Value(*v);
				if (v1) {
					D.SetAt(k, v1);
				}
			}
		}
	}

	/**
	 * Delete all associated values from a map (not the key elements).
	 * @param M Map of pointer values to delete.
	 */
	template<typename Key, typename T>
	inline void ClearMap(std::map<Key, T*>& M)
	{
		typename std::map<Key, T*>::iterator it = M.begin();
		typename std::map<Key, T*>::iterator it2;
		while (it != M.end()) {
			it2 = it++;
			delete (*it2).second;
			M.erase(it2);
		}
	}

	/**
	 * Delete all associated values from a map (not the key elements).
	 * Const key type version.
	 * @param M Map of pointer values to delete.
	 */
	template<typename Key, typename T>
	inline void ClearMap(std::map<const Key, T*>& M)
	{
		typename std::map<const Key, T*>::iterator it = M.begin();
		typename std::map<const Key, T*>::iterator it2;
		while (it != M.end()) {
			it2 = it++;
			delete (*it2).second;
			M.erase(it2);
		}
	}

	/**
	 * Delete all associated values from a map (not the key elements).
	 * Const key type version.
	 * @param M Map of pointer values to delete.
	 */
	template<typename Key, typename ArgKey, typename Value, typename ArgValue>
	inline void ClearMap(CMap<Key, ArgKey, Value*, ArgValue*>& M)
	{
		Value* v = 0;
		Key k;
		POSITION pos;

		pos = M.GetStartPosition();
		while (pos) {
			M.GetNextAssoc(pos, k, v);
			if (v) {
				delete v;
				v = 0;
			}
		}
		M.RemoveAll();
	}

	//////////////////////////////////////////////////////////////////////////
	// FILE PATH
	//////////////////////////////////////////////////////////////////////////

	CString	GetFileName(CString szFilePath);
	CString	GetFileNameNoExt(CString szFilePath);
	CString	GetFileExtension(CString szFileName);
	CString GetFolderPath(CString szFilePath);

	std::string	GetFileName(std::string szFilePath);
	std::wstring GetFileNameW(LPCWSTR szFilePath);
	std::string	GetFileNameNoExt(std::string szFilePath);
	std::string	GetFileExtension(std::string szFileName);

	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////

	/**
	 * Get the full path of the executable file of the current process.
	 *
	 * @author: Minh Huynh
	 * @param: 
	 * @return: The full path of the executable file.
	 */
	CString GetModuleFileNameEx();

	/**
	 * Get the newest (latest) file in specified directory.
	 *
	 * @author: Minh Huynh
	 * @param:	szPath		The specified directory
	 * @param:	szFileExt	The extension of the finding file
	 * @param:	listFile	The list of file in the specified directory
	 * @return: The full path of the latest file.
	 */
	CString GetLatestFile(CString szPath, CString szFileExt, CStringArray& listFile);

	/**
	 * Get the directory of the executable file of the current process.
	 *
	 * @author: Minh Huynh
	 * @param: szFilePath The full path of the executable file
	 * @return: The directory of the executable file.
	 */
	CString GetDirFromFullPath(CString szFilePath);	
	std::string GetDirFromFullPath(std::string szFilePath);	

	/**
	 * Return the executable icon file of system base on file extension.
	 * @author: Dung
	 * @param: CString szExt
	 * @param: BOOL bIconSize
	 * @param: BOOL bIsDirectory
	 * @return: HICON
	 */
	HICON	GetFileIcon(CString szExt, BOOL bIconSize, BOOL bIsDirectory = FALSE);

	BOOL	CheckFileExist(LPCTSTR szPath);	
	BOOL	IsDirectory(LPCTSTR szPath);
	
	BOOL	GetFileSize(CString szFilePath, ULONG& iFileSize);

	BOOL	ReadFile(CString szFilePath, std::string& szOutData);
	BOOL	ReadFile(CString szFilePath, CString& szOutData);
	
	BOOL	WriteFile(CString szInData, CString szFilePath);
	BOOL	WriteFile(std::string szInData, CString szFilePath);

	std::string GetFileExErrorMsg(CFileException* e, BOOL bDelete = FALSE);

	/**
	* Create full file path include its folder.
	* Note: file is not created.
	*
	* @author: Giang Nguyen
	* @param: CString szFilePath
	* @return: CString
	*/
	CString	CreateFolderOfFilePath(const CString szFilePath);
	BOOL	CreateFile(CString szFilePath);	
	BOOL	DeleteFile(CString szFilePath, bool bAskReadOnly = true);
	
	/**
	 * If don't set temp directory, it will use the environment temporary directory.
	 * @author: Giang Nguyen
	 * @param: CString szPrefixName
	 * @param: CString szTempDir
	 * @return: CString
	 */
	CString	CreateTempFile(CString szPrefixName, CString szTempDir = _T(""));
	std::string	CreateTempFile(std::string szPrefixName, std::string szTempDir = "");

	int		CreateFolderPath(const CString szFolderPath);
	BOOL	DeleteFolder(CString szFolderPath, bool bAskReadOnly = true);
	BOOL	ClearFolder(CString szFolderPath, bool bAskReadOnly = true);

	void	ClearFileInFolder(CString szFolderPath, CStringArray &arrExceptFileTitles);
	BOOL	IsExistInStringArray(CString szName, CStringArray &arrNames);

	DWORD	GetListOfFileInDir(CString szDirPath, CStringArray &fileList);

	//////////////////////////////////////////////////////////////////////////
	// FILETIME Utility
	//////////////////////////////////////////////////////////////////////////

	/**
	 * Create datetime session identify used for saving to history message.
	 *
	 * @author: Giang Nguyen
	 * @return: LONGLONG
	 */
	LONGLONG CreateLLONGSessionID();
	LONGLONG CreateLLONGDateTime(BOOL miliSleep = FALSE);
	LONGLONG CreateLLONGDateTime(SYSTEMTIME systemTime);
	LONGLONG CreateLLONGMSecDateTime();
	LONGLONG CreateLLONGSecDateTime();
	
	/*
	 * Create Unix datetime, UTC, number of seconds since 1970-01-01 
	 *  (http://en.wikipedia.org/wiki/Unix_time)
	 * @author: Ninh Sai
	 * @return: LONGLONG
	 */
	LONGLONG CreateLLONGUnixDateTime();
	LONGLONG CreateLLONGUnixMSecDateTime();
	LONGLONG CreateLLONGUnixDateTime(FILETIME pTimeStamp);

	/*
	 * [DateTime] Convert unix date time to application (DMC) string format
	 * @author: Ninh Sai
	 * @param: LONGLONG llUnixDateTime
	 * @param: BOOL bCompact
	 * @param: BOOL bSecond
	 * @param: CString szAMText The localize text
	 * @param: CString szPMText The localize text
	 * @return: CString
	 */
	CString DateTimeUnix2AppStr(LONGLONG llUnixDateTime, 
								BOOL bCompact, BOOL bSecond,
								CString szAMText, CString szPMText);
	
	/*
	 * [DateTime] Convert XMPPTime, YYYYMMDDTHH:mm:ss (20110603T06:33:55), To Unix time (both are +0)
	 * @author: Ninh Sai
	 * @param: std::string datetime
	 * @return: LONGLONG
	 */
	LONGLONG DateTimeXMPP2Unix(std::string datetime);

	/*
	 * [DateTime] Time different between local and UTC
	 * @author: Ninh Sai
	 * @return: LONGLONG
	 */
	LONGLONG TimeDiffLocalAndUTC();

	/*
	 * [DateTime]Convert date time from SQL format (YYYY-mm-dd hh:mm:ss)
	 *  to Unix time (number seconds from Epoch) 
	 * @author: Ninh Sai
	 * @param: CString szDateTime
	 * @return: LONGLONG
	 */
	LONGLONG DateTimeSQL2Unix(CString szDateTime);

	/*
	 * [DateTime] Convert date time from Unix time
	 *  to SQL format (YYYY-mm-dd hh:mm:ss)
	 * @author: Ninh Sai
	 * @param: LONGLONG llDateTimeUnix
	 * @return: CString
	 */
	CString DateTimeUnix2SQL(LONGLONG llDateTimeUnix);
	CString RemoveLeadingZero(CString szFormat, CString szDateTime, BOOL bOnlyHour = TRUE);

	/**
	* The following function converts a filetime in the UNIX time_t format to a Win32 FILETIME format. 
	* Note that time_t is a 32-bit value and FILETIME is a 64-bit structure, 
	* so the Win32 function, Int32x32To64() is used in the following function:
	* @author: Huy Nguyen
	* @param: time_t t
	* @param: LPFILETIME pft
	* @return: void
	*/
	void	UnixTimeToFileTime(time_t t, LPFILETIME pft);

	/**
	* Once the UNIX time is converted to a FILETIME structure, other Win32 time formats 
	* can be easily obtained by using Win32 functions such as 
	* FileTimeToSystemTime() and FileTimeToDosDateTime().
	* @author: Huy Nguyen
	* @param: time_t t
	* @param: LPSYSTEMTIME pst
	* @return: void
	*/
	void	UnixTimeToSystemTime(time_t t, LPSYSTEMTIME pst);

	/**
	 * Create datetime from system time
	 * @author: Huy Nguyen
	 * @param: SYSTEMTIME systemTime
	 * @return: LONGLONG
	 */
	LONGLONG	CreateLLONGDateTime(FILETIME pTimeStamp);

	SYSTEMTIME	GetCurrentSystemTime();
	SYSTEMTIME	ToSystemTime(LONGLONG llDateTime);
	FILETIME	ToFileTime(LONGLONG llDateTime);
	LONGLONG	ConvertTimestampToLLONG(std::string datetime);		//YYYY-MM-DDTHH::ssZ
	LONGLONG	ConvertTimestampToLLONG(int year, int month, int date);

	/**
	 * Convert from GMT time to Local time, use Local Time Zone.
	 * 
	 * @author: Huy Nguyen
	 * @return: time_t
	 */
	time_t		GMTToLocalTime();
	std::string ConvertUTCToLocalTime(std::string datetime);

	// Compare time, but don't care hour, minutes, second
	int		CompareTimeOnlyDate(const CTime &t1, const CTime &t2);
	int		CompareTimeOnlyTime(const CTime &t1, const CTime &t2);

	int		CompareTimeOnlyDate(const COleDateTime &t1, const COleDateTime &t2);
	int		CompareTimeOnlyTime(const COleDateTime &t1, const COleDateTime &t2);

	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////

	/**
	 * Load a IDS string in string table.
	 *
	 * @author: Giang Nguyen
	 * @param: UINT uText
	 * @return: CString
	 */
	CString	LoadIDString(const UINT uText);

	/**
	 * Check input string is in string array
	 * @author: Dung
	 * @param: CString input
	 * @param: CStringArray
	 * @return: BOOL
	 */
	BOOL	IsInStringArray(const CString input, const CStringArray& strArray);

	/**
	 * Create a message session ID.
	 *
	 * @author: Giang Nguyen
	 * @return: std::string
	 */
	std::string	CreateMessageSessionID(std::string fullJid);

	/**
	 * Create MUC Room JID.
	 *
	 * @author: Giang Nguyen
	 * @param: CString szMUCService
	 * @param: CString szRoomName
	 * @param: CString szRoomNick
	 * @return: CString
	 */
	CString	CreateMUCRoomJID(CString szMUCService, CString szRoomName, CString szRoomNick);

	/**
	 * Set session JID for mail alert.
	 * @author: Giang Nguyen
	 * @param: CString szKey
	 * @return: CString
	 */
	CString CreateAlertMailSessionJID(CString szKey = _T(""));

	/**
	 * This function generates a random port in the range
	 *
	 * @author:	Minh Huynh
	 * @param:	iMin The minimum value of the range for generation
	 * @param:	iMax The maximum value of the range for generation
	 * @return:	A random port as an integer number.
	 */

	int		CreateRandomNumber(int iMin, int iMax);
	
	//////////////////////////////////////////////////////////////////////////
	// WORDWRAP AND CSTRING UTILITIES
	//////////////////////////////////////////////////////////////////////////

	/**
	 * Return min index of one of char in findChars array in input string
	 * @author: Dung
	 * @param: int startIndex
	 * @param: CString input
	 * @param: int findCharsLength
	 * @param: TCHAR * findChars
	 * @return: int
	 */
	int		FindCharIndex(int startIndex, CString input, int findCharsLength, TCHAR* findChars);

	/**
	 * Return string from start index to maxIndex
	 * @author: Dung
	 * @param: CString input
	 * @param: int maxIndex
	 * @return: CString
	 */
	CString GetBreakedPart(CString input, int maxIndex);

	// Separate szData to array string, then return a given string at iIndex.
	CString GetDataAtIndex(CString szData, CString szSeparator, int iIndex);

	/**
	 * Split string by findChars and insert to letter array.
	 *
	 * @author: Giang Nguyen
	 * @param: const CString szInput
	 * @param: const CString szDelimiter
	 * @param: CStringArray & DataArray
	 * @return: int
	 */
	int		SplitString(const CString szInput, const CString szDelimiter, CStringArray& DataArray);
	void	SplitStringEx(CStringArray& LetterArray, CString szInput, int iDelimeterLength, TCHAR* pzDelimeter, BOOL bExtractSplit);

	int		CStringFindCount(CString& szData, LPCTSTR szPattern);

	//////////////////////////////////////////////////////////////////////////
	// FILE SIZE FORMAT
	//////////////////////////////////////////////////////////////////////////

	/**
	 * Generate file size format.
	 * @author: Giang Nguyen
	 * @param: UINT size
	 * @return: CString
	 */
	CString GetRateView(UINT size);

	/**
	 * Convert CString to const char*
	 * @author: Huy Nguyen
	 * @param: LPCWSTR szInName
	 * @param: ULONG nCodePage
	 * @return: LPSTR
	 */
	LPSTR	WC2MB(LPCWSTR szInName, ULONG nCodePage);

	/**
	 * 
	 * @author: Dung
	 * @param: bool condition
	 * @return: void
	 */
	void	DMCAssert(BOOL condition);

	/**
	 * 
	 * @author: Dung
	 * @param: BOOL condition
	 * @return: void
	 */
	void	DMCVerify(BOOL condition);

	/**
	 * Execute a external program.
	 * @author: Giang Nguyen
	 * @param: CString szExeName
	 * @param: CString szArguments
	 * @param: CString szExeDir
	 * @return: CString
	 */
	CString ExecuteExternalProg(CString szExeName, CString szArguments, CString szExeDir);

	/**
	 * Create a shortcut to specified object.
	 *
	 * @author: Minh Huynh + Thach
	 * @param: CString szExeName
	 * @param: CString szArguments
	 * @param: CString szExeDir
	 * @return: 
	 */
	HRESULT CreateLink(LPCWSTR lpszPathObj, LPCWSTR lpszPathLink, LPCWSTR lpszIcon, LPCWSTR lpszDesc);

	/**
	 * Check OS version.
	 * @author: Giang Nguyen
	 * @return: BOOL
	 */
	BOOL	IsWindowsXPorLater();

	/**
	 * Get All User Application Data Directory of Window XP or later OS.
	 * @author: Giang Nguyen
	 * @return: CString
	 */
	CString GetWindowAllUserAppDataDir();	

	/**
	 * Get Current User My Document Directory.
	 * @author: Giang Nguyen
	 * @return: CString
	 */
	CString GetWindowCurUserDocumentDir();

	/**
	 * Compare version of application.
	 * Format: 1.0.x
	 *
	 * @author: Giang Nguyen
	 * @param: CString szCurVersion
	 * @param: CString szNewVersion
	 * @return: int
	 */
	int		CompareVersion(CString szCurVersion, CString szNewVersion, int iLevel = 3);
	int		CompareINT(int a, int b);
	int		StrCompare(const char* str1, const char* str2);

	/**
	 * Convert COLORREF to string hexadecimal
	 * @author: Huy Nguyen
	 * @param: int num
	 * @return: std::string
	 */
	std::string	ConvertRGBtoHex(int num);
	std::string	ConvertRGBtoHex(int r, int g, int b);
	std::string	ConvertRGBtoHex(COLORREF color);

	/**
	 * Update des string to src string if they are different.
	 * @author: Giang Nguyen
	 * @param: CString & szSrc
	 * @param: CString & szDes
	 * @return: BOOL
	 */
	BOOL	UpdateString(CString& szSrc, CString szDes);
	BOOL	UpdateInt(int& iSrc, int iDes);

	/**
	 * Return current time with provided format
	 * @author: Dung
	 * @param: CString szFormat
	 * @return: CString
	 */
	CString GetCurrentDateTime(CString szFormat);

	/*
	 * Concatenate array of strings into a string, with specified delimiter
	 *  and quote (put into '') if specified.
	 * @author: Ninh Sai
	 * @param: CStringArray & arr
	 * @param: BOOL bQuote
	 * @param: CString szDelimiter
	 * @return: CString
	 */
	CString	Concat(CStringArray& arr, BOOL bQuote = FALSE, CString szDelimiter = _T(","));

	//////////////////////////////////////////////////////////////////////////
	// Escape and relax HTML.
	// Ref: Gloox lib (tag.h,.cpp)
	//////////////////////////////////////////////////////////////////////////

	/**
	 * Does some fancy escaping. (& --> &amp;, etc).
	 * @param what A string to escape.
	 * @deprecated Will be removed in 1.0.
	 */
	const std::string Escape(std::string what);
	CString Escape(CString what);

	/**
	 * Reverses operation of escape(). (&amp; --> &).
	 * @param what A string to de-escape.
	 * @deprecated Will be removed in 1.0.
	 */
	const std::string Relax(std::string what);
	CString Relax(CString what);

	// Perform birthday data.
	CString FixBirthDay(CString szBirthDay, CString szFormat);
	BOOL	IsValidBirthDay(CString szBirthDay);
	BOOL	IsBirthDayToday(CString szBirthDay);

	/**
	 * http://msdn.microsoft.com/en-us/library/windows/desktop/aa363860%28v=VS.85%29.aspx
	 * http://msdn.microsoft.com/en-us/library/windows/desktop/aa363858%28v=VS.85%29.aspx
	 * http://msdn.microsoft.com/en-us/library/windows/desktop/aa363855%28v=VS.85%29.aspx
	 *
	 * Create folder with user full control ACL
	 * @author: Giang Nguyen
	 * @param: LPCTSTR lpPath
	 * @return: BOOL
	 */
	BOOL	CreateFolderWithEveryOneACL(LPCTSTR lpPath);
	BOOL	CreateFileWithEveryOneACL(LPCTSTR lpPath, BOOL bSetPerm = TRUE);

	// Use for both: file & folder.
	BOOL	SetFileSecurityWithEveryOneACL(HANDLE &hFile);
	BOOL	SetFileSecurityWithEveryOneACL(LPCTSTR lpPath);

	/**
	 * Save bitmap to file.
	 * @author: Giang Nguyen
	 * @param: const char * szFilename
	 * @param: HBITMAP hBitmap
	 * @return: BOOL
	 */
	BOOL	SaveBitmapFile(const char *szFilename, HBITMAP hBitmap);

	/**
	 * Enable flash window extension.
	 * @author: Huy Nguyen
 	 * @param: HWND hWnd
	 * @param: DWORD dwFlags
	 * @return: BOOL
	 */
	BOOL	EnableFlashEx(HWND hWnd, DWORD dwFlags);

	/**
	 * Release thread handle.
	 * @author: Giang Nguyen
	 * @param: HANDLE &hThread
	 * @param: DWORD dwMiliseconds
	 * @return: void
	 */
	void	ReleaseThread(HANDLE &hThread, DWORD dwMiliseconds = -1);

	/**
	 * http://msdn.microsoft.com/en-us/library/windows/desktop/ms686219%28v=vs.85%29.aspx
	 * @author: Giang Nguyen
	 * @param: DWORD dwPriorityClass
	 * @return: void
	 */
	void	SetCurrentProcessPriority(DWORD dwPriorityClass);

	/**
	 * Restore DubuDisk path in HTML message content
	 * 
	 * @author: Huy Nguyen
	 * @param: CString & szHTMLContent
	 * @param: CLogger * m_pLogger
	 * @return: void
	 */
	void	RestoreDubuDiskPath(CString &szHTMLContent, CLogger *m_pLogger);
	void	RestoreDubuDisk_iPath(CString &szHTMLContent, CLogger *m_pLogger);
	void	RestoreDubuDisk_ePath(CString &szHTMLContent, CLogger *m_pLogger);

	int		RestoreDubuDiskLink(CString szHref, CString &szResPath, int &iType, CLogger *m_pLogger);
	int		RestoreDubuDisk_iLink(CString szHref, CString &szResPath, CLogger *m_pLogger);
	int		RestoreDubuDisk_eLink(CString szHref, CString &szResPath, CLogger *m_pLogger);

	BOOL	IsDubuDiskPath(CString szData);
	BOOL	IsDubuDiskPath(CString szData, CString szMountPoint);

	CString RemoveBeginningSlash(CString str);
	CString RemoveTrailingSlash(CString str);

	CString Convert2FilePath(CString szFilePath);
	BOOL	BrowseAndSelectFilePath(CString szFilePath);
	BOOL	Browse2FolderPath(CString szFolderPath);

	//////////////////////////////////////////////////////////////////////////
	// Font utilities
	//////////////////////////////////////////////////////////////////////////

	void	CopyFont(CFont *pSrcFont, CFont *pDesFont);

	//////////////////////////////////////////////////////////////////////////
	// Color utilities
	//////////////////////////////////////////////////////////////////////////

	COLORREF StrToColor(CString str);
	std::string ColorToStr(COLORREF color);

	// Transparent color
	COLORREF TransparentColor(COLORREF clr, int alpha);
};

#endif // !defined(AFX_DMCUTIL_H__9CF8288E_90BE_4EA7_962C_9E92C61F3D13__INCLUDED_)
