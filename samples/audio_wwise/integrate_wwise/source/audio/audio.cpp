//
// Created by captainchen on 2021/8/23.
//

#include "audio.h"

#ifdef USE_FMOD_STUDIO
    #include "studio/audio_studio.h"
    #define AudioSolution AudioStudio
#elif defined(USE_FMOD_CORE)
    #include "core/audio_core.h"
    #define AudioSolution AudioCore
#elif defined(USE_WWISE_AUDIO)
    #include "wwise/wwise_audio.h"
    #define AudioSolution WwiseAudio
#endif

void Audio::Init() {
    AudioSolution::Init();
}

void Audio::Update() {
    AudioSolution::Update();
}
