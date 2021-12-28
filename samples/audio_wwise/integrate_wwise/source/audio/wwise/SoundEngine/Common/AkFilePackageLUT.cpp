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
// AkFilePackageLUT.cpp
//
// This class parses the header of file packages that were created with the 
// AkFilePackager utility app (located in ($WWISESDK)/samples/FilePackager/),
// and looks-up files at run-time.
// 
// The header of these file packages contains look-up tables that describe the 
// internal offset of each file it references, their block size (required alignment), 
// and their language. Each combination of AkFileID and Language ID is unique.
//
// The language was created dynamically when the package was created. The header 
// also contains a map of language names (strings) to their ID, so that the proper 
// language-specific version of files can be resolved. The language name that is stored
// matches the name of the directory that is created by the Wwise Bank Manager,
// except for the trailing slash.
//
//////////////////////////////////////////////////////////////////////


#include "stdafx.h"
#include "AkFilePackageLUT.h"
#include <AK/SoundEngine/Common/AkMemoryMgr.h>
#include <AK/SoundEngine/Common/AkSoundEngine.h>	// For string hash.
#include <AK/Tools/Common/AkPlatformFuncs.h>
#include <AK/Tools/Common/AkFNVHash.h>

#ifdef _DEBUG
	template<bool> struct AkCompileTimeAssert;
	template<> struct AkCompileTimeAssert<true> { };
	#define AK_STATIC_ASSERT(e) (AkCompileTimeAssert<(e) != 0>())
#else
	#define AK_STATIC_ASSERT(e)
#endif

#define AK_MAX_EXTERNAL_NAME_SIZE		260

CAkFilePackageLUT::CAkFilePackageLUT()
:m_curLangID( AK_INVALID_LANGUAGE_ID )
,m_pLangMap( NULL )
,m_pSoundBanks( NULL )
,m_pStmFiles( NULL )
,m_pExternals( NULL )
{
	AK_STATIC_ASSERT(sizeof(AkFileEntry<AkFileID>) == 20);
	AK_STATIC_ASSERT(sizeof(AkFileEntry<AkUInt64>) == 24);
}

CAkFilePackageLUT::~CAkFilePackageLUT()
{
}

// Create a new LUT from a packaged file header.
// The LUT sets pointers to appropriate location inside header data (in_pData).
AKRESULT CAkFilePackageLUT::Setup(
	AkUInt8 *			in_pData,			// Header data.
	AkUInt32			in_uHeaderSize		// Size of file package header.
	)
{
	struct FileHeaderFormat
	{
		char				headerDefinition[AKPK_HEADER_CHUNK_DEF_SIZE];
		AkUInt32			uVersion;
		AkUInt32			uLanguageMapSize;
		AkUInt32			uSoundBanksLUTSize;
		AkUInt32			uStmFilesLUTSize;
		AkUInt32			uExternalsLUTSize;
	};
	FileHeaderFormat * pHeader = (FileHeaderFormat*)in_pData;

	// Check header size,
	if ( in_uHeaderSize < sizeof(FileHeaderFormat)
			+ pHeader->uLanguageMapSize
			+ pHeader->uSoundBanksLUTSize
			+ pHeader->uStmFilesLUTSize
			+ pHeader->uExternalsLUTSize)
	{
		return AK_Fail;
	}

	// Check version.
	if ( pHeader->uVersion < AKPK_CURRENT_VERSION )
		return AK_Fail;

	// Get address of maps and LUTs.
	in_pData += sizeof(FileHeaderFormat);

	m_pLangMap		= (StringMap*)in_pData;
	in_pData += pHeader->uLanguageMapSize;

	m_pSoundBanks	= (FileLUT<AkFileID>*)in_pData;
	in_pData += pHeader->uSoundBanksLUTSize;

	m_pStmFiles		= (FileLUT<AkFileID>*)in_pData;
	in_pData += pHeader->uStmFilesLUTSize;

	m_pExternals	= (FileLUT<AkUInt64>*)in_pData;

	return AK_Success;
}

// Find a file entry by ID.
const CAkFilePackageLUT::AkFileEntry<AkFileID> * CAkFilePackageLUT::LookupFile(
	AkFileID			in_uID,			// File ID.
	AkFileSystemFlags * in_pFlags		// Special flags. Do not pass NULL.
	)
{
	AKASSERT( in_pFlags && in_pFlags->uCompanyID == AKCOMPANYID_AUDIOKINETIC );

	if ( in_pFlags->uCodecID == AKCODECID_BANK 
		&& m_pSoundBanks
		&& m_pSoundBanks->HasFiles() )
	{
		return LookupFile<AkFileID>( in_uID, m_pSoundBanks, in_pFlags->bIsLanguageSpecific );
	}
	else if ( m_pStmFiles && m_pStmFiles->HasFiles() )
	{
		// We assume that the file is a streamed audio file.
		return LookupFile<AkFileID>( in_uID, m_pStmFiles, in_pFlags->bIsLanguageSpecific );
	}
	// No table loaded.
	return NULL;
}

// Find a file entry by ID.
const CAkFilePackageLUT::AkFileEntry<AkUInt64> * CAkFilePackageLUT::LookupFile(
	AkUInt64			in_uID,			// File ID.
	AkFileSystemFlags * in_pFlags		// Special flags. Do not pass NULL.
	)
{
	AKASSERT( in_pFlags );

	if ( in_pFlags->uCompanyID == AKCOMPANYID_AUDIOKINETIC_EXTERNAL 
		&& m_pExternals 
		&& m_pExternals->HasFiles() )
	{
		return LookupFile<AkUInt64>( in_uID, m_pExternals, in_pFlags->bIsLanguageSpecific );
	}

	// No table loaded.
	return NULL;
}

