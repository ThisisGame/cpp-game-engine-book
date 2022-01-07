/*******************************************************************************
The content of this file includes portions of the AUDIOKINETIC Wwise Technology
released in source code form as part of the SDK installer package.

Commercial License Usage

Licensees holding valid commercial licenses to the AUDIOKINETIC Wwise Technology
may use this file in accordance with the end user license agreement provided 
with the software or, alternatively, in accordance with the terms contained in a
written agreement between you and Audiokinetic Inc.

  Copyright (c) 2020 Audiokinetic Inc.
*******************************************************************************/

// Platform.cpp
/// \file 
/// Contains definitions for functions declared in Platform.h

#include <AK/Tools/Win32/AkPlatformFuncs.h>
#include "stdafx.h"
#include "Platform.h"

void ShortenPath(AkOSChar* io_Path)
{
	// Remove one directory level and the "\.." that follows it whenever possible in the path.
	AkOSChar token[] = { AK_PATH_SEPARATOR[0] , '.','.', '\0'};
	const size_t tokensize = AKPLATFORM::OsStrLen(token);

	size_t searchIdx = 1;// Skip the case the path would start with the searched token.
	size_t len = AKPLATFORM::OsStrLen(io_Path);
	while (searchIdx < len - tokensize)
	{
		if (!AKPLATFORM::OsStrNCmp(&io_Path[searchIdx], token, tokensize))
		{
			size_t prevSeparatorIndex = searchIdx - 1; // The searchIdx is always greater than 1
			while (io_Path[prevSeparatorIndex] != AK_PATH_SEPARATOR[0] && prevSeparatorIndex >= 0) prevSeparatorIndex--;

			AkOSChar tmpPath[MAX_PATH];
			AKPLATFORM::SafeStrCpy(tmpPath, &io_Path[searchIdx + tokensize], MAX_PATH);

			// Remove one directory level and the "\.." that follows it
			io_Path[prevSeparatorIndex] = '\0';
			AKPLATFORM::SafeStrCat(io_Path, tmpPath, MAX_PATH);

			searchIdx = AkMax(1, prevSeparatorIndex);// Go back to the previous separator index to start searching again.
			len = AKPLATFORM::OsStrLen(io_Path);// Update the size as it is now shorter.
		}
		else
		{
			searchIdx++;
		}
	}
}

AkOSChar* GetSoundBanksPath(const wchar_t* in_pRelativePath)
{
	static AkOSChar soundBanksPath[MAX_PATH];
	static size_t pathLen = 0;

	if (pathLen == 0)
	{
		char szFilename[MAX_PATH];
		AkOSChar *path;
		// Get the executable path
		DWORD result = GetModuleFileNameA(NULL, szFilename, MAX_PATH);
		if (result > 0)
		{
			CONVERT_CHAR_TO_OSCHAR(szFilename, path);
			// Remove the executable name from the path
			int i = result - 1;
			while (path[i] != AK_PATH_SEPARATOR[0])
			{
				path[i] = '\0';
				i--;
			}

			// Copy the path to its final destination
			AKPLATFORM::SafeStrCpy(soundBanksPath, path, MAX_PATH);
			// Concatenate the soundbanks path
			AKPLATFORM::SafeStrCat(soundBanksPath, in_pRelativePath, MAX_PATH);
			// Shorten the path as much as possible by removing the "\.."
			ShortenPath(soundBanksPath);
			pathLen = AKPLATFORM::OsStrLen(soundBanksPath);
		}
	}
	return soundBanksPath;
}