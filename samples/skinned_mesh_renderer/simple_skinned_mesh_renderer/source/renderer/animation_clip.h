//
// Created by captain on 2021/11/21.
//

#ifndef UNTITLED_ANIMATION_CLIP_H
#define UNTITLED_ANIMATION_CLIP_H
#include <vector>
#include <string>
#include <unordered_map>
#include <glm/glm.hpp>

/// 骨骼动画片段
class AnimationClip {
public:
    AnimationClip();
    ~AnimationClip();

    /// 加载动画片段
    /// \param file_path
    void LoadFromFile(const char *file_path);

    /// 获取持续时间
    float duration(){
        return this->duration_;
    }

    /// 获取当前帧最新的骨骼矩阵
    std::vector<glm::mat4> GetCurrentFrameBoneMatrix();

public:
    /// 播放骨骼动画
    void Play();

    /// 刷帧，计算当前时间所处的帧
    void Update();

    /// 停止播放
    void Stop();

private:
    /// 预计算骨骼矩阵
    void Bake();

    /// 递归计算骨骼矩阵,从根节点开始。Blender导出的时候要确保先导出父节点。
    /// \param bone_name
    /// \param parent_matrix
    /// \param bone_matrix
    void CalculateBoneMatrix(std::vector<glm::mat4>& current_frame_bone_matrices,unsigned short bone_index, const glm::mat4 &parent_matrix);

private:
    /// 所有骨骼名字
    std::vector<std::string> bone_names_;
    /// 骨骼的子节点
    std::vector<std::vector<unsigned short>> bone_children_vector_;
    /// 骨骼T-pose
    std::vector<glm::mat4> bone_t_pose_vector_;
    /// 持续时间
    float duration_=0.0f;
    /// 总帧数
    unsigned short frame_count_=0;
    /// 每一帧每一个骨骼的位移矩阵
    std::vector<std::vector<glm::mat4>> bone_matrix_frames_vector_;
    /// 骨骼动画开始播放时间
    float start_time_=0.0f;
    /// 骨骼动画是否在播放
    bool is_playing_=false;
    /// 当前帧
    unsigned short current_frame_=0;
};


#endif //UNTITLED_ANIMATION_CLIP_H
