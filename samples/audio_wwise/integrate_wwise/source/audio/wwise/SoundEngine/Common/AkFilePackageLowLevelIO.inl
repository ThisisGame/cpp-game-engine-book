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
// AkFilePackageLowLevelIO.h
//
// CAkFilePackageLowLevelIO extends a Low-Level I/O device by providing 
// the ability to reference files that are part of a file package.
//
// It can extend either blocking or deferred I/O hooks (both inheriting from
// AK::StreamMgr::IAkLowLevelIOHook), since its base class is templated. 
// In either case, the base class must also implement 
// AK::StreamMgr::IAkFileLocationResolver. This interface defines both overloads
// for Open(), and this is where the package's look-up table is searched.
// If no match is found, then it falls back on the base implementation.
// 
// Clients of devices that use this class' functionnality simply need to call
// LoadFilePackage(), which loads and parses file packages that were created with 
// the AkFilePackager utility app (located in ($WWISESDK)/samples/FilePackager/). 
// The header of these file packages contains look-up tables that describe the 
// internal offset of each file it references, their block size (required alignment), 
// and their language. Each combination of AkFileID and Language ID is unique.
//
// LoadFilePackage() returns a package ID that can be used to unload it. Any number
// of packages can be loaded simultaneously. When Open() is called, the last package 
// loaded is searched first, then the previous one, and so on.
//
// The language ID was created dynamically when the package was created. The header 
// also contains a map of language names (strings) to their ID, so that the proper 
// language-specific version of files can be resolved. The language name that is stored
// matches the name of the directory that is created by the Wwise Bank Manager,
// except for the trailing slash.
//
// The type of package is also a template argument. By default, it is a disk package
// (see AkDiskPackage.h).
//
//////////////////////////////////////////////////////////////////////

#include "AkFilePackageLowLevelIO.h"
#include "AkFileHelpers.h"
#include <AK/Tools/Common/AkPlatformFuncs.h>

template <class T_LLIOHOOK_FILELOC, class T_PACKAGE>
CAkFilePackageLowLevelIO<T_LLIOHOOK_FILELOC,T_PACKAGE>::CAkFilePackageLowLevelIO()
: m_bRegisteredToLangChg( false )
{
}

template <class T_LLIOHOOK_FILELOC, class T_PACKAGE>
CAkFilePackageLowLevelIO<T_LLIOHOOK_FILELOC,T_PACKAGE>::~CAkFilePackageLowLevelIO()
{
}

// Initialize/terminate.
template <class T_LLIOHOOK_FILELOC, class T_PACKAGE>
void CAkFilePackageLowLevelIO<T_LLIOHOOK_FILELOC,T_PACKAGE>::Term()
{
    UnloadAllFilePackages();
	m_packages.Term();
	if ( m_bRegisteredToLangChg )
		AK::StreamMgr::RemoveLanguageChangeObserver( this );
	T_LLIOHOOK_FILELOC::Term();
}

// Override Open (string): Search file in each LUT first. If it cannot be found, use base class services.
// If the file is found in the LUTs, open is always synchronous.
template <class T_LLIOHOOK_FILELOC, class T_PACKAGE>
AKRESULT CAkFilePackageLowLevelIO<T_LLIOHOOK_FILELOC,T_PACKAGE>::Open( 
    const AkOSChar* in_pszFileName,     // File name.
    AkOpenMode      in_eOpenMode,       // Open mode.
    AkFileSystemFlags * in_pFlags,      // Special flags. Can pass NULL.
	bool &			io_bSyncOpen,		// If true, the file must be opened synchronously. Otherwise it is left at the File Location Resolver's discretion. Return false if Open needs to be deferred.
    AkFileDesc &    out_fileDesc        // Returned file descriptor.
    )
{
	AkAutoLock<CAkLock> lock(m_lock);
    // If the file is an AK sound bank, try to find the identifier in the lookup table first.
    if ( in_eOpenMode == AK_OpenModeRead 
		&& in_pFlags )
    {
		if( in_pFlags->uCompanyID == AKCOMPANYID_AUDIOKINETIC 
			&& in_pFlags->uCodecID == AKCODECID_BANK )
		{
			// Search file in each package.
			ListFilePackages::Iterator it = m_packages.Begin();
			while ( it != m_packages.End() )
			{
				AkFileID fileID = (*it)->lut.GetSoundBankID( in_pszFileName );

				if ( FindPackagedFile( (T_PACKAGE*)(*it), fileID, in_pFlags, out_fileDesc ) == AK_Success )
				{
					// Found the ID in the lut. 
					io_bSyncOpen = true;	// File is opened, now.
					(*it)->AddRef();
					out_fileDesc.pPackage = (*it);
					return AK_Success;
				}
				++it;
			}
		}
		else if ( in_pFlags->uCompanyID == AKCOMPANYID_AUDIOKINETIC_EXTERNAL )
		{
			// Search file in each package.
			ListFilePackages::Iterator it = m_packages.Begin();
			while ( it != m_packages.End() )
			{
				AkUInt64 fileID = (*it)->lut.GetExternalID( in_pszFileName );

				if ( FindPackagedFile( (T_PACKAGE*)(*it), fileID, in_pFlags, out_fileDesc ) == AK_Success )
				{
					// Found the ID in the lut. 
					io_bSyncOpen = true;	// File is opened, now.
					(*it)->AddRef();
					out_fileDesc.pPackage = (*it);
					return AK_Success;
				}

				++it;
			}
		}
	}

    // It is not a soundbank, or it is not in the file package LUT. Use default implementation.
    return T_LLIOHOOK_FILELOC::Open( 
		in_pszFileName,
		in_eOpenMode,
		in_pFlags,
		io_bSyncOpen,
		out_fileDesc);
}

