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

#ifndef _AK_FILE_PACKAGE_LOW_LEVEL_IO_H_
#define _AK_FILE_PACKAGE_LOW_LEVEL_IO_H_

#include <AK/SoundEngine/Common/AkStreamMgrModule.h>
#include "AkFilePackage.h"
#include <AK/Tools/Common/AkAutoLock.h>
#include <AK/Tools/Common/AkLock.h>

//-----------------------------------------------------------------------------
// Name: AkFilePackageReader 
// Desc: This class wraps an AK::IAkStdStream to read a file package.
//-----------------------------------------------------------------------------
class AkFilePackageReader
{
public:
	AkFilePackageReader()
		: m_pStream( NULL ), m_uBlockSize( 0 ) {}
	~AkFilePackageReader() 
	{
		// IMPORTANT: Do not close file. This object can be copied.
	}

	AKRESULT Open(
		const AkOSChar*	in_pszFilePackageName,	// File package name. Location is resolved using base class' Open().
		bool in_bReadFromSFXOnlyDir		// If true, the file package is opened from the language agnostic directory only. Otherwise, it tries to open it 
										// from the current language-specific directory first, and then from the common directory if it fails, similarly to the soundbanks loader of the Sound Engine (Default).
		)
	{
		AkFileSystemFlags flags;
		flags.uCompanyID = AKCOMPANYID_AUDIOKINETIC;
		flags.uCodecID = AKCODECID_FILE_PACKAGE;
		flags.uCustomParamSize = 0;
		flags.pCustomParam = NULL;
		flags.bIsLanguageSpecific = !in_bReadFromSFXOnlyDir;

		AKRESULT eResult = AK::IAkStreamMgr::Get()->CreateStd(
            in_pszFilePackageName,
            &flags,
			AK_OpenModeRead,
			m_pStream,
			true );

		if ( eResult != AK_Success 
			&& !in_bReadFromSFXOnlyDir )
		{
			// Try again, in SFX-only directory.
			flags.bIsLanguageSpecific = false;
			eResult = AK::IAkStreamMgr::Get()->CreateStd(
				in_pszFilePackageName,
				&flags,
				AK_OpenModeRead,
				m_pStream,
				true );
		}

		return eResult;
	}

	AKRESULT Read(
		void *			in_pBuffer,			// Buffer. Must be aligned with value returned by GetBlockSize().
		AkUInt32		in_uSizeToRead,		// Size to read. Must be a multiple of value returned by GetBlockSize().
		AkUInt32 &		out_uSizeRead,		// Returned size read.
		AkPriority		in_priority = AK_DEFAULT_PRIORITY,	// Priority heuristic.
		AkReal32		in_fThroughput = 0	// Throughput heuristic. 0 means "not set", and results in "immediate".
		)
	{
		AKASSERT( m_pStream );
		AkReal32 fDeadline = ( in_fThroughput > 0 ) ? in_uSizeToRead / in_fThroughput : 0;
		return m_pStream->Read( 
			in_pBuffer, 
			in_uSizeToRead,
			true,
			in_priority,
			fDeadline,
			out_uSizeRead );
	}

	AKRESULT Seek( 
		AkUInt32		in_uPosition,
		AkUInt32 &		out_uRealOffset
		)
	{
		AkInt64 iRealOffset;
		AKRESULT eResult = m_pStream->SetPosition( in_uPosition, AK_MoveBegin, &iRealOffset );
		AKASSERT( eResult == AK_Success || !"Failed changing stream position" );
		out_uRealOffset = (AkUInt32)iRealOffset;
		return eResult;
	}

	void Close()
	{
		if ( m_pStream )
			m_pStream->Destroy();
		m_pStream = NULL;
	}

	void SetName( 
		const AkOSChar* 
#ifndef AK_OPTIMIZED
		in_pszName 
#endif
		)
	{
#ifndef AK_OPTIMIZED
		AKASSERT( m_pStream );
		m_pStream->SetStreamName( in_pszName );
#endif
	}

	AkUInt64 GetSize()
	{
		AKASSERT( m_pStream );
		AkStreamInfo info;
		m_pStream->GetInfo( info );
		return info.uSize;
	}

	AkUInt32 GetBlockSize()
	{
		AKASSERT( m_pStream );
		// AK::IAkStdStream::GetBlockSize() is costly. Cache block size.
		if ( !m_uBlockSize )
			m_uBlockSize = m_pStream->GetBlockSize();
		return m_uBlockSize;
	}

	AkFileHandle GetHandle()
	{
		AKASSERT( m_pStream );
		AkFileDesc * pFileDesc = (AkFileDesc*)m_pStream->GetFileDescriptor();
		AKASSERT( pFileDesc );
		return pFileDesc->hFile;
	}

	AkFileDesc *GetFileDesc()
	{
		return (AkFileDesc*)m_pStream->GetFileDescriptor();
	}

private:
	AK::IAkStdStream *	m_pStream;
	AkUInt32			m_uBlockSize;
};

