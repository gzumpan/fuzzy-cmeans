//
// FileRotator.cpp : Define the FileRotator members.
//
// Author: Manish K Agarwal (manish.k.agarwal@gmail.com)
//
// History:
//		18 March 2009 : Initial revision.
//
//
//
#include "StdAfx.h"
#include "FileRotator.h"
#include "Windows.h"

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

CFileRotator::CFileRotator()
	: m_fp(NULL),
	  m_iMaxFileSize(0),
	  m_szFilePath(""),
	  m_szBaseFileName(""),
	  m_szFileExtn(""),
	  m_iNewRevisionCount(0),
	  m_iDeleteRevisionCount(0),
	  m_kMaxFileCount(0)
{
	memset(m_szFormatStr, 0, sizeof(m_szFormatStr));
}

void CFileRotator::Initialize(const char *pszFilePath, int maxFileSizeInBytes, int maxFileCount)
{
	size_t dotIndex;
	int numberOfDigits = 0;
	int maxCount = 1;

	// Set options.
	m_szFilePath	= pszFilePath;
	m_iMaxFileSize	= maxFileSizeInBytes;
	m_kMaxFileCount	= maxFileCount;

	// Find last position of . (dot)
	dotIndex = m_szFilePath.rfind(".");
	if (dotIndex != -1) {
		m_szBaseFileName	= m_szFilePath.substr(0, dotIndex);
		m_szFileExtn		= m_szFilePath.substr(dotIndex, m_szFilePath.length());
	} else {
		m_szBaseFileName	= m_szFilePath;
		m_szFileExtn		= "";
	}
	
	// Count the digits
	while (maxCount < m_kMaxFileCount) {
		numberOfDigits++;
		maxCount = maxCount*10;
	}
	
	// Set format string: Generate a string like "%s%02d%s"
	memset(m_szFormatStr, 0, sizeof(m_szFormatStr));
	sprintf(m_szFormatStr, "%s0%dd%s", "%s%", numberOfDigits, "%s");
}

int CFileRotator::Write2File(const char *pszData)
{
	// PrepareToWrite: validate file size, if required, rename existing file and reopen a new file.
	PrepareToWrite();

	// Write new string to file.
	return (fprintf(m_fp, "%s", pszData));
}

bool CFileRotator::Open()
{
	if (IsMoreWriteAllowed()) {
		// Open current log file to write.
		m_fp = fopen(m_szFilePath.c_str(), "a+");
	}
	else {
		// Rename it to unique name.
		RenameFile();

		// Now open a new log file to write.
		m_fp = fopen(m_szFilePath.c_str(), "w+");
	}

	return (m_fp != NULL);
}

void CFileRotator::Close()
{
	if (m_fp) {
		fclose(m_fp);
		m_fp = NULL;
	}
}

bool CFileRotator::IsMoreWriteAllowed() const
{
	FILE* pFile = NULL;
	long lFileSize = 0;
	
	pFile = fopen(m_szFilePath.c_str(), "rb");
	if (pFile) {
		fseek (pFile, 0, SEEK_END);
		lFileSize = ftell(pFile);
		fclose (pFile);
	}
	
	if (lFileSize < m_iMaxFileSize) {
		return (true);
	}
	
	return (false);
}

void CFileRotator::PrepareToWrite()
{
	if (!IsMoreWriteAllowed()) {

		// Close current file.
		fclose(m_fp);

		// Rename it to unique name.
		RenameFile();

		// Now open a new file to write.
		m_fp = fopen(m_szFilePath.c_str(), "w+");
	}
}

void CFileRotator::RenameFile()
{
	char szNewFileName[2048] = "";
	int i = 0;

	// Generate an available filename, if mNewRevisionCount count reached to its max limit
	// no need to reset it. We will delete a file and create new one.
	for (i = m_iNewRevisionCount; i <= m_kMaxFileCount; i++) {

		sprintf(szNewFileName, m_szFormatStr, m_szBaseFileName.c_str(), i, m_szFileExtn.c_str());

		// Rename logfile and keep a version of it with name file01.txt
		m_fp = fopen(szNewFileName, "r");
		if (!m_fp) {
			// File doesn't exist.
			rename(m_szFilePath.c_str(), szNewFileName);
			m_iNewRevisionCount = i + 1;
			break;

		} else {
			// if file exists close it and try for another file.
			fclose(m_fp);
		}
	} // for

	if (i > m_kMaxFileCount) {

		// Now we have to delete the first file i.e. 00 
		// TODO: check the time stamp of file and delete the oldest file first.
		sprintf(szNewFileName, m_szFormatStr, m_szBaseFileName.c_str(), m_iDeleteRevisionCount, m_szFileExtn.c_str());

		if (m_iDeleteRevisionCount < m_kMaxFileCount) {
			m_iDeleteRevisionCount++;
		} else {
			m_iDeleteRevisionCount = 0;
		}

		// Delete file file00.txt always
		remove(szNewFileName);

		// Rename current file to new name.
		rename(m_szFilePath.c_str(), szNewFileName);
	}
}