// Override Open (ID): Search file in each LUT first. If it cannot be found, use base class services.
// If the file is found in the LUTs, open is always synchronous.
template <class T_LLIOHOOK_FILELOC, class T_PACKAGE>
AKRESULT CAkFilePackageLowLevelIO<T_LLIOHOOK_FILELOC,T_PACKAGE>::Open( 
    AkFileID        in_fileID,          // File ID.
    AkOpenMode      in_eOpenMode,       // Open mode.
    AkFileSystemFlags * in_pFlags,      // Special flags. Can pass NULL.
	bool &			io_bSyncOpen,		// If true, the file must be opened synchronously. Otherwise it is left at the File Location Resolver's discretion. Return false if Open needs to be deferred.
    AkFileDesc &    out_fileDesc        // Returned file descriptor.
    )
{
	AkAutoLock<CAkLock> lock(m_lock);
    // Try to find the identifier in the lookup table first.
    if ( in_eOpenMode == AK_OpenModeRead 
		&& in_pFlags 
		&& in_pFlags->uCompanyID == AKCOMPANYID_AUDIOKINETIC)
	{
		// Search file in each package.
		ListFilePackages::Iterator it = m_packages.Begin();
		while ( it != m_packages.End() )
		{
			if ( FindPackagedFile( (T_PACKAGE*)(*it), in_fileID, in_pFlags, out_fileDesc ) == AK_Success )
			{
				// File found. Return now.
				io_bSyncOpen = true;	// File is opened, now.
				(*it)->AddRef();
				out_fileDesc.pPackage = (*it);
				return AK_Success;
			}
			++it;
		}
	}
	else if ( in_pFlags->uCompanyID == AKCOMPANYID_AUDIOKINETIC_EXTERNAL )
	{
		// Search file in each package.
		ListFilePackages::Iterator it = m_packages.Begin();
		while ( it != m_packages.End() )
		{	
			AkOSChar szFileName[20];
			AK_OSPRINTF(szFileName, 20, AKTEXT("%u.wem"), (unsigned int)in_fileID);
			AkUInt64 fileID = (*it)->lut.GetExternalID(szFileName);

			if ( FindPackagedFile( (T_PACKAGE*)(*it), fileID, in_pFlags, out_fileDesc ) == AK_Success )
			{
				// Found the ID in the lut. 
				io_bSyncOpen = true;	// File is opened, now.
				(*it)->AddRef();
				out_fileDesc.pPackage = (*it);
				return AK_Success;
			}

			++it;
		}
	}

    // If it the fileID is not in the LUT, perform standard path concatenation logic.
    return T_LLIOHOOK_FILELOC::Open( 
		in_fileID,
		in_eOpenMode,
		in_pFlags,
		io_bSyncOpen,
		out_fileDesc);
}

