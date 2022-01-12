//
// Created by captainchen on 2021/12/27.
//

#include "wwise_audio.h"
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
#ifdef WIN32
#include "SoundEngine/Win32/AkFilePackageLowLevelIODeferred.h"
#include "SoundEngine/Win32/Platform.h"
#define RELATIVE_PATH L"..\\data\\audio\\"
#endif
#include "utils/debug.h"
#include "utils/time.h"

AkGameObjectID WwiseAudio::audio_object_id_next_=10000;

void ResourceMonitorDataCallback(const AkResourceMonitorDataSummary * in_pdataSummary);

void WwiseAudio::Init() {
    AkMemSettings memSettings;
    AkStreamMgrSettings streamMgrSettings;
    AkDeviceSettings deviceSettings;
    AkInitSettings initSettings;
    AkPlatformInitSettings platformInitSettings;
    AkMusicSettings musicInit;
    AkCommSettings commSettings;

    AK::MemoryMgr::GetDefaultSettings(memSettings);
    AK::StreamMgr::GetDefaultSettings(streamMgrSettings);
    AK::StreamMgr::GetDefaultDeviceSettings(deviceSettings);
#if defined(INTDEMO_DEFERRED_IO)
    deviceSettings.uSchedulerTypeFlags = AK_SCHEDULER_DEFERRED_LINED_UP;
#endif

    AK::SoundEngine::GetDefaultInitSettings(initSettings);
#if defined( INTEGRATIONDEMO_ASSERT_HOOK )
    m_initSettings.pfnAssertHook = INTEGRATIONDEMO_ASSERT_HOOK;
#endif // defined( INTEGRATIONDEMO_ASSERT_HOOK )

    AK::SoundEngine::GetDefaultPlatformInitSettings(platformInitSettings);
    AK::MusicEngine::GetDefaultInitSettings(musicInit);

#if !defined AK_OPTIMIZED && !defined INTEGRATIONDEMO_DISABLECOMM
    AK::Comm::GetDefaultInitSettings(commSettings);
#endif

    // 创建并初始化默认的内存管理器。注意，你可以使用自己的内存管理器覆盖默认的内存管理器。详细信息请参考SDK文档。
    AKRESULT res = AK::MemoryMgr::Init(&memSettings);
    if ( res != AK_Success ){
        DEBUG_LOG_ERROR("WwiseAudio::Init() AK::MemoryMgr::Init() failed,res:{}", res);
        return;
    }

    // 创建并初始化默认的流管理器。注意，你可以使用自己的流管理器覆盖默认的流管理器。详细信息请参考SDK文档。
    if (!AK::StreamMgr::Create( streamMgrSettings)){
        DEBUG_LOG_ERROR("WwiseAudio::Init() AK::StreamMgr::Create() failed");
        return;
    }

    // 创建一个流设备，并使用阻塞的低级I/O握手。注意，你可以使用自己的低级I/O模块覆盖默认的低级I/O模块。详细信息请参考SDK文档。
    // CAkFilePackageLowLevelIOBlocking::Init()创建了一个流设备，并将自己注册为文件位置解析器。
    deviceSettings.bUseStreamCache = true;
    CAkFilePackageLowLevelIODeferred* lowLevelIoDeferred= new CAkFilePackageLowLevelIODeferred();
    res = lowLevelIoDeferred->Init( deviceSettings );
    if ( res != AK_Success ){
        DEBUG_LOG_ERROR("m_lowLevelIO.Init() returned AKRESULT {}", res );
        return;
    }

    // 创建声音引擎，使用默认的初始化参数
    res = AK::SoundEngine::Init(&initSettings, &platformInitSettings);
    if ( res != AK_Success ){
        DEBUG_LOG_ERROR("AK::SoundEngine::Init() returned AKRESULT {}", res );
        return;
    }

    // 初始化音乐引擎，使用默认的初始化参数
    res = AK::MusicEngine::Init( &musicInit );
    if ( res != AK_Success ){
        DEBUG_LOG_ERROR("AK::MusicEngine::Init() returned AKRESULT {}", res );
        return;
    }

#if !defined AK_OPTIMIZED && !defined INTEGRATIONDEMO_DISABLECOMM
    // 初始化通信（非发布版本！）
    AKPLATFORM::SafeStrCpy(commSettings.szAppNetworkName, "Integration Demo", AK_COMM_SETTINGS_MAX_STRING_SIZE);
    res = AK::Comm::Init(commSettings);
    if ( res != AK_Success ){
        DEBUG_LOG_ERROR("AK::Comm::Init() returned AKRESULT {}. Communication between the Wwise authoring application and the game will not be possible.", res );
        return;
    }
#endif // AK_OPTIMIZED

    // 初始化音频空间
    AkSpatialAudioInitSettings settings;
    res = AK::SpatialAudio::Init(settings);
    if (res != AK_Success){
        DEBUG_LOG_ERROR("AK::SpatialAudio::Init() returned AKRESULT {}", res);
        return;
    }

    // 对于具有只读bank路径的平台，将可写文件夹添加到基本路径列表中。 当打开文件写入失败时，IO 将回退到此路径。
#if defined(WRITABLE_PATH)
    m_pLowLevelIO->AddBasePath(WRITABLE_PATH);
#endif

    // 最后设置 SoundBank 文件的路径。最后一个基本路径总是第一个查询文件。
    lowLevelIoDeferred->SetBasePath(GetSoundBanksPath(RELATIVE_PATH));

    // 设置全局语言。 低级 I/O 设备可以使用此字符串来查找特定于语言的资产。
    if (AK::StreamMgr::SetCurrentLanguage(AKTEXT("English(US)")) != AK_Success){
        DEBUG_LOG_ERROR("AK::StreamMgr::SetCurrentLanguage() failed");
        return;
    }

    AK::SoundEngine::RegisterResourceMonitorCallback(ResourceMonitorDataCallback);
}

