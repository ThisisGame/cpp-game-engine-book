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

public:
    /// 播放骨骼动画
    void Play();

    /// 更新骨骼动画
    void Update();

    /// 递归计算骨骼矩阵,从根节点开始。Blender导出的时候要确保先导出父节点。
    /// \param bone_name
    /// \param parent_matrix
    /// \param bone_matrix
    void CalculateBoneMatrix(std::vector<glm::mat4>& current_frame_bone_matrices,unsigned short bone_index, const glm::mat4 &parent_matrix);

    /// 停止播放
    void Stop();

private:
    /// 所有骨骼名字
    std::vector<std::string> bone_names_;
    /// 骨骼的子节点
    std::vector<std::vector<unsigned short>> bone_children_vector_;
    /// 骨骼T-pose
    std::vector<glm::mat4> bone_t_pose_vector_;
    /// 持续时间
    float duration_;
    /// 帧数
    unsigned short frame_count_;
    /// 骨骼动画
    std::vector<std::vector<glm::mat4>> bone_animation_vector_;
    /// 骨骼动画开始播放时间
    float start_time_;
};


#endif //UNTITLED_ANIMATION_CLIP_H