// Override Close: Do not close handle if file descriptor is part of the current packaged file.
template <class T_LLIOHOOK_FILELOC, class T_PACKAGE>
AKRESULT CAkFilePackageLowLevelIO<T_LLIOHOOK_FILELOC, T_PACKAGE>::Close(
	AkFileDesc& in_fileDesc      // File descriptor.
)
{
	AkAutoLock<CAkLock> lock(m_lock);
	// Do not close handle if it is that of the file package (closed only in UnloadFilePackage()).
	if (IsInPackage(in_fileDesc))
	{
		in_fileDesc.pPackage->Release();
		in_fileDesc.pPackage = NULL;
		return AK_Success;
	}
	return T_LLIOHOOK_FILELOC::Close(in_fileDesc);    
}

// Override GetBlockSize: Get the block size of the LUT if a file package is loaded.
template <class T_LLIOHOOK_FILELOC, class T_PACKAGE>
AkUInt32 CAkFilePackageLowLevelIO<T_LLIOHOOK_FILELOC,T_PACKAGE>::GetBlockSize(
    AkFileDesc &  in_fileDesc     // File descriptor.
    )
{
	AkAutoLock<CAkLock> lock(m_lock);
	if ( IsInPackage( in_fileDesc ) )
	{
		// This file is part of a package. At Open(), we used the 
		// AkFileDesc.uCustomParamSize field to store the block size.
		return in_fileDesc.uCustomParamSize;
	}
	return T_LLIOHOOK_FILELOC::GetBlockSize( in_fileDesc );
}

// Updates language of all loaded packages. Packages keep a language ID to help them find 
// language-specific assets quickly.
template <class T_LLIOHOOK_FILELOC, class T_PACKAGE>
void CAkFilePackageLowLevelIO<T_LLIOHOOK_FILELOC,T_PACKAGE>::OnLanguageChange( 
	const AkOSChar * const in_pLanguageName	// New language name.
	)
{
	AkAutoLock<CAkLock> lock(m_lock);
	// Set language on all loaded packages.
	ListFilePackages::Iterator it = m_packages.Begin();
	while ( it != m_packages.End() )
	{
		(*it)->lut.SetCurLanguage( in_pLanguageName );
		++it;
	}
}

// Searches the LUT to find the file data associated with the FileID.
// Returns AK_Success if the file is found.
template <class T_LLIOHOOK_FILELOC, class T_PACKAGE> 
template <class T_FILEID>
AKRESULT CAkFilePackageLowLevelIO<T_LLIOHOOK_FILELOC,T_PACKAGE>::FindPackagedFile( 
	T_PACKAGE *			in_pPackage,	// Package to search into.
	T_FILEID			in_fileID,		// File ID.
	AkFileSystemFlags * in_pFlags,		// Special flags. Can pass NULL.
	AkFileDesc &		out_fileDesc	// Returned file descriptor.
	)
{
	AkAutoLock<CAkLock> lock(m_lock);
	AKASSERT( in_pPackage && in_pFlags );
	const CAkFilePackageLUT::AkFileEntry<T_FILEID> * pEntry = in_pPackage->lut.LookupFile( in_fileID, in_pFlags );

	if ( pEntry )
	{
		// Fill file descriptor.
		out_fileDesc.iFileSize	= pEntry->uFileSize;
		out_fileDesc.uSector	= pEntry->uStartBlock;
		// NOTE: We use the uCustomParamSize to store the block size.
		// We will determine whether this file was opened from a package by comparing 
		// uCustomParamSize with 0 (see IsInPackage()).
		out_fileDesc.uCustomParamSize = pEntry->uBlockSize;
		out_fileDesc.hFile = in_pPackage->GetHandleForFileDesc();
		out_fileDesc.pCustomParam = in_pPackage->GetCustomParamPtrForFileDesc();
		out_fileDesc.deviceID = T_LLIOHOOK_FILELOC::m_deviceID;

		// Deal with custom parameters in derived classes.
		InitFileDesc(in_pPackage, out_fileDesc);
		return AK_Success;
	}
	return AK_FileNotFound;
}

