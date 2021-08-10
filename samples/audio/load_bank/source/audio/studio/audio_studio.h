//
// Created by captainchen on 2021/8/10.
//

#ifndef UNTITLED_AUDIO_STUDIO_H
#define UNTITLED_AUDIO_STUDIO_H

#include "fmod/api/core/inc/fmod.h"
#include "fmod/api/studio/inc/fmod_studio.h"

class AudioStudio {
public:
    static FMOD_RESULT Create(FMOD_STUDIO_SYSTEM **system, unsigned int header_version = FMOD_VERSION);

    static FMOD_RESULT Initialize(int max_channels, FMOD_STUDIO_INITFLAGS fmod_studio_init_flags, FMOD_INITFLAGS fmod_init_flags, void *extra_driver_data);

    /// 刷帧
    /// \return
    static FMOD_RESULT Update();

    /// 加载 bank 文件
    /// \param file_name
    /// \param fmod_studio_load_bank_flags
    /// \param bank
    /// \return
    static FMOD_RESULT LoadBankFile(const char *file_name, FMOD_STUDIO_LOAD_BANK_FLAGS fmod_studio_load_bank_flags, FMOD_STUDIO_BANK **bank);

    /// 创建AudioStudioEvent
    /// \param path
    /// \return
    static FMOD_RESULT CreateEvent(const char *path);
private:
    static FMOD_STUDIO_SYSTEM* system_;
};


#endif //UNTITLED_AUDIO_STUDIO_H
