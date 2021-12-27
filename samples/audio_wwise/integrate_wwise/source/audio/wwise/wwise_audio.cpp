//
// Created by captainchen on 2021/12/27.
//

#include "wwise_audio.h"
#include <AK/SoundEngine/Common/AkTypes.h>
#include <AK/SoundEngine/Common/AkMemoryMgr.h>		// Memory Manager
#include <AK/SoundEngine/Common/AkModule.h>			// Default memory and stream managers
#include <AK/SoundEngine/Common/IAkStreamMgr.h>		// Streaming Manager
#include <AK/SoundEngine/Common/AkSoundEngine.h>    // Sound engine
#include <AK/MusicEngine/Common/AkMusicEngine.h>	// Music Engine
#include <AK/SoundEngine/Common/AkStreamMgrModule.h>	// AkStreamMgrModule
#include <AK/SpatialAudio/Common/AkSpatialAudio.h>	// Spatial Audio module
#include <AK/SoundEngine/Common/AkCallback.h>    // Callback
#ifndef AK_OPTIMIZED
#include <AK/Comm/AkCommunication.h>	// Communication between Wwise and the game (excluded in release build)
#endif
#include "utils/debug.h"

void WwiseAudio::Init() {
    // Create and initialize an instance of the default memory manager. Note
    // that you can override the default memory manager with your own. Refer
    // to the SDK documentation for more information.
    AkMemSettings memSettings;
    AKRESULT res = AK::MemoryMgr::Init( &memSettings );
    if ( res != AK_Success )
    {
        DEBUG_LOG_ERROR("WwiseAudio::Init() AK::MemoryMgr::Init() failed,res:{}", res);
        return;
    }

    //
    // Create and initialize an instance of the default streaming manager. Note
    // that you can override the default streaming manager with your own. Refer
    // to the SDK documentation for more information.
    //

    // Customize the Stream Manager settings here.
    AkStreamMgrSettings streamMgrSettings;

    if ( !AK::StreamMgr::Create( streamMgrSettings ) )
    {
        DEBUG_LOG_ERROR("WwiseAudio::Init() AK::StreamMgr::Create() failed");
        return;
    }

    //
    // Create a streaming device with blocking low-level I/O handshaking.
    // Note that you can override the default low-level I/O module with your own. Refer
    // to the SDK documentation for more information.
    //

    // CAkFilePackageLowLevelIOBlocking::Init() creates a streaming device
    // in the Stream Manager, and registers itself as the File Location Resolver.
    AkDeviceSettings deviceSettings;
    deviceSettings.bUseStreamCache = true;
    CAkFilePackageLowLevelIODeferred
    res = m_pLowLevelIO->Init( m_deviceSettings );
    if ( res != AK_Success )
    {
        __AK_OSCHAR_SNPRINTF( in_szErrorBuffer, in_unErrorBufferCharCount, AKTEXT("m_lowLevelIO.Init() returned AKRESULT %d"), res );
        return false;
    }

    //
    // Create the Sound Engine
    // Using default initialization parameters
    //

    res = AK::SoundEngine::Init( &m_initSettings, &m_platformInitSettings );
    if ( res != AK_Success )
    {
        __AK_OSCHAR_SNPRINTF( in_szErrorBuffer, in_unErrorBufferCharCount, AKTEXT("AK::SoundEngine::Init() returned AKRESULT %d"), res );
        return false;
    }

    //
    // Initialize the music engine
    // Using default initialization parameters
    //

    res = AK::MusicEngine::Init( &m_musicInit );
    if ( res != AK_Success )
    {
        __AK_OSCHAR_SNPRINTF( in_szErrorBuffer, in_unErrorBufferCharCount, AKTEXT("AK::MusicEngine::Init() returned AKRESULT %d"), res );
        return false;
    }

#if !defined AK_OPTIMIZED && !defined INTEGRATIONDEMO_DISABLECOMM
    //
    // Initialize communications (not in release build!)
    //
    AKPLATFORM::SafeStrCpy(m_commSettings.szAppNetworkName, "Integration Demo", AK_COMM_SETTINGS_MAX_STRING_SIZE);
    res = AK::Comm::Init(m_commSettings);
    if ( res != AK_Success )
    {
        __AK_OSCHAR_SNPRINTF( in_szErrorBuffer, in_unErrorBufferCharCount, AKTEXT("AK::Comm::Init() returned AKRESULT %d. Communication between the Wwise authoring application and the game will not be possible."), res );
    }
#endif // AK_OPTIMIZED

    AkSpatialAudioInitSettings settings;

    res = AK::SpatialAudio::Init(settings);
    if (res != AK_Success)
    {
        __AK_OSCHAR_SNPRINTF(in_szErrorBuffer, in_unErrorBufferCharCount, AKTEXT("AK::SpatialAudio::Init() returned AKRESULT %d"), res);
        return false;
    }

    AK::SoundEngine::RegisterGameObj(LISTENER_ID, "Listener (Default)");
    AK::SoundEngine::SetDefaultListeners(&LISTENER_ID, 1);

    // For platforms with a read-only bank path, add a writable folder
    // to the list of base paths. IO will fallback on this path
    // when opening a file for writing fails.
#if defined(WRITABLE_PATH)
    m_pLowLevelIO->AddBasePath(WRITABLE_PATH);
#endif

    // Set the path to the SoundBank Files last.
    // The last base path is always the first queried for files.
    m_pLowLevelIO->SetBasePath(SOUND_BANK_PATH);

    // Set global language. Low-level I/O devices can use this string to find language-specific assets.
    if (AK::StreamMgr::SetCurrentLanguage(AKTEXT("English(US)")) != AK_Success)
        return false;

    AK::SoundEngine::RegisterResourceMonitorCallback(ResourceMonitorDataCallback);
}

void WwiseAudio::Update() {

}