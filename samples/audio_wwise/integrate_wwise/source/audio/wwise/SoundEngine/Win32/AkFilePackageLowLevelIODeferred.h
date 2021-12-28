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
// AkFilePackageLowLevelIODeferred.h
//
// Extends the CAkDefaultIOHookDeferred low level I/O hook with File 
// Package handling functionality. 
//
// See AkDefaultIOHookBlocking.h for details on using the deferred 
// low level I/O hook. 
// 
// See AkFilePackageLowLevelIO.h for details on using file packages.
//
//////////////////////////////////////////////////////////////////////

#ifndef _AK_FILE_PACKAGE_LOW_LEVEL_IO_DEFERRED_H_
#define _AK_FILE_PACKAGE_LOW_LEVEL_IO_DEFERRED_H_

#include "../Common/AkFilePackageLowLevelIO.h"
#include "AkDefaultIOHookDeferred.h"

class CAkFilePackageLowLevelIODeferred
	: public CAkFilePackageLowLevelIO<CAkDefaultIOHookDeferred>
{
public:
	CAkFilePackageLowLevelIODeferred() {}
	virtual ~CAkFilePackageLowLevelIODeferred() {}

	// Override Cancel: The Windows platform SDK only permits cancellations of all transfers 
	// for a given file handle. Since the packaged files share the same handle, we cannot do this.
	virtual void Cancel(
		AkFileDesc &			in_fileDesc,		// File descriptor.
		AkAsyncIOTransferInfo & io_transferInfo,	// Transfer info to cancel.
		bool & io_bCancelAllTransfersForThisFile	// Flag indicating whether all transfers should be cancelled for this file (see notes in function description).
		)
	{
		if ( !IsInPackage( in_fileDesc ) )
		{
			CAkDefaultIOHookDeferred::Cancel(
				in_fileDesc,		// File descriptor.
				io_transferInfo,	// Transfer info to cancel.
				io_bCancelAllTransfersForThisFile	// Flag indicating whether all transfers should be cancelled for this file (see notes in function description).
				);
		}
	}
};

#endif //_AK_FILE_PACKAGE_LOW_LEVEL_IO_DEFERRED_H_
