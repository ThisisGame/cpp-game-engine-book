/*******************************************************************************
The content of this file includes portions of the AUDIOKINETIC Wwise Technology
released in source code form as part of the SDK installer package.

Commercial License Usage

Licensees holding valid commercial licenses to the AUDIOKINETIC Wwise Technology
may use this file in accordance with the end user license agreement provided 
with the software or, alternatively, in accordance with the terms contained in a
written agreement between you and Audiokinetic Inc.

  Version: v2021.1.5  Build: 7749
  Copyright (c) 2006-2021 Audiokinetic Inc.
*******************************************************************************/
//////////////////////////////////////////////////////////////////////
//
// AkFileLocationBase.cpp
//
// Basic file location resolving: Uses simple path concatenation logic.
// Exposes basic path functions for convenience.
// For more details on resolving file location, refer to section "File Location" inside
// "Going Further > Overriding Managers > Streaming / Stream Manager > Low-Level I/O"
// of the SDK documentation. 
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include <AK/SoundEngine/Common/AkStreamMgrModule.h>
#include <AK/Tools/Common/AkPlatformFuncs.h>
#ifdef AK_SUPPORT_WCHAR
#include <wchar.h>
#endif //AK_SUPPORT_WCHAR
#include <stdio.h>
#include <AK/Tools/Common/AkAssert.h>
#include <AK/Tools/Common/AkObject.h>

#include "AkFileHelpers.h"
#include "AkMultipleFileLocation.h"

#define MAX_NUMBER_STRING_SIZE      (10)    // 4G
#define ID_TO_STRING_FORMAT_BANK    AKTEXT("%u.bnk")
#define ID_TO_STRING_FORMAT_WEM     AKTEXT("%u.wem")
#define MAX_EXTENSION_SIZE          (4)     // .xxx
#define MAX_FILETITLE_SIZE          (MAX_NUMBER_STRING_SIZE+MAX_EXTENSION_SIZE+1)   // null-terminated

void CAkMultipleFileLocation::Term()
{
	if (!m_Locations.IsEmpty())
	{
		FilePath *p = (*m_Locations.Begin());
		while (p)
		{
			FilePath *next = p->pNextLightItem;
			AkDelete(AkMemID_Streaming, p);
			p = next;
		}
	}
	m_Locations.Term();
}

AKRESULT CAkMultipleFileLocation::Open(
	const AkOSChar* in_pszFileName,     // File name.
	AkOpenMode      in_eOpenMode,       // Open mode.
	AkFileSystemFlags * in_pFlags,      // Special flags. Can pass NULL.
	bool			in_bOverlapped,		// Overlapped IO open
	AkFileDesc &    out_fileDesc        // Returned file descriptor.
	)
{
	AkOSChar szFullFilePath[AK_MAX_PATH];
	if (m_Locations.IsEmpty())
	{
		// Get the full file path, using path concatenation logic.

		if (GetFullFilePath(in_pszFileName, in_pFlags, in_eOpenMode, szFullFilePath ) == AK_Success)
		{
			AKRESULT res = CAkFileHelpers::Open(szFullFilePath, in_eOpenMode, in_bOverlapped, out_fileDesc);
			if (res == AK_Success)
			{
				// iFileSize must be set by the OpenPolicy.
				AKASSERT(out_fileDesc.iFileSize != 0 && (in_eOpenMode == AK_OpenModeRead || in_eOpenMode == AK_OpenModeReadWrite) || !(in_eOpenMode == AK_OpenModeRead || in_eOpenMode == AK_OpenModeReadWrite));
				return AK_Success;
			}
		}
	}
	else
	{
		for (AkListBareLight<FilePath>::Iterator it = m_Locations.Begin(); it != m_Locations.End(); ++it)
		{
			// Get the full file path, using path concatenation logic.

			if (GetFullFilePath(in_pszFileName, in_pFlags, in_eOpenMode, szFullFilePath, (*it)->szPath) == AK_Success)
			{
				AKRESULT res = CAkFileHelpers::Open(szFullFilePath, in_eOpenMode, in_bOverlapped, out_fileDesc);
				if (res == AK_Success)
				{
					// iFileSize must be set by the OpenPolicy.
					AKASSERT(out_fileDesc.iFileSize != 0 && (in_eOpenMode == AK_OpenModeRead || in_eOpenMode == AK_OpenModeReadWrite) || !(in_eOpenMode == AK_OpenModeRead || in_eOpenMode == AK_OpenModeReadWrite));
					return AK_Success;
				}
			}
		}
	}
	return AK_Fail;    
}

AKRESULT CAkMultipleFileLocation::Open( 
										AkFileID        in_fileID,          // File ID.
										AkOpenMode      in_eOpenMode,       // Open mode.
										AkFileSystemFlags * in_pFlags,      // Special flags. Can pass NULL.
										bool			in_bOverlapped,		// Overlapped IO open
										AkFileDesc &    out_fileDesc        // Returned file descriptor.
										)
{
	if ( !in_pFlags ||
		!(in_pFlags->uCompanyID == AKCOMPANYID_AUDIOKINETIC || in_pFlags->uCompanyID == AKCOMPANYID_AUDIOKINETIC_EXTERNAL))
	{
		AKASSERT( !"Unhandled file type" );
		return AK_Fail;
	}

	AkOSChar pszTitle[MAX_FILETITLE_SIZE+1];
	AkMultipleFileLocation::ConvertFileIdToFilename(pszTitle, MAX_FILETITLE_SIZE, in_pFlags->uCodecID, in_fileID);

	return Open(pszTitle, in_eOpenMode, in_pFlags, in_bOverlapped, out_fileDesc);
}