//-----------------------------------------------------------------------------
// Name: CAkDiskPackage 
// Desc: This class extends the CAkFilePackage class by providing system handle
// management.
// It keeps a copy of the file package reader that was used to read the file package
// header from disk, and uses it to query and cache its low-level system handle
// (AkFileDesc::hFile). This handle is kept open and used directly to read portions
// of the package from disk, corresponding to read requests for the files it 
// contains. The streaming object / package handle is closed when the package
// is destroyed.
//-----------------------------------------------------------------------------
class CAkDiskPackage : public CAkFilePackage
{
public:
	// Factory for disk package.
	// Instantiates a file package object, queries its file handle once and keep in package.
	// Also keeps a copy of its reader object, which is used to close the file handle on destruction.
	static CAkDiskPackage * Create( 
		AkFilePackageReader & in_reader,		// File package reader.
		const AkOSChar*		in_pszPackageName,	// Name of the file package (for memory monitoring).
		AkUInt32 			in_uHeaderSize,		// File package header size, including the size of the header chunk AKPK_HEADER_CHUNK_DEF_SIZE.
		AkUInt32 &			out_uReservedHeaderSize, // Size reserved for header, taking mem align into account.
		AkUInt8 *&			out_pHeaderBuffer	// Returned address of memory for header.
		)
	{
		CAkDiskPackage * pPackage = CAkFilePackage::Create<CAkDiskPackage>( 
			in_pszPackageName,
			in_uHeaderSize,
			in_reader.GetBlockSize(),
			out_uReservedHeaderSize,
			out_pHeaderBuffer );
		if ( pPackage )
		{
			pPackage->m_reader = in_reader;				// Copy reader.
			AkFileDesc* pFileDesc = in_reader.GetFileDesc();
			pPackage->m_hFile = pFileDesc->hFile;	// Cache handle.
			pPackage->m_pCustomParam = pFileDesc->pCustomParam;	// Cache custom param.
		}
		return pPackage;
	}

	CAkDiskPackage(AkUInt32 in_uPackageID, AkUInt32 in_uHeaderSize, void * in_pToRelease)
		: CAkFilePackage(in_uPackageID, in_uHeaderSize, in_pToRelease)
	{ }

	// Override Destroy(): Close 
	virtual void Destroy()
	{
		m_reader.Close();
		CAkFilePackage::Destroy();
	}

	// Fills an AkFileHandle with a value that will be useable by the low-level I/O hook.
	// Disk packages return the package's system handle: the hook reads from the package file itself, with
	// proper offset, to get the data it needs.
	inline AkFileHandle GetHandleForFileDesc() { return m_hFile; }
	inline void* GetCustomParamPtrForFileDesc() { return m_pCustomParam; }

	AkFileDesc* GetFileDesc() {return m_reader.GetFileDesc();}

protected:
	
	AkFilePackageReader	m_reader;	// READER object. Holds the stream used to read the package. Closed only upon package destruction.
	AkFileHandle		m_hFile;	// Platform-independent file handle (cached from READER).
	void*				m_pCustomParam; // Platform-independent storage of custom param from file (cached from READER)
};

//-----------------------------------------------------------------------------
// Name: class CAkFilePackageLowLevelIO.
// Desc: Extends default Low-level IO implementation with packaged file support.
//		 Base class must implement one of the low-level I/O hooks 
//		 (AK::StreamMgr::IAkIOHookBlocking or AK::StreamMgr::IAkIOHookDeferred)
//		 _and_ the AK::StreamMgr::IAkFileLocationResolver interface.
//		 It must also define the following methods:
//			- void Term()
// Note: This class uses AkFileDesc::uCustomParamSize to store the block size 
//		 of files opened from a package, and relies on the fact that it is 0 
//		 when they are not part of the package.
//-----------------------------------------------------------------------------
template <class T_LLIOHOOK_FILELOC, class T_PACKAGE = CAkDiskPackage>
class CAkFilePackageLowLevelIO : public T_LLIOHOOK_FILELOC
{
public:

    CAkFilePackageLowLevelIO();
    virtual ~CAkFilePackageLowLevelIO();

	// File package loading:
    // Opens a package file, parses its header, fills LUT.
    // Overrides of Open() will search files in loaded LUTs first, then use default Low-Level I/O 
	// services if they cannot be found.
	// Any number of packages can be loaded at a time. Each LUT is searched until a match is found.
	// Returns AK_Success if successful, AK_InvalidLanguage if the current language 
	// does not exist in the LUT (not necessarily an error), AK_Fail for any other reason.
	// Also returns a package ID which can be used to unload it (see UnloadFilePackage()).
	// WARNING: This method is not thread safe. Ensure there are no I/O occurring on this device
	// when loading a file package.
    virtual AKRESULT LoadFilePackage(
        const AkOSChar* in_pszFilePackageName,	// File package name. Location is resolved using base class' Open().
		AkUInt32 &		out_uPackageID			// Returned package ID.
        );
	