// Set current language. 
// Returns AK_InvalidLanguage if a package is loaded but the language string cannot be found.
// Returns AK_Success otherwise.
AKRESULT CAkFilePackageLUT::SetCurLanguage(
	const AkOSChar*			in_pszLanguage	// Language string.
	)
{
	m_curLangID = AK_INVALID_LANGUAGE_ID;
	if ( m_pLangMap && in_pszLanguage )
	{
		AkUInt16 uLangID = (AkUInt16)m_pLangMap->GetID( in_pszLanguage );
		if ( uLangID == AK_INVALID_UNIQUE_ID 
			&& m_pLangMap->GetNumStrings() > 1 )	// Do not return AK_InvalidLanguage if package contains only SFX data.
		{
			return AK_InvalidLanguage;
		}
		m_curLangID = uLangID;
	}

	return AK_Success;
}

void CAkFilePackageLUT::RemoveFileExtension( AkOSChar* in_pstring )
{
	int i = (int)AKPLATFORM::OsStrLen(in_pstring) - 1;

	while (i >= 0)
	{
		if (in_pstring[i] == AKTEXT('.'))
		{
			in_pstring[i] = AKTEXT('\0');
			return;
		}

		i--;
	}
}

// Find a soundbank ID by its name. 
// Returns AK_INVALID_FILE_ID if no soundbank LUT is loaded.
AkFileID CAkFilePackageLUT::GetSoundBankID( 
	const AkOSChar*			in_pszBankName	// Soundbank name.
	)
{
	// Remove the file extension if it was used.
	AkUInt32 stringSize = (AkUInt32)AKPLATFORM::OsStrLen( in_pszBankName ) + 1;
	AkOSChar* pStringWithoutExtension = (AkOSChar *)AkAlloca( (stringSize) * sizeof( AkOSChar ) );
	AKPLATFORM::SafeStrCpy( pStringWithoutExtension, in_pszBankName, stringSize );
	RemoveFileExtension( pStringWithoutExtension );
	
	// Hash
	return AK::SoundEngine::GetIDFromString( pStringWithoutExtension );
}

AkUInt64 CAkFilePackageLUT::GetExternalID( 
	const AkOSChar*			in_pszExternalName		// External Source name.
	)
{
	char* szString; 
    CONVERT_OSCHAR_TO_CHAR(in_pszExternalName, szString);
	
	size_t stringSize = strlen( szString );

	// 1- Make lower case.
	_MakeLowerA( szString, stringSize );

	AK::FNVHash64 MainHash;
	return MainHash.Compute( (const unsigned char *) szString, (unsigned int)stringSize );
}

void CAkFilePackageLUT::_MakeLowerA( char* in_pString, size_t in_strlen )
{
	for( size_t i = 0; i < in_strlen; ++i )
	{
		if( in_pString[i] >= 'A' && in_pString[i] <= 'Z' )
		{
			in_pString[i] += 0x20;  
		}
	}
}

void CAkFilePackageLUT::_MakeLower( AkOSChar* in_pString )
{
	size_t uStrlen = AKPLATFORM::OsStrLen( in_pString );
	const AkOSChar CaseDiff = AKTEXT('a') - AKTEXT('A');
	for( size_t i = 0; i < uStrlen; ++i )
	{
		if( in_pString[i] >= AKTEXT('A') && in_pString[i] <= AKTEXT('Z') )
		{
			in_pString[i] += CaseDiff;
		}
	}
}

AkUInt32 CAkFilePackageLUT::StringMap::GetID( const AkOSChar* in_pszString )
{
	// Make string lower case.
	size_t uStrLen = AKPLATFORM::OsStrLen(in_pszString)+1;
	AkOSChar * pszLowerCaseString = (AkOSChar*)AkAlloca(uStrLen*sizeof(AkOSChar));
	AKASSERT( pszLowerCaseString );
	AKPLATFORM::SafeStrCpy(pszLowerCaseString, in_pszString, uStrLen );
	_MakeLower( pszLowerCaseString );

	// 'this' is m_uNumStrings. +1 points to the beginning of the StringEntry array.
	StringEntry * pTable = (StringEntry*)((AkUInt32*)this + 1);	
	
	// Binary search: strings are sorted (case sensitive).
	AkInt32 uTop = 0, uBottom = m_uNumStrings-1;
	do
	{
		AkInt32 uThis = ( uBottom - uTop ) / 2 + uTop; 
		AkOSChar * pString = (AkOSChar*)((AkUInt8*)this + pTable[ uThis ].uOffset);
		int iCmp = AKPLATFORM::OsStrCmp( pString, pszLowerCaseString );
		if ( 0 == iCmp )
			return pTable[uThis].uID;
		else if ( iCmp > 0 )	//in_pTable[ uThis ].pString > pszLowerCaseString 
			uBottom = uThis - 1;
		else					//in_pTable[ uThis ].pString < pszLowerCaseString 
			uTop = uThis + 1;
	}
	while ( uTop <= uBottom );

	// ID not found.
	return AK_INVALID_UNIQUE_ID;
}
