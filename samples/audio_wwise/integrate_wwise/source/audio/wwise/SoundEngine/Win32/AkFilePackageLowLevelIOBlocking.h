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
// AkFilePackageLowLevelIOBlocking.h
//
// Extends the CAkDefaultIOHookBlocking low level I/O hook with File 
// Package handling functionality. 
//
// See AkDefaultIOHookBlocking.h for details on using the blocking 
// low level I/O hook. 
// 
// See AkFilePackageLowLevelIO.h for details on using file packages.
//
//////////////////////////////////////////////////////////////////////

#ifndef _AK_FILE_PACKAGE_LOW_LEVEL_IO_BLOCKING_H_
#define _AK_FILE_PACKAGE_LOW_LEVEL_IO_BLOCKING_H_

#include "../Common/AkFilePackageLowLevelIO.h"
#include "AkDefaultIOHookBlocking.h"

class CAkFilePackageLowLevelIOBlocking 
	: public CAkFilePackageLowLevelIO<CAkDefaultIOHookBlocking>
{
public:
	CAkFilePackageLowLevelIOBlocking() {}
	virtual ~CAkFilePackageLowLevelIOBlocking() {}
};

#endif //_AK_FILE_PACKAGE_LOW_LEVEL_IO_BLOCKING_H_
