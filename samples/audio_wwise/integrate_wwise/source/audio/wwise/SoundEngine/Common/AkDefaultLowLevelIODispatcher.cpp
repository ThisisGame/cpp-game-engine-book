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
// AkDefaultLowLevelIODispatcher.cpp
//
// Canvas for implementation of the AK::StreamMgr::IAkFileLocationResolver,
// meant to be used in a multiple streaming devices system. It is this
// object that should be registered as the one and only 
// AK::StreamMgr::IAkFileLocationResolver of the Stream Manager
// (by calling AK::StreamMgr::SetFileLocationResolver()).
// 
// It forwards the calls to Open() to one of the low level I/O devices 
// that were added to it using AddDevice(). These devices must thus also 
// implement AK::StreamMgr::IAkFileLocationResolver.
// 
// The strategy for device dispatching is that of a chain of responsibility:
// the dispatcher asks the first file resolver hook to open the file. If it 
// fails, then it tries with the second, and so on, until a hook succeeds. 
// This is inefficient. In your game, you should implement a strategy of 
// your own (see CAkDefaultLowLevelIODispatcher::Open()).
//
//////////////////////////////////////////////////////////////////////

#include "AkDefaultLowLevelIODispatcher.h"
#include <AK/Tools/Common/AkAssert.h>


CAkDefaultLowLevelIODispatcher::CAkDefaultLowLevelIODispatcher()
:m_uNumDevices( 0 )
{
	RemoveAllDevices();
}

CAkDefaultLowLevelIODispatcher::~CAkDefaultLowLevelIODispatcher()
{
}

// Returns a file descriptor for a given file name (string).
AKRESULT CAkDefaultLowLevelIODispatcher::Open( 
    const AkOSChar* in_pszFileName,     // File name.
    AkOpenMode      in_eOpenMode,       // Open mode.
    AkFileSystemFlags * in_pFlags,      // Special flags. Can pass NULL.
	bool &			io_bSyncOpen,		// If true, the file must be opened synchronously. Otherwise it is left at the File Location Resolver's discretion. Return false if Open needs to be deferred.
    AkFileDesc &    out_fileDesc        // Returned file descriptor.
    )
{
	// Here, you need to define a strategy to determine which device is going to handle this file's I/O.
	// You could use some naming convention, or use the AkFileSystemFlags or file name extension if it depends 
	// on file type, or define a map, or read the mapping from an XML file... it is up to the game's organization.
	// Since this default implementation doesn't know anything about that, it forwards the calls to each 
	// device until one of them succeeds. 

	// Disable deferred opening because devices may usually return AK_Success if io_bSyncOpen=false,
	// and we count on the fact that they will return AK_Fail to select the proper device.
	io_bSyncOpen = true;

	AKRESULT eResult = AK_FileNotFound;
	AkUInt32 uDevice = 0;
	while ( uDevice < AK_MAX_IO_DEVICES
			&& eResult != AK_Success )
	{
		if ( m_arDevices[uDevice] )
		{
			eResult = m_arDevices[uDevice]->Open( 
				in_pszFileName,     // File name.
				in_eOpenMode,       // Open mode.
				in_pFlags,			// Special flags. Can pass NULL.
				io_bSyncOpen,		// If true, the file must be opened synchronously. Otherwise it is left at the File Location Resolver's discretion. Return false if Open needs to be deferred.
				out_fileDesc        // Returned file descriptor.
				);
			AKASSERT( io_bSyncOpen || !"It is illegal to reset io_bSyncOpen" );
		}
		++uDevice;
	}

	return eResult; 
}

// Returns a file descriptor for a given file ID.
AKRESULT CAkDefaultLowLevelIODispatcher::Open( 
    AkFileID        in_fileID,          // File ID.
    AkOpenMode      in_eOpenMode,       // Open mode.
    AkFileSystemFlags * in_pFlags,      // Special flags. Can pass NULL.
	bool &			io_bSyncOpen,		// If true, the file must be opened synchronously. Otherwise it is left at the File Location Resolver's discretion. Return false if Open needs to be deferred.
    AkFileDesc &    out_fileDesc        // Returned file descriptor.
    )
{
    // Here, you need to define a strategy to determine which device is going to handle this file's I/O.
	// You could use the AkFileSystemFlags if it depends on file type, or define a map, or read the mapping 
	// from an XML file... it is up to the game's organization.
	// Since this default implementation doesn't know anything about that, it forwards the calls to each 
	// device until one of them succeeds. 

	// Disable deferred opening because devices may usually return AK_Success if io_bSyncOpen=false,
	// and we count on the fact that they will return AK_Fail to select the proper device.
	io_bSyncOpen = true;
    
	AKRESULT eResult = AK_FileNotFound;
	AkUInt32 uDevice = 0;
	while ( uDevice < AK_MAX_IO_DEVICES
			&& eResult != AK_Success )
	{
		if ( m_arDevices[uDevice] )
		{
			eResult = m_arDevices[uDevice]->Open( 
				in_fileID,          // File ID.
				in_eOpenMode,       // Open mode.
				in_pFlags,			// Special flags. Can pass NULL.
				io_bSyncOpen,		// If true, the file must be opened synchronously. Otherwise it is left at the File Location Resolver's discretion. Return false if Open needs to be deferred.
				out_fileDesc        // Returned file descriptor.
				);
			AKASSERT( io_bSyncOpen || !"It is illegal to reset io_bSyncOpen" );
		}
		++uDevice;
	}

	return eResult; 
}

AKRESULT CAkDefaultLowLevelIODispatcher::AddDevice(
	AK::StreamMgr::IAkFileLocationResolver *	in_pHook
	)
{
	// Add the device in a free slot.
	for ( AkUInt32 uRecord = 0; uRecord < AK_MAX_IO_DEVICES; uRecord++ )
	{
		if ( !m_arDevices[uRecord] )
		{
			m_arDevices[uRecord] = in_pHook;
			++m_uNumDevices;
			return AK_Success;
		}
	}
	AKASSERT( !"Cannot hold any more I/O devices" );
	return AK_Fail;
}

void CAkDefaultLowLevelIODispatcher::RemoveAllDevices()
{
	for ( AkUInt32 uRecord = 0; uRecord < AK_MAX_IO_DEVICES; uRecord++ )
		m_arDevices[uRecord] = NULL;
	m_uNumDevices = 0;
}

