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
// AkFilePackage.h
//
// This class represents a file package that was created with the 
// AkFilePackager utility app (located in ($WWISESDK)/samples/FilePackager/). 
// It holds a system file handle and a look-up table (CAkFilePackageLUT).
//
// CAkFilePackage objects can be chained together using the ListFilePackages
// typedef defined below.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "AkFilePackage.h"

// Destroy file package and free memory.
void CAkFilePackage::Destroy()
{
	// Cache memory pointer to ensure memory is free'd _after_ deleting this.
	void * pToRelease	= m_pToRelease;

	// Call destructor.
	this->~CAkFilePackage();

	// Free memory.
	ClearMemory(pToRelease);
}

void CAkFilePackage::ClearMemory(
	void *		in_pMemToRelease	// Memory to free.
	)
{
	if (in_pMemToRelease)
	{
		AkFree(AkMemID_FilePackage, in_pMemToRelease);
	}
}