#define DATA_SUMMARY_REFRESH_COOLDOWN 7.0f; // 刷新冷却时间影响资源监视器数据汇总的刷新率
void ResourceMonitorDataCallback(const AkResourceMonitorDataSummary *in_pdataSummary) {
    static float PrintDataSummaryTimeStamp=0;
    if (PrintDataSummaryTimeStamp > Time::TimeSinceStartup()){
        return;
    }
    PrintDataSummaryTimeStamp=Time::TimeSinceStartup()+DATA_SUMMARY_REFRESH_COOLDOWN;

#ifdef PRINT_DATA_SUMMARY
    AkResourceMonitorDataSummary resourceDataSummary = *in_pdataSummary;
    DEBUG_LOG_INFO("------- RESOURCE DATA SUMMARY -------");
    DEBUG_LOG_INFO("Total CPU % : {} ", resourceDataSummary.totalCPU);
    DEBUG_LOG_INFO("Plugin CPU % : {} ", resourceDataSummary.pluginCPU);
    DEBUG_LOG_INFO("Virtual Voices : {} ", resourceDataSummary.virtualVoices);
    DEBUG_LOG_INFO("Physical Voices : {} ", resourceDataSummary.physicalVoices);
    DEBUG_LOG_INFO("Total Voices : {} ", resourceDataSummary.totalVoices);
    DEBUG_LOG_INFO("Active events : {} ", resourceDataSummary.nbActiveEvents);
    DEBUG_LOG_INFO("-------------------------------------");
#endif
}

void WwiseAudio::Update() {
    if (!AK::SoundEngine::IsInitialized()){
        return;
    }
    AKRESULT result = AK::SoundEngine::RenderAudio();
    if (result != AK_Success){
        DEBUG_LOG_ERROR("AK::SoundEngine::RenderAudio() failed.result:{}",result);
        return;
    }
}

void WwiseAudio::LoadBank(const char *bank_name) {
    AkBankID bank_id;
    AKRESULT result=AK::SoundEngine::LoadBank(bank_name, bank_id);
    if ( result!= AK_Success){
        DEBUG_LOG_ERROR("Failed to load bank {},result:{}", bank_name,result);
        return;
    }
}

AkGameObjectID WwiseAudio::GeneratorGameObjectID() {
    return audio_object_id_next_++;
}

void WwiseAudio::CreateAudioObject(AkGameObjectID audio_object_id,const char *audio_object_name){
    AKRESULT result=AK::SoundEngine::RegisterGameObj(audio_object_id, audio_object_name);
    if ( result!= AK_Success){
        DEBUG_LOG_ERROR("Failed to register game object {},result:{}", audio_object_name,result);
        return;
    }
}

void WwiseAudio::SetDefaultListeners(const AkGameObjectID& game_object_id) {
    AKRESULT result=AK::SoundEngine::SetDefaultListeners(&game_object_id, 1);;
    if ( result!= AK_Success){
        DEBUG_LOG_ERROR("Failed to set default listeners,result:{}", result);
        return;
    }
}

void WwiseAudio::SetPosition(AkGameObjectID game_object_id, glm::vec3 position, glm::vec3 front, glm::vec3 up) {
    AkSoundPosition soundPos;
    AkVector akPosition={position.x,position.y,position.z};
    AkVector akFront={front.x,front.y,front.z};
    AkVector akUp={up.x,up.y,up.z};
    soundPos.Set(akPosition,akFront,akUp);

    AK::SoundEngine::SetPosition(game_object_id, soundPos);
}

AkPlayingID WwiseAudio::PostEvent(const char *event_name, AkGameObjectID audio_object_id, AkUInt32 flags,
                           AkCallbackFunc callback, void *user_data) {
    AkPlayingID playing_id = AK::SoundEngine::PostEvent(event_name,audio_object_id,flags,callback,user_data);
    if(playing_id==AK_INVALID_PLAYING_ID){
        DEBUG_LOG_ERROR("AudioSource::Play() failed");
    }
    return playing_id;
}

void WwiseAudio::SetRTPCValue(const char *realtime_parameter_control_name, AkRtpcValue value,
                              AkGameObjectID audio_object_id) {
    AKRESULT result = AK::SoundEngine::SetRTPCValue(realtime_parameter_control_name, value, audio_object_id);
    if (result != AK_Success) {
        DEBUG_LOG_ERROR("Set RealTime Parameter Control value failed,parameter name:{} ,value:{}", realtime_parameter_control_name, value);
    }
}

void WwiseAudio::StopEvent(AkPlayingID playing_id) {
    AK::SoundEngine::ExecuteActionOnPlayingID(AK::SoundEngine::AkActionOnEventType::AkActionOnEventType_Stop,playing_id);
}