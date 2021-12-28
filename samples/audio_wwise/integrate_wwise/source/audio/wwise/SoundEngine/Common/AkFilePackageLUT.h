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
// AkFilePackageLUT.h
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

#ifndef _AK_FILE_PACKAGE_LUT_H_
#define _AK_FILE_PACKAGE_LUT_H_

#include <AK/SoundEngine/Common/IAkStreamMgr.h>
#include <AK/Tools/Common/AkAssert.h>

// AK file packager definitions.
#define AKPK_CURRENT_VERSION		(1)

#define AKPK_HEADER_CHUNK_DEF_SIZE	(8)	// The header chunk definition is 8 bytes wide.

#define AKPK_FILE_FORMAT_TAG	\
		AkmmioFOURCC('A','K','P','K')

//-----------------------------------------------------------------------------
// Name: class CAkFilePackageLUT.
// Desc: Keeps pointers to various parts of the header. Offers look-up services
// for file look-up and soundbank ID retrieval.
//-----------------------------------------------------------------------------
class CAkFilePackageLUT
{
public:

	static const AkUInt16 AK_INVALID_LANGUAGE_ID = 0;

// Ensure no padding is done because this structure is mapped to file content
#pragma pack(push, 4)
	template <class T_FILEID>
	struct AkFileEntry 
    {
        T_FILEID	fileID;		// File identifier. 
		AkUInt32	uBlockSize;	// Size of one block, required alignment (in bytes).
		AkUInt32	uFileSize;  // File size in bytes. 
        AkUInt32	uStartBlock;// Start block, expressed in terms of uBlockSize. 
        AkUInt32	uLanguageID;// Language ID. AK_INVALID_LANGUAGE_ID if not language-specific. 
    };
#pragma pack(pop)

    CAkFilePackageLUT();
    virtual ~CAkFilePackageLUT();

	// Create a new LUT from a packaged file header.
	// The LUT sets pointers to appropriate location inside header data (in_pData).
	AKRESULT Setup(
		AkUInt8 *			in_pData,			// Header data.
		AkUInt32			in_uHeaderSize		// Size of file package header.
		);

	// Find a file entry by ID.
	const AkFileEntry<AkFileID> * LookupFile(
		AkFileID			in_uID,				// File ID.
		AkFileSystemFlags * in_pFlags			// Special flags. Do not pass NULL.
		);
	
    // Find a file entry by ID with 64 bit ID.
	const AkFileEntry<AkUInt64> * LookupFile(
		AkUInt64			in_uID,				// File ID.
		AkFileSystemFlags * in_pFlags			// Special flags. Do not pass NULL.
		);

	// Set current language.
	// Returns AK_InvalidLanguage if a package is loaded but the language string cannot be found.
	// Returns AK_Success otherwise.
	AKRESULT SetCurLanguage(
		const AkOSChar*			in_pszLanguage		// Language string.
		);

	// Find a soundbank ID by its name (by hashing its name)
	AkFileID GetSoundBankID( 
		const AkOSChar*			in_pszBankName		// Soundbank name.
		);

    // Return the id of an external file (by hashing its name in 64 bits)
	AkUInt64 GetExternalID( 
		const AkOSChar*			in_pszExternalName		// External Source name.
		);	

protected:
	static void RemoveFileExtension( AkOSChar* in_pstring );
	static void _MakeLower( AkOSChar* in_pString );
	static void _MakeLowerA( char* in_pString, size_t in_strlen );
	static AkUInt64 _MakeLowerAndHash64( char* in_pszString );
	static AkUInt64 GetID64FromString( const char* in_pszString );
	static AkUInt64 GetID64FromString( const wchar_t* in_pszString );

	//
	// File LUTs.
	// 
	template <class T_FILEID>
	class FileLUT
	{
	public:
		const AkFileEntry<T_FILEID> *	FileEntries() const { return (AkFileEntry<T_FILEID>*)((AkUInt32*)this + 1); }
		bool HasFiles() const { return ( m_uNumFiles > 0 ); }
		AkUInt32 NumFiles() const { return m_uNumFiles; }
	private:
		FileLUT();	// Do not create this object, just cast raw data.
		AkUInt32		m_uNumFiles;
	};

	// Helper: Find a file entry by ID.
	template <class T_FILEID>
	const AkFileEntry<T_FILEID> * LookupFile(
		T_FILEID					in_uID,					// File ID.
		const FileLUT<T_FILEID> *	in_pLut,				// LUT to search.
		bool						in_bIsLanguageSpecific	// True: match language ID.
		);

private:

	AkUInt16			m_curLangID;	// Current language.


	//
	// Maps format.
	//
	class StringMap
	{
	public:
		// Returns AK_INVALID_UNIQUE_ID if ID is not found.
		AkUInt32 GetID( const AkOSChar* in_pszString );
		inline AkUInt32 GetNumStrings() { return m_uNumStrings; }
	private:
		struct StringEntry
		{
			AkUInt32	uOffset;	// Byte offset of the string in the packaged strings section, 
									// from beginning of the string map.
			AkUInt32	uID;		// ID.
		};
		StringMap();	// Do not create this object, just cast raw data to use GetID().
		AkUInt32	m_uNumStrings;
	};

	// Languages map.
	StringMap *			m_pLangMap;

	// SoundBanks LUT.
    FileLUT<AkFileID> *			m_pSoundBanks;
	
	// StreamedFiles LUT.
    FileLUT<AkFileID> *			m_pStmFiles;

	// External Sources LUT.
    FileLUT<AkUInt64> *			m_pExternals;
};

// Helper: Find a file entry by ID.
template <class T_FILEID>
const CAkFilePackageLUT::AkFileEntry<T_FILEID> * CAkFilePackageLUT::LookupFile(
	T_FILEID					in_uID,					// File ID.
	const FileLUT<T_FILEID> *	in_pLut,				// LUT to search.
	bool						in_bIsLanguageSpecific	// True: match language ID.
	)
{
	const AkFileEntry<T_FILEID> * pTable	= in_pLut->FileEntries();

	AKASSERT( pTable && in_pLut->HasFiles() );
	AkUInt16 uLangID = in_bIsLanguageSpecific ? m_curLangID : AK_INVALID_LANGUAGE_ID;

	// Binary search. LUT items should be sorted by fileID, then by language ID.
	AkInt32 uTop = 0, uBottom = in_pLut->NumFiles()-1;
	do
	{
		AkInt32 uThis = ( uBottom - uTop ) / 2 + uTop; 
		if ( pTable[ uThis ].fileID > in_uID ) 
			uBottom = uThis - 1;
		else if ( pTable[ uThis ].fileID < in_uID ) 
			uTop = uThis + 1;
		else
		{
			// Correct ID. Check language.
			if ( pTable[ uThis ].uLanguageID > uLangID ) 
				uBottom = uThis - 1;
			else if ( pTable[ uThis ].uLanguageID < uLangID ) 
				uTop = uThis + 1;
			else
				return pTable + uThis;
		}
	}
	while ( uTop <= uBottom );

	return NULL;
}

#endif //_AK_FILE_PACKAGE_LUT_H_
