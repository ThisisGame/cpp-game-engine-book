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
// AkFileLocationBase.h
//
// Basic file location resolving: Uses simple path concatenation logic.
// Exposes basic path functions for convenience.
// For more details on resolving file location, refer to section "File Location" inside
// "Going Further > Overriding Managers > Streaming / Stream Manager > Low-Level I/O"
// of the SDK documentation. 
//
//////////////////////////////////////////////////////////////////////

#ifndef _AK_FILE_HELPERS_BASE_H
#define _AK_FILE_HELPERS_BASE_H

#include <AK/SoundEngine/Common/AkTypes.h>
#include <AK/Tools/Common/AkPlatformFuncs.h>

/// Provides platform-agnostic helper functions
class CAkFileHelpersBase
{
public:
    /// Joins input in_paths into out_path with AK_PATH_SEPARATOR for up to maxSize characters.
    /// Expect Paths to be of type "[const] AkOSChar*".
    template <typename... Paths>
    static void JoinPath(AkOSChar* out_path, AkUInt32 maxSize, Paths... in_paths)
    {
        if (out_path == nullptr)
            return;

        // Don't transform a relative path into an absolute path
        // if first is empty (e.g. dir/file.txt -> /dir/file.txt).
        // Also prevents duplicate path separators.
        bool lastPathEmpty = true;
        
        out_path[0] = '\0';
        const int nbPaths = sizeof...(in_paths);
        const AkOSChar* paths[nbPaths] = { in_paths... };
        for (AkUInt32 i = 0; i < nbPaths; ++i)
        {
            if (lastPathEmpty == false)
                AKPLATFORM::SafeStrCat(out_path, AK_PATH_SEPARATOR, maxSize);

            AKPLATFORM::SafeStrCat(out_path, paths[i], maxSize);
            lastPathEmpty = AKPLATFORM::OsStrLen(paths[i]) == 0;
        }
    }

    ///
    /// Calls function f for each directory level
    /// E.g., for "./Folder/B/Filename.txt" with bSkipFilename = true:
    ///   -> ./Folder
    ///   -> ./Folder/B
    ///
    /// Iteration is stopped if Func(const AkOsChar*) doesn't return AK_Success.
    /// Accepts absolute Win32 and Unix paths as well as relative paths.
    /// Using bSkipFilename = true will ignore the last part of the path.
    ///
    template <class Func>
    static AKRESULT ForEachDirectoryLevel(const AkOSChar* in_path, Func f, bool bSkipFilename = false)
    {
        if (in_path == nullptr)
            return AK_Success;

        size_t len = AKPLATFORM::OsStrLen(in_path);
        if (len < 2)
            return AK_Success;

        // Create an overwritable local copy
        AkOSChar* path = (AkOSChar*)AkAlloca((len + 1) * sizeof(AkOSChar));
        AKPLATFORM::SafeStrCpy(path, in_path, len + 1);

        AkOSChar* pChar = &path[0];
        AkOSChar* pEnd = &path[len];

        auto IsSeparator = [](AkOSChar* pChar) {
            return pChar[0] == '/' || pChar[0] == '\\';
        };

        if (bSkipFilename)
        {
            // Split path in two at the last separator to remove the file name
            int lastSeparatorIndex = 0;
            while (pChar < pEnd)
            {
                if (IsSeparator(pChar))
                    lastSeparatorIndex = (int)((const AkOSChar*)pChar - &path[0]);

                pChar++;
            }
            if (lastSeparatorIndex == 0)
                return AK_Success;

            path[lastSeparatorIndex] = '\0';
            pChar = &path[0];
        }

        // Skip unix path first slash, e.g: /mnt
        // or network drive first two slashes, e.g. \\NetworkDrive
        while (IsSeparator(pChar) && pChar < pEnd)
            pChar++;

        // Also skip the first part ("mnt" or "NetworkDrive")
        if (pChar != &path[0])
        {
            while (IsSeparator(pChar) && pChar < pEnd)
                pChar++;
            while (!IsSeparator(pChar) && pChar < pEnd)
                pChar++;
        }

        // Skip Win32 path drive, e.g. "C:/" or mount points (rom:)
        if (pChar == &path[0])
        {
            bool foundColons = false;

            // "C:"
            while (!IsSeparator(pChar) && pChar < pEnd)
            {
                if (pChar[0] == ':')
                    foundColons = true;
                
                pChar++;
            }

            // "/"
            while (IsSeparator(pChar) && pChar < pEnd)
                pChar++;

            // Found nothing, restore pointer to beginning.
            if (!foundColons)
                pChar = &path[0];
        }

        // Skip "./" for relative paths
        if (pChar == &path[0])
        {
            while ((pChar[0] == AK_PATH_SEPARATOR[0] || pChar[0] == '.') && pChar < pEnd)
                pChar++;
        }

        while (pChar < pEnd)
        {
            // Handle duplicate separators
            while (IsSeparator(pChar) && pChar < pEnd)
                pChar++;

            // Move to the next separator
            while (!IsSeparator(pChar) && pChar < pEnd)
                pChar++;

            // Split path by adding a null-termination char
            AkOSChar separator = pChar[0];
            pChar[0] = '\0';
            
            AKRESULT res = f(path);
            if (res != AK_Success) // stop if requested
                return res;

            pChar[0] = separator; // restore original path
            pChar++;
        }

        return AK_Success;
    }
};

#endif // _AK_FILE_HELPERS_BASE_H
