//
// Created by captain on 2021/11/21.
//

#include "animation_clip.h"
#include <fstream>
#include <glm/ext.hpp>
#include <glm/gtx/string_cast_beauty.hpp>
#include "app/application.h"
#include "utils/debug.h"
#include "utils/time.h"

using std::ifstream;
using std::ios;

#define SKELETON_ANIMATION_HEAD "skeleton_anim"
#define SKELETON_ANIMATION_FRAME_RATE 24

AnimationClip::AnimationClip() {

}

AnimationClip::~AnimationClip() {

}

void AnimationClip::LoadFromFile(const char *file_path) {
    //读取文件头
    ifstream input_file_stream(Application::data_path()+file_path,ios::in | ios::binary);
    if(!input_file_stream.is_open()) {
        DEBUG_LOG_ERROR("AnimationClip::LoadFromFile: open file failed,file_path:{}",file_path);
        return;
    }
    char file_head[14];
    input_file_stream.read(file_head,13);
    file_head[13]='\0';
    if(strcmp(file_head,SKELETON_ANIMATION_HEAD) != 0) {
        DEBUG_LOG_ERROR("AnimationClip::LoadFromFile: file head error,file_head:{},the right is:{}",file_head,SKELETON_ANIMATION_HEAD);
        return;
    }
    //读取名字长度
    unsigned short name_length;
    input_file_stream.read(reinterpret_cast<char *>(&name_length),sizeof(name_length));
    //读取名字
    char *name_buffer = new char[name_length+1];
    input_file_stream.read(name_buffer,name_length);
    name_buffer[name_length]='\0';
    name_ = name_buffer;
    delete[] name_buffer;
    //读取帧数
    input_file_stream.read(reinterpret_cast<char *>(&frame_count_),sizeof(frame_count_));
    //读取帧率
    input_file_stream.read(reinterpret_cast<char *>(&frame_per_second_),sizeof(frame_per_second_));
    //读取骨骼数量
    unsigned short bone_count=0;
    input_file_stream.read(reinterpret_cast<char *>(&bone_count), sizeof(unsigned short));
    //读取骨骼名字数组
    for(unsigned short i=0;i<bone_count;i++) {
        //读取骨骼名字长度
        unsigned short bone_name_size=0;
        input_file_stream.read(reinterpret_cast<char *>(&bone_name_size), sizeof(unsigned short));

        char* bone_name=new char[bone_name_size+1];
        input_file_stream.read(bone_name,bone_name_size);
        bone_name[bone_name_size]='\0';
        bone_names_.push_back(bone_name);
        delete[] bone_name;
    }
    //读取骨骼动画
    for (int frame_index = 0; frame_index < frame_count_; frame_index++) {
        //读取一帧的骨骼矩阵
        std::vector<glm::mat4> bone_matrices;
        for (unsigned short bone_index = 0; bone_index < bone_count; bone_index++) {
            glm::mat4 bone_matrix;
            input_file_stream.read(reinterpret_cast<char *>(&bone_matrix), sizeof(float) * 16);
            bone_matrices.push_back(bone_matrix);
        }
        bone_matrix_frames_vector_.push_back(bone_matrices);
    }
    input_file_stream.close();
}

void AnimationClip::Play() {
    //记录当前时间
    start_time_=Time::TimeSinceStartup();
    is_playing_=true;
}

void AnimationClip::Update() {
    if (is_playing_== false){
        DEBUG_LOG_ERROR("AnimationClip is not playing");
        return;
    }
    //计算当前时间对应的帧序号
    float current_time=Time::TimeSinceStartup()-start_time_;
//    DEBUG_LOG_INFO("current_time:{}",current_time);
    unsigned short current_frame_index=static_cast<unsigned short>(current_time*SKELETON_ANIMATION_FRAME_RATE);
    current_frame_=current_frame_index%frame_count_;
//    current_frame_=19;
}

std::vector<glm::mat4>& AnimationClip::GetCurrentFrameBoneMatrix(){
    if (is_playing_== false){
        DEBUG_LOG_ERROR("AnimationClip is not playing");
    }
    return bone_matrix_frames_vector_[current_frame_];
//    return bone_matrix_frames_vector_[19];
}




void AnimationClip::Stop() {
    is_playing_=false;
}
