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
// AkFileHelpers.h
//
// Platform-specific helpers for files.
//
//////////////////////////////////////////////////////////////////////

#ifndef _AK_FILE_HELPERS_H_
#define _AK_FILE_HELPERS_H_

#include "../Common/AkFileHelpersBase.h"

#include <AK/Tools/Common/AkAssert.h>
#include <windows.h>
#include <AK/SoundEngine/Common/AkStreamMgrModule.h>

class CAkFileHelpers : public CAkFileHelpersBase
{
public:
	// Wrapper for Win32 CreateFile().
	static AKRESULT OpenFile( 
        const AkOSChar* in_pszFilename,     // File name.
        AkOpenMode      in_eOpenMode,       // Open mode.
        bool            in_bOverlappedIO,	// Use FILE_FLAG_OVERLAPPED flag.
        bool            in_bUnbufferedIO,   // Use FILE_FLAG_NO_BUFFERING flag.
        AkFileHandle &  out_hFile           // Returned file identifier/handle.
        )
	{
		// Check parameters.
		if ( !in_pszFilename )
		{
			AKASSERT( !"NULL file name" );
			return AK_InvalidParameter;
		}

		// Open mode
		DWORD dwShareMode;
		DWORD dwAccessMode;
		DWORD dwCreationDisposition;
		switch ( in_eOpenMode )
		{
			case AK_OpenModeRead:
					dwShareMode = FILE_SHARE_READ | FILE_SHARE_WRITE;
					dwAccessMode = GENERIC_READ;
					dwCreationDisposition = OPEN_EXISTING;
				break;
			case AK_OpenModeWrite:
					dwShareMode = FILE_SHARE_READ;
					dwAccessMode = GENERIC_WRITE;
					dwCreationDisposition = OPEN_ALWAYS;
				break;
			case AK_OpenModeWriteOvrwr:
					dwShareMode = FILE_SHARE_READ;
					dwAccessMode = GENERIC_WRITE;
					dwCreationDisposition = CREATE_ALWAYS;
				break;
			case AK_OpenModeReadWrite:
					dwShareMode = FILE_SHARE_READ;
					dwAccessMode = GENERIC_READ | GENERIC_WRITE;
					dwCreationDisposition = OPEN_ALWAYS;
				break;
			default:
					AKASSERT( !"Invalid open mode" );
					out_hFile = NULL;
					return AK_InvalidParameter;
				break;
		}

		// Flags
		DWORD dwFlags = FILE_FLAG_SEQUENTIAL_SCAN;
		if ( in_bUnbufferedIO && in_eOpenMode == AK_OpenModeRead )
			dwFlags |= FILE_FLAG_NO_BUFFERING;
		if ( in_bOverlappedIO )
			dwFlags |= FILE_FLAG_OVERLAPPED;

		// Create the file handle.
#if defined(AK_USE_UWP_API) && !defined(AK_XBOX) // Xbox can use "normal" IO
		out_hFile = ::CreateFile2( 
			in_pszFilename,
			dwAccessMode,
			dwShareMode, 
			dwCreationDisposition,
			NULL );
#else
		out_hFile = ::CreateFileW( 
			in_pszFilename,
			dwAccessMode,
			dwShareMode, 
			NULL,
			dwCreationDisposition,
			dwFlags,
			NULL );
#endif
		if( out_hFile == INVALID_HANDLE_VALUE )
		{
			DWORD dwAllocError = ::GetLastError();
			if ( ERROR_FILE_NOT_FOUND == dwAllocError ||
				 ERROR_PATH_NOT_FOUND == dwAllocError )
				return AK_FileNotFound;

			return AK_Fail;
		}

		return AK_Success;
	}

	//Open file and fill AkFileDesc
	static AKRESULT Open(
		const AkOSChar* in_pszFileName,     // File name.
		AkOpenMode      in_eOpenMode,       // Open mode.
		bool			in_bOverlapped,		// Overlapped IO
		AkFileDesc &    out_fileDesc		// File descriptor
		)
	{
		// Open the file without FILE_FLAG_OVERLAPPED and FILE_FLAG_NO_BUFFERING flags.
		AKRESULT eResult = OpenFile( 
			in_pszFileName,
			in_eOpenMode,
			in_bOverlapped,
			in_bOverlapped, //No buffering flag goes in pair with overlapped flag for now.  Block size must be set accordingly
			out_fileDesc.hFile );

		if (eResult == AK_Success)
		{
#ifdef AK_USE_UWP_API
			FILE_STANDARD_INFO info;
			::GetFileInformationByHandleEx(out_fileDesc.hFile, FileStandardInfo, &info, sizeof(info));
			out_fileDesc.iFileSize = info.EndOfFile.QuadPart;
#else
			ULARGE_INTEGER Temp;
			Temp.LowPart = ::GetFileSize(out_fileDesc.hFile, (LPDWORD)&Temp.HighPart);
			out_fileDesc.iFileSize = Temp.QuadPart;
#endif
		}
		return eResult;
	}

	// Wrapper for system file handle closing.
	static AKRESULT CloseFile( AkFileHandle in_hFile )
	{
		::FlushFileBuffers(in_hFile);
		if ( ::CloseHandle( in_hFile ) )
			return AK_Success;
		
		AKASSERT( !"Failed to close file handle" );
		return AK_Fail;
	}

	//
	// Simple platform-independent API to open and read files using AkFileHandles, 
	// with blocking calls and minimal constraints.
	// ---------------------------------------------------------------------------

