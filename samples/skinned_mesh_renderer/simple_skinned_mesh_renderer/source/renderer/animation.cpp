//
// Created by captain on 2021/11/21.
//

#include "animation.h"
#include "animation_clip.h"
#include "utils/debug.h"

void Animation::LoadAnimationClipFromFile(const char *path, const char *alias_name) {
    AnimationClip* animationClip = new AnimationClip();
    animationClip->LoadFromFile(path);
    animation_clips_map_[alias_name] = animationClip;
}

void Animation::Play(const char *alias_name) {
    if (animation_clips_map_.find(alias_name) == animation_clips_map_.end()) {
        DEBUG_LOG_ERROR("AnimationClip not found: {}", alias_name);
        return;
    }
    animation_clips_map_[alias_name]->Play();
}
