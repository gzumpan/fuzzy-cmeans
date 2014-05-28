//
// FileRotator.h : Declare the FileRotator class.
//
// FileRotator class will open a file, every time before writing in file, it will
// check for the file size, if it is more than a defined limit. It will rename the
// current file by appending 01, 02 ... etc. up to a kMaxFileCount and create a new file.
// When we will reach to the maximum file count, it will will start deleting the old files.
//
// Note: File size check is approximate. It will just verify file size before writing into the file.
//
// Author: Manish K Agarwal (manish.k.agarwal@gmail.com)
//
// History:
//		18 March 2009 : Initial revision.
//
// Ref: http://www.codeproject.com/Articles/34203/File-Rotator

#ifndef __FILEROTATOR_H
#define __FILEROTATOR_H

#include <string>

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

class CFileRotator
{
public:
	CFileRotator();
	virtual ~CFileRotator()
	{
		if (m_fp) {
			fclose(m_fp);
			m_fp = NULL;
		}
	}

	// Initialize
	void	Initialize(const char *pszFilePath, int maxFileSizeInBytes, int maxFileCount);

	// Open/Close the file, if required get the next name.
	bool	Open();
	void	Close();

	int		Write2File(const char *pszData);

protected:
	// Check if we write more in current opened file.
	bool	IsMoreWriteAllowed() const;

	// PrepareToWrite: validate file size, if required, rename existing file and reopen a new file.
	void	PrepareToWrite();

	// Rename existing file name to next file name. Also delete old file if required.
	void	RenameFile();

protected:
	FILE*				m_fp;

	int					m_iMaxFileSize; // In bytes

	std::string			m_szFilePath;
	std::string			m_szBaseFileName;
	std::string			m_szFileExtn;

	int					m_iNewRevisionCount;
	int					m_iDeleteRevisionCount;

	int					m_kMaxFileCount;
	char				m_szFormatStr[64];
};

#endif //__FILEROTATOR_H
