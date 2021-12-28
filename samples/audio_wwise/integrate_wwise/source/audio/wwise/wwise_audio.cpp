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
#include "SoundEngine/Win32/AkFilePackageLowLevelIODeferred.h"
#include "SoundEngine/Win32/Platform.h"
#include "utils/debug.h"

static const AkGameObjectID LISTENER_ID = 10000;

void ResourceMonitorDataCallback(const AkResourceMonitorDataSummary * in_pdataSummary);

void WwiseAudio::Init() {
    // 创建并初始化默认的内存管理器。注意，你可以使用自己的内存管理器覆盖默认的内存管理器。详细信息请参考SDK文档。
    AkMemSettings memSettings;
    AKRESULT res = AK::MemoryMgr::Init( &memSettings );
    if ( res != AK_Success ){
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

    if ( !AK::StreamMgr::Create( streamMgrSettings ) ){
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
    CAkFilePackageLowLevelIODeferred* lowLevelIoDeferred= new CAkFilePackageLowLevelIODeferred();
    res = lowLevelIoDeferred->Init( deviceSettings );
    if ( res != AK_Success ){
        DEBUG_LOG_ERROR("m_lowLevelIO.Init() returned AKRESULT {}", res );
        return;
    }

    //
    // Create the Sound Engine
    // Using default initialization parameters
    //
    AkInitSettings initSettings;
    AkPlatformInitSettings platformInitSettings;
    res = AK::SoundEngine::Init(&initSettings, &platformInitSettings);
    if ( res != AK_Success ){
        DEBUG_LOG_ERROR("AK::SoundEngine::Init() returned AKRESULT {}", res );
        return;
    }

    //
    // Initialize the music engine
    // Using default initialization parameters
    //
    AkMusicSettings musicInit;
    res = AK::MusicEngine::Init( &musicInit );
    if ( res != AK_Success ){
        DEBUG_LOG_ERROR("AK::MusicEngine::Init() returned AKRESULT {}", res );
        return;
    }

#if !defined AK_OPTIMIZED && !defined INTEGRATIONDEMO_DISABLECOMM
    //
    // Initialize communications (not in release build!)
    //
    AkCommSettings commSettings;
    AKPLATFORM::SafeStrCpy(commSettings.szAppNetworkName, "Integration Demo", AK_COMM_SETTINGS_MAX_STRING_SIZE);
    res = AK::Comm::Init(commSettings);
    if ( res != AK_Success ){
        DEBUG_LOG_ERROR("AK::Comm::Init() returned AKRESULT {}. Communication between the Wwise authoring application and the game will not be possible.", res );
        return;
    }
#endif // AK_OPTIMIZED

    AkSpatialAudioInitSettings settings;

    res = AK::SpatialAudio::Init(settings);
    if (res != AK_Success){
        DEBUG_LOG_ERROR("AK::SpatialAudio::Init() returned AKRESULT {}", res);
        return;
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
    lowLevelIoDeferred->SetBasePath(SOUND_BANK_PATH);

    // Set global language. Low-level I/O devices can use this string to find language-specific assets.
    if (AK::StreamMgr::SetCurrentLanguage(AKTEXT("English(US)")) != AK_Success){
        DEBUG_LOG_ERROR("AK::StreamMgr::SetCurrentLanguage() failed");
        return;
    }

    AK::SoundEngine::RegisterResourceMonitorCallback(ResourceMonitorDataCallback);
}

#define DATA_SUMMARY_REFRESH_COOLDOWN 7; // Refresh cooldown affecting the refresh rate of the resource monitor data summary
void ResourceMonitorDataCallback(const AkResourceMonitorDataSummary *in_pdataSummary) {
    static int ResourceMonitorUpdateCooldown = 0;
    if (ResourceMonitorUpdateCooldown <= 0){
        AkResourceMonitorDataSummary resourceDataSummary = *in_pdataSummary;
        DEBUG_LOG_INFO("Total CPU % : {} ", resourceDataSummary.totalCPU);
        DEBUG_LOG_INFO("Plugin CPU % : {} ", resourceDataSummary.pluginCPU);
        DEBUG_LOG_INFO("Virtual Voices : {} ", resourceDataSummary.virtualVoices);
        DEBUG_LOG_INFO("Physical Voices : {} ", resourceDataSummary.physicalVoices);
        DEBUG_LOG_INFO("Total Voices : {} ", resourceDataSummary.totalVoices);
        DEBUG_LOG_INFO("Active events : {} ", resourceDataSummary.nbActiveEvents);
        ResourceMonitorUpdateCooldown = DATA_SUMMARY_REFRESH_COOLDOWN;
    }else{
        ResourceMonitorUpdateCooldown--;
    }
}

void WwiseAudio::Update() {
    if (AK::SoundEngine::IsInitialized())
        AK::SoundEngine::RenderAudio();
}