	// Open file to use with ReadBlocking().
	static AKRESULT OpenBlocking(
        const AkOSChar* in_pszFilename,     // File name.
        AkFileHandle &  out_hFile           // Returned file handle.
		)
	{
		return OpenFile( 
			in_pszFilename,
			AK_OpenModeRead,
			false,
			false, 
			out_hFile );
	}

	// Required block size for reads (used by ReadBlocking() below).
	static const AkUInt32 s_uRequiredBlockSize = 1;

	// Simple blocking read method.
	static AKRESULT ReadBlocking(
        AkFileHandle &	in_hFile,			// Returned file identifier/handle.
		void *			in_pBuffer,			// Buffer. Must be aligned on CAkFileHelpers::s_uRequiredBlockSize boundary.
		AkUInt32		in_uPosition,		// Position from which to start reading.
		AkUInt32		in_uSizeToRead,		// Size to read. Must be a multiple of CAkFileHelpers::s_uRequiredBlockSize.
		AkUInt32 &		out_uSizeRead		// Returned size read.        
		)
	{
		AKASSERT( in_uSizeToRead % s_uRequiredBlockSize == 0 
			&& in_uPosition % s_uRequiredBlockSize == 0 );

#ifdef AK_USE_UWP_API
		LARGE_INTEGER uPosition;
		uPosition.QuadPart = in_uPosition;
		if ( SetFilePointerEx( in_hFile, uPosition, NULL, FILE_BEGIN ) == FALSE )
			return AK_Fail;
#else
		if ( SetFilePointer( in_hFile, in_uPosition, NULL, FILE_BEGIN ) != in_uPosition )
			return AK_Fail;
#endif
		if ( ::ReadFile( in_hFile, in_pBuffer, in_uSizeToRead, (LPDWORD)&out_uSizeRead, NULL ) )
			return AK_Success;
		return AK_Fail;
	}

	/// Returns AK_Success if the directory is valid, AK_Fail if not.
	/// For validation purposes only.
	/// Some platforms may return AK_NotImplemented, in this case you cannot rely on it.
	static AKRESULT CheckDirectoryExists( const AkOSChar* in_pszBasePath )
	{
        return AK_Success;
//		DWORD fileAttributes = INVALID_FILE_ATTRIBUTES;
//#ifdef AK_USE_UWP_API
//		WIN32_FILE_ATTRIBUTE_DATA    fileInfo;
//		if( GetFileAttributesEx(in_pszBasePath, GetFileExInfoStandard, &fileInfo ) )
//		{
//			fileAttributes = fileInfo.dwFileAttributes;
//		}
//#else
//		fileAttributes = GetFileAttributes( in_pszBasePath );
//#endif
//		if (fileAttributes == INVALID_FILE_ATTRIBUTES)
//			return AK_Fail;  //something is wrong with your path!
//
//		if (fileAttributes & FILE_ATTRIBUTE_DIRECTORY)
//			return AK_Success;   // this is a directory!
//
//
//		return AK_Fail;    // this is not a directory!
	}

	static AKRESULT WriteBlocking(
		AkFileHandle &	in_hFile,			// Returned file identifier/handle.
		void *			in_pData,			// Buffer. Must be aligned on CAkFileHelpers::s_uRequiredBlockSize boundary.		
		AkUInt64		in_uPosition,		// Position from which to start writing.
		AkUInt32		in_uSizeToWrite)
	{
		AKASSERT( in_pData && in_hFile != INVALID_HANDLE_VALUE );

		OVERLAPPED overlapped;
		overlapped.Offset = (DWORD)( in_uPosition & 0xFFFFFFFF );
		overlapped.OffsetHigh = (DWORD)( ( in_uPosition >> 32 ) & 0xFFFFFFFF );
		overlapped.hEvent = NULL;

		DWORD uSizeTransferred;

		if ( ::WriteFile( 
			in_hFile,
			in_pData,
			in_uSizeToWrite,
			&uSizeTransferred,
			&overlapped ) )
		{
			AKASSERT( uSizeTransferred == in_uSizeToWrite );
			return AK_Success;
		}

		return AK_Fail;
	}

//	static AKRESULT CreateEmptyDirectory(const AkOSChar* in_pszDirectoryPath)
//	{
//		bool bSuccess = ::CreateDirectory(in_pszDirectoryPath, NULL) == TRUE;
//		if (!bSuccess && ::GetLastError() != ERROR_ALREADY_EXISTS)
//			return AK_Fail;
//
//		return AK_Success;
//	}
//
//	static AKRESULT RemoveEmptyDirectory(const AkOSChar* in_pszDirectoryPath)
//	{
//		bool bSuccess = ::RemoveDirectory(in_pszDirectoryPath) == TRUE;
//		if (!bSuccess)
//			return AK_Fail;
//
//		return AK_Success;
//	}

	static AKRESULT GetDefaultWritablePath(AkOSChar* out_pszPath, AkUInt32 in_pathMaxSize)
	{
		if (out_pszPath == nullptr)
			return AK_InsufficientMemory;

		out_pszPath[0] = '\0';

#if defined(AK_WIN)
		// No strict writable path enforcement on Windows (return "")
#elif defined(AK_XBOX)
		AKPLATFORM::SafeStrCat(out_pszPath, AKTEXT("D:\\"), in_pathMaxSize);
#else
		return AK_NotImplemented;
#endif
		
		return AK_Success;
	}
};

#endif //_AK_FILE_HELPERS_H_