// File package loading:
// Opens a package file, parses its header, fills LUT.
// Overrides of Open() will search files in loaded LUTs first, then use default Low-Level I/O 
// services if they cannot be found.
// Any number of packages can be loaded at a time. Each LUT is searched until a match is found.
// Returns AK_Success if successful, AK_InvalidLanguage if the current language 
// does not exist in the LUT (not necessarily an error), AK_Fail for any other reason.
// Also returns a package ID which can be used to unload it (see UnloadFilePackage()).
template <class T_LLIOHOOK_FILELOC, class T_PACKAGE>
AKRESULT CAkFilePackageLowLevelIO<T_LLIOHOOK_FILELOC,T_PACKAGE>::LoadFilePackage(
    const AkOSChar *    in_pszFilePackageName,	// File package name. 
	AkUInt32 &			out_uPackageID			// Returned package ID.
    )
{
	// Open package file.
	AkFilePackageReader filePackageReader;
	AKRESULT eRes = filePackageReader.Open( in_pszFilePackageName, true );	// Open from SFX-only directory.
	if ( eRes != AK_Success )
        return eRes;

	filePackageReader.SetName( in_pszFilePackageName );

	T_PACKAGE * pPackage;
	eRes = _LoadFilePackage( in_pszFilePackageName, filePackageReader, AK_DEFAULT_PRIORITY, pPackage );
	if ( eRes == AK_Success
		|| eRes == AK_InvalidLanguage )
	{
		AKASSERT( pPackage );
		// Add to packages list.
		AkAutoLock<CAkLock> lock(m_lock);
		m_packages.AddFirst( pPackage );
		
		out_uPackageID = pPackage->ID();
	}
	return eRes;
}
	
// Loads a file package, with a given file package reader.
template <class T_LLIOHOOK_FILELOC, class T_PACKAGE>
AKRESULT CAkFilePackageLowLevelIO<T_LLIOHOOK_FILELOC,T_PACKAGE>::_LoadFilePackage(
    const AkOSChar*			in_pszFilePackageName,	// File package name. 
	AkFilePackageReader &	in_reader,				// File package reader.
	AkPriority				in_readerPriority,		// File package reader priority heuristic.
	T_PACKAGE *&			out_pPackage			// Returned package
    )
{
	// Read header chunk definition.
	struct AkFilePackageHeader
	{
		AkUInt32 uFileFormatTag;
		AkUInt32 uHeaderSize;
	};

	AkUInt32 uReadBufferSize = AkMax( 2 * in_reader.GetBlockSize(), sizeof(AkFilePackageHeader) );
	AkUInt8 * pBufferForHeader = (AkUInt8*)AkAlloca( uReadBufferSize );
	AkUInt32 uSizeToRead;
	bool bAligned = ( sizeof(AkFilePackageHeader) % in_reader.GetBlockSize() ) > 0;
	if ( bAligned )
	{
		// Header size is not a multiple of the required block size. Allocate an aligned buffer on the stack.
		pBufferForHeader += ( in_reader.GetBlockSize() - (AkUIntPtr)pBufferForHeader % in_reader.GetBlockSize() );
		uSizeToRead = in_reader.GetBlockSize();
	}
	else
	{
		// Header size is a multiple of the required block size. 
		uSizeToRead = sizeof(AkFilePackageHeader);
	}

	AKRESULT eRes;
	AkUInt32 uSizeRead;
	{
		void* pReadBuffer = AkAlloc(AkMemID_Streaming, uReadBufferSize);
		eRes = in_reader.Read(pReadBuffer, uSizeToRead, uSizeRead, in_readerPriority );
		if ( eRes != AK_Success || uSizeRead < sizeof(AkFilePackageHeader) )
		{
			AKASSERT( !"Could not read package, or package is invalid" );
			in_reader.Close();
			AkFree(AkMemID_Streaming, pReadBuffer);

			return AK_Fail;
		}
		AKPLATFORM::AkMemCpy(pBufferForHeader, pReadBuffer, uSizeRead);
		AkFree(AkMemID_Streaming, pReadBuffer);
	}

	const AkFilePackageHeader & uFileHeader = *(AkFilePackageHeader*)pBufferForHeader;

	if ( uFileHeader.uFileFormatTag != AKPK_FILE_FORMAT_TAG 
		|| 0 == uFileHeader.uHeaderSize )
	{
		AKASSERT( !"Invalid file package header" );
		in_reader.Close();
        return AK_Fail;
	}

	// Create file package.
	AkUInt32 uReservedHeaderSize;
	AkUInt8 * pFilePackageHeader;
	out_pPackage = T_PACKAGE::Create(
		in_reader,
		in_pszFilePackageName,
		uFileHeader.uHeaderSize + AKPK_HEADER_CHUNK_DEF_SIZE,	// NOTE: The header size written in the file package excludes the AKPK_HEADER_CHUNK_DEF_SIZE.
		uReservedHeaderSize,
		pFilePackageHeader );
	if ( !out_pPackage )
	{
		AKASSERT( !"Could not create file package" );
		in_reader.Close();
        return AK_Fail;
	}

	AkUInt32 uHeaderSize = uFileHeader.uHeaderSize;
	AkUInt32 uHeaderReadOffset = AKPK_HEADER_CHUNK_DEF_SIZE;

	// If we had already read more than sizeof(AkFilePackageHeader), copy the rest now.
	if ( uSizeRead > sizeof(AkFilePackageHeader) )
	{
		pBufferForHeader += sizeof(AkFilePackageHeader);
		AkUInt32 uSizeToCopy = uSizeRead - sizeof(AkFilePackageHeader);
		AKPLATFORM::AkMemCpy( pFilePackageHeader+AKPK_HEADER_CHUNK_DEF_SIZE, pBufferForHeader, uSizeToCopy );
		// Adjust header size and read offset.
		if ( uSizeToCopy > uHeaderSize )
			uSizeToCopy = uHeaderSize;
		uHeaderSize -= uSizeToCopy;
		uHeaderReadOffset += uSizeToCopy;
		// Round it up to required block size. It should be equal to the size that was reserved (minus what was already read).
		uHeaderSize = ( ( uHeaderSize + in_reader.GetBlockSize() - 1 ) / in_reader.GetBlockSize() ) * in_reader.GetBlockSize();
		AKASSERT( uHeaderSize == uReservedHeaderSize - uSizeRead );
	}

	// Stream in remaining of the header.
	if ( uHeaderSize > 0 )
	{
		AKASSERT( uHeaderReadOffset % in_reader.GetBlockSize() == 0 );
		if ( in_reader.Read( pFilePackageHeader+uHeaderReadOffset, uHeaderSize, uSizeRead, in_readerPriority ) != AK_Success
			|| uSizeRead < uHeaderSize )
		{
			AKASSERT( !"Could not read file package" );
			out_pPackage->Release();
			return AK_Fail;
		}
	}

	// Parse LUT.
	eRes = out_pPackage->lut.Setup( pFilePackageHeader, uFileHeader.uHeaderSize + AKPK_HEADER_CHUNK_DEF_SIZE );
	if ( eRes != AK_Success )
	{
		out_pPackage->Release();
		return eRes;
	}

	// Register to language change notifications if it wasn't already done
	if ( !m_bRegisteredToLangChg )
	{
		if ( AK::StreamMgr::AddLanguageChangeObserver( LanguageChangeHandler, this ) != AK_Success )
		{
			out_pPackage->Release();
			return AK_Fail;
		}
		m_bRegisteredToLangChg = true;
	}

	// Use the current language path (if defined) to set the language ID, 
    // for language specific file mapping.
	return out_pPackage->lut.SetCurLanguage( AK::StreamMgr::GetCurrentLanguage() );
}