// String overload.
// Returns AK_Success if input flags are supported and the resulting path is not too long.
// Returns AK_Fail otherwise.
AKRESULT CAkMultipleFileLocation::GetFullFilePath(
	const AkOSChar*		in_pszFileName,		// File name.
	AkFileSystemFlags * in_pFlags,			// Special flags. Can be NULL.
	AkOpenMode			in_eOpenMode,		// File open mode (read, write, ...).
	AkOSChar*			out_pszFullFilePath, // Full file path.
	AkOSChar*			in_pszBasePath		// Base path to use, might be null	
	)
{
    if ( !in_pszFileName )
    {
        AKASSERT( !"Invalid file name" );
        return AK_InvalidParameter;
    }

	// Prepend string path (basic file system logic).

    // Compute file name with file system paths.
	size_t uiPathSize = AKPLATFORM::OsStrLen( in_pszFileName );

    if ( uiPathSize >= AK_MAX_PATH )
	{
		AKASSERT( !"Input string too large" );
		return AK_InvalidParameter;
	}

	if (in_pszBasePath != NULL)
	{
		AKPLATFORM::SafeStrCpy(out_pszFullFilePath, in_pszBasePath, AK_MAX_PATH);
	}
	else
	{
		if (m_Locations.IsEmpty())
		{
			out_pszFullFilePath[0] = 0;
		}
		else
		{
			AkListBareLight<FilePath>::Iterator it = m_Locations.Begin();
			AKPLATFORM::SafeStrCpy(out_pszFullFilePath, (*it)->szPath, AK_MAX_PATH);
		}
	}


    if ( in_pFlags && in_eOpenMode == AK_OpenModeRead )
    {        
		// Add language directory name if needed.
		if ( in_pFlags->bIsLanguageSpecific )
		{
			size_t uLanguageStrLen = AKPLATFORM::OsStrLen( AK::StreamMgr::GetCurrentLanguage() );
			if ( uLanguageStrLen > 0 )
			{
				uiPathSize += ( uLanguageStrLen + 1 );
				if ( uiPathSize >= AK_MAX_PATH )
				{
					AKASSERT( !"Path is too large" );
					return AK_Fail;
				}
				AKPLATFORM::SafeStrCat( out_pszFullFilePath, AK::StreamMgr::GetCurrentLanguage(), AK_MAX_PATH );
				AKPLATFORM::SafeStrCat( out_pszFullFilePath, AK_PATH_SEPARATOR, AK_MAX_PATH );
			}
		}
	}
        
    // Append file title.
	uiPathSize += AKPLATFORM::OsStrLen( out_pszFullFilePath );
	if ( uiPathSize >= AK_MAX_PATH )
	{
		AKASSERT( !"File name string too large" );
		return AK_Fail;
	}
	AKPLATFORM::SafeStrCat( out_pszFullFilePath, in_pszFileName, AK_MAX_PATH );
	return AK_Success;
}

AKRESULT CAkMultipleFileLocation::AddBasePath(const AkOSChar* in_pszBasePath)
{
	AkUInt32 origLen = (AkUInt32)AKPLATFORM::OsStrLen(in_pszBasePath);
    AkUInt32 newByteSize = origLen + 1;	// One for the trailing \0
    if(origLen && in_pszBasePath[origLen - 1] != AK_PATH_SEPARATOR[0])
    {
        newByteSize++; // Add space for a trailing slash
    }

    // Make sure the new base path is not too long in case language gets appended
    // Format of the test is: basePath/Language/.
	if ( origLen + 1 + AKPLATFORM::OsStrLen( AK::StreamMgr::GetCurrentLanguage() + 1 ) >= AK_MAX_PATH )
		return AK_InvalidParameter;

	FilePath * pPath = (FilePath*)AkAlloc(AkMemID_Streaming, sizeof(FilePath) + sizeof(AkOSChar)*(newByteSize - 1));
	if (pPath == NULL)
		return AK_InsufficientMemory;

	// Copy the base path EVEN if the directory does not exist.
	AKPLATFORM::SafeStrCpy( pPath->szPath, in_pszBasePath, origLen + 1);
    
    // Add the trailing slash, if necessary
	if (origLen)// UWP is not soo happy of the trailing slash on an empty string.
	{
		if (in_pszBasePath[origLen - 1] != AK_PATH_SEPARATOR[0])
		{
			pPath->szPath[origLen] = AK_PATH_SEPARATOR[0];
			pPath->szPath[origLen + 1] = 0;
		}
	}
	pPath->pNextLightItem = NULL;
	m_Locations.AddFirst(pPath);

	AKRESULT eDirectoryResult = CAkFileHelpers::CheckDirectoryExists( in_pszBasePath );
	if( eDirectoryResult == AK_Fail ) // AK_NotImplemented could be returned and should be ignored.
	{
		return AK_PathNotFound;
	}

	return AK_Success;
}

namespace AkMultipleFileLocation
{

void ConvertFileIdToFilename(AkOSChar * out_pszTitle, AkUInt32 in_pszTitleMaxLen, AkUInt32 in_codecId, AkFileID in_fileID)
{
	AKASSERT(in_pszTitleMaxLen >= MAX_FILETITLE_SIZE);

	if (in_codecId == AKCODECID_BANK)
		AK_OSPRINTF(out_pszTitle, in_pszTitleMaxLen, ID_TO_STRING_FORMAT_BANK, (unsigned int)in_fileID);
	else
		AK_OSPRINTF(out_pszTitle, in_pszTitleMaxLen, ID_TO_STRING_FORMAT_WEM, (unsigned int)in_fileID);
}

}
