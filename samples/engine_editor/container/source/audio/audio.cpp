//
// Created by captainchen on 2021/8/23.
//

#include "audio.h"

#ifdef USE_FMOD_STUDIO
#include "studio/audio_studio.h"
#define AudioSolution AudioStudio
#else
#include "core/audio_core.h"
#define AudioSolution AudioCore
#endif

void Audio::Init() {
    AudioSolution::Init();
}

void Audio::Update() {
    AudioSolution::Update();
}