	// Unload a file package.
	// Returns AK_Success if in_uPackageID exists, AK_Fail otherwise.
	// WARNING: This method is not thread safe. Ensure there are no I/O occurring on this device
	// when unloading a file package.
	virtual AKRESULT UnloadFilePackage( 
		AkUInt32	in_uPackageID			// Returned package ID.
		);

	// Unload all file packages.
	// Returns AK_Success;
	// WARNING: This method is not thread safe. Ensure there are no I/O occurring on this device
	// when unloading a file package.
    virtual AKRESULT UnloadAllFilePackages();


	//
	// Overriden base class policies.
	// ---------------------------------------------------------------

    // Clean up.
    void Term();

protected:

	//
	// IAkFileLocationResolver interface overriden methods.
	// ---------------------------------------------------------------

	// Override Open (string): Search file in each LUT first. If it cannot be found, use base class services.
	// If the file is found in the LUTs, open is always synchronous.
	// Applies to AK soundbanks only.
    virtual AKRESULT Open( 
        const AkOSChar* in_pszFileName,     // File name.
        AkOpenMode      in_eOpenMode,       // Open mode.
        AkFileSystemFlags * in_pFlags,      // Special flags. Can pass NULL.
		bool &			io_bSyncOpen,		// If true, the file must be opened synchronously. Otherwise it is left at the File Location Resolver's discretion. Return false if Open needs to be deferred.
        AkFileDesc &    out_fileDesc        // Returned file descriptor.
        );

    // Override Open (ID): Search file in each LUT first. If it cannot be found, use base class services.
    // If the file is found in the LUTs, open is always synchronous.
	virtual AKRESULT Open( 
        AkFileID        in_fileID,          // File ID.
        AkOpenMode      in_eOpenMode,       // Open mode.
        AkFileSystemFlags * in_pFlags,      // Special flags. Can pass NULL.
		bool &			io_bSyncOpen,		// If true, the file must be opened synchronously. Otherwise it is left at the File Location Resolver's discretion. Return false if Open needs to be deferred.
        AkFileDesc &    out_fileDesc        // Returned file descriptor.
        );

    //
	// IAkLowLevelIOHook interface overriden methods.
	// ---------------------------------------------------------------

	// Override Close: Do not close handle if file descriptor is part of the current packaged file.
    virtual AKRESULT Close(
        AkFileDesc &	in_fileDesc			// File descriptor.
        );

	// Override GetBlockSize: Get the block size of the LUT if a file package is loaded.
	virtual AkUInt32 GetBlockSize(
        AkFileDesc &	in_fileDesc			// File descriptor.
        );

protected:

	// Language change handling.
    // ------------------------------------------

	// Handler for global language change.
	static AK_FUNC( void, LanguageChangeHandler )( 
		const AkOSChar * const in_pLanguageName,// New language name.
		void * in_pCookie						// Cookie that was passed to AddLanguageChangeObserver().
		)
	{
		((CAkFilePackageLowLevelIO<T_LLIOHOOK_FILELOC, T_PACKAGE>*)in_pCookie)->OnLanguageChange( in_pLanguageName );
	}

	// Updates language of all loaded packages. Packages keep a language ID to help them find 
	// language-specific assets quickly.
	void OnLanguageChange( 
		const AkOSChar * const in_pLanguageName	// New language name.
		);


	// File package handling methods.
    // ------------------------------------------

	// Loads a file package, with a given file package reader.
	AKRESULT _LoadFilePackage(
        const AkOSChar*			in_pszFilePackageName,	// File package name. Location is resolved using base class' Open().
		AkFilePackageReader &	in_reader,				// File package reader.
		AkPriority				in_readerPriority,		// File package reader priority heuristic.
		T_PACKAGE *&			out_pPackage			// Returned package
        );

    // Searches the LUT to find the file data associated with the FileID.
    // Returns AK_Success if the file is found.
	template <class T_FILEID>
    AKRESULT FindPackagedFile( 
		T_PACKAGE *			in_pPackage,	// Package to search into.
		T_FILEID			in_fileID,		// File ID.
		AkFileSystemFlags * in_pFlags,		// Special flags. Can pass NULL.
		AkFileDesc &		out_fileDesc	// Returned file descriptor.
		);

	virtual void InitFileDesc( T_PACKAGE * /*in_pPackage*/, AkFileDesc & /*io_fileDesc*/){};
	
	// Returns true if file described by in_fileDesc is in a package.
	inline bool IsInPackage( 
		const AkFileDesc & in_fileDesc		// File descriptor.
		)
	{
		AKASSERT(in_fileDesc.pPackage == NULL || in_fileDesc.uCustomParamSize > 0);
		return in_fileDesc.pPackage != NULL;
	}

protected:
	// List of loaded packages.
	ListFilePackages	m_packages;
	CAkLock				m_lock;
	bool				m_bRegisteredToLangChg;	// True after registering to language change notifications.
};

#include "AkFilePackageLowLevelIO.inl"

#endif //_AK_FILE_PACKAGE_LOW_LEVEL_IO_H_