// Unload a file package.
template <class T_LLIOHOOK_FILELOC, class T_PACKAGE>
AKRESULT CAkFilePackageLowLevelIO<T_LLIOHOOK_FILELOC,T_PACKAGE>::UnloadFilePackage( 
	AkUInt32	in_uPackageID			// Package ID.
	)
{
	AkAutoLock<CAkLock> lock(m_lock);
	ListFilePackages::IteratorEx it = m_packages.BeginEx();
	while ( it != m_packages.End() )
	{
		if ( (*it)->ID() == in_uPackageID )
		{
			CAkFilePackage * pPackage = (*it);
			it = m_packages.Erase( it );

			// Destroy package.
			pPackage->Release();

			return AK_Success;
		}
		else
			++it;
	}

	AKASSERT( !"Invalid package ID" );
	return AK_Fail;
}

// Unload all file packages.
template <class T_LLIOHOOK_FILELOC, class T_PACKAGE>
AKRESULT CAkFilePackageLowLevelIO<T_LLIOHOOK_FILELOC,T_PACKAGE>::UnloadAllFilePackages()
{
	AkAutoLock<CAkLock> lock(m_lock);
	ListFilePackages::IteratorEx it = m_packages.BeginEx();
	while ( it != m_packages.End() )
	{
		CAkFilePackage * pPackage = (*it);
		it = m_packages.Erase( it );

		// Destroy package.
		pPackage->Release();
	}

	return AK_Success;
}


