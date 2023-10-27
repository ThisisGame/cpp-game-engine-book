﻿//
// Created by captain on 2021/4/6.
// 图片加载与解析
//
#include "texture2d.h"

#include "stb/stb_image.h"
#include <vector>
#include <fstream>

#include <codec_api.h>
#include <cassert>
#include <cstring>
#include <iostream>

#include "timetool/stopwatch.h"
#include "spdlog/spdlog.h"

using timetool::StopWatch;

Texture2D* Texture2D::LoadFromFile(std::string& image_file_path) {
    StopWatch stopwatch;
    stopwatch.start();

    Texture2D* texture2d=new Texture2D();

    int channels_in_file;//通道数
    unsigned char* data = stbi_load(image_file_path.c_str(), &(texture2d->width_), &(texture2d->height_), &channels_in_file, 0);
    if (data!= nullptr) {
        //根据颜色通道数，判断颜色格式。
        switch (channels_in_file) {
            case 1:{
                texture2d->gl_texture_format_ = GL_ALPHA;
                break;
            }
            case 3:{
                texture2d->gl_texture_format_ = GL_RGB;
                break;
            }
            case 4:{
                texture2d->gl_texture_format_ = GL_RGBA;
                break;
            }
        }
    }

    stopwatch.stop();
    spdlog::info("stbi_load cost:{}",stopwatch.milliseconds());

    stopwatch.restart();

    // 将YUV444转YUV420
    std::vector<unsigned char> imageYuv(texture2d->width_ * texture2d->height_ * 3 / 2);

    int pixel_num=texture2d->width_ * texture2d->height_;

    stopwatch.restart();

    unsigned char* yuv444_data = data;

    //YUV是单通道连续存放，Y是全尺寸，UV是1/4尺寸。
    //Y通道的起点就是0
    //U通道的起点就是Y通道的终点，然后U通道是1/4的Y尺寸
    //V通道的起点就是U通道终点
    int y_index=0;
    int u_index=pixel_num;
    int v_index=pixel_num*1.25;
    for (int j = 0; j < texture2d->height_; ++j) {
        for (int i = 0; i < texture2d->width_; ++i) {
            // Y通道是起点位置，连续存储。
            imageYuv[y_index++] = *(yuv444_data++);

            // U和V通道，进行下采样
            if (j % 2 == 0 && i % 2 == 0) {
                imageYuv[u_index++] = *(yuv444_data++);
                imageYuv[v_index++] = *(yuv444_data++);
            }
            else
            {
                yuv444_data+=2;
            }
        }
    }

    stopwatch.stop();
    spdlog::info("yuv444 to yuv420 cost:{}",stopwatch.microseconds());

    // 输出H264编码文件
    std::ofstream outFi;
    outFi.open ("test.264", std::ios::out | std::ios::binary);

    // 创建H264编码器实例
    ISVCEncoder *encoder_ = nullptr;
    int rv = WelsCreateSVCEncoder (&encoder_);
    assert (0==rv);
    assert (encoder_ != nullptr);

    int frame_width = texture2d->width_;
    int frame_height = texture2d->height_;
    int frame_num = 100;

    // 设置编码器参数
    SEncParamBase param;
    memset (&param, 0, sizeof (SEncParamBase));
    param.iUsageType = CAMERA_VIDEO_REAL_TIME;  // 使用类型为实时摄像头视频
    param.fMaxFrameRate = 30;  // 帧率为30
    param.iPicWidth = frame_width;  // 图片宽度
    param.iPicHeight = frame_height;  // 图片高度
    param.iTargetBitrate = 5000000;  // 目标比特率为5000000
    encoder_->Initialize (&param);  // 初始化编码器

    //SFrameBSInfo 是OpenH264编码器用来存储编码后的比特流信息的结构体。全称应该是SFrameBitStreamInfo
    SFrameBSInfo info;
    memset (&info, 0, sizeof (SFrameBSInfo));

    //SSourcePicture 是OpenH264编码器用来存储待编码的图片信息的结构体。
    SSourcePicture pic;  // 创建一个SSourcePicture结构体实例

    for(int frame_index = 0; frame_index < frame_num; frame_index++){
        stopwatch.restart();

        // 将YUV444格式的图片数据传递给编码器进行编码，openh264编码器只支持YUV420格式的图片数据，所以需要将YUV444格式的图片数据转换为YUV420格式的图片数据。

        //在录屏的时候想降低录制的帧率，跳过一些帧，那么方法是使用重复帧而不是空帧。
        //比如说游戏第10帧，第20帧，中间10帧要跳过，那么就一直使用第10帧的数据，而不是使用空帧。
        if(frame_index % 2 == 0){
            memset (&pic, 0, sizeof (SSourcePicture));  // 使用memset函数将其初始化为0
            pic.iPicWidth = frame_width;  // 设置图片的宽度
            pic.iPicHeight = frame_height;  // 设置图片的高度
            pic.iColorFormat = videoFormatI420;  // 设置图片的颜色格式为I420

            pic.iStride[0] = frame_width; // 设置Y通道的步长
            pic.iStride[1] = frame_width / 2; // 设置U通道的步长
            pic.iStride[2] = frame_width / 2; // 设置V通道的步长

            pic.pData[0] = imageYuv.data(); // Y通道的数据
            pic.pData[1] = pic.pData[0] + frame_width * frame_height; // U通道的数据
            pic.pData[2] = pic.pData[1] + frame_width * frame_height / 4; // V通道的数据
        }
        else{ //使用重复帧，而不是使用空帧，即直接使用上一帧的数据
        }

        //prepare input data
        rv = encoder_->EncodeFrame (&pic, &info);
        assert (rv == cmResultSuccess);
        if (info.eFrameType != videoFrameTypeSkip /*&& cbk != nullptr*/){
            //output bitstream
            for (int iLayer=0; iLayer < info.iLayerNum; iLayer++)
            {
                SLayerBSInfo* pLayerBsInfo = &info.sLayerInfo[iLayer];

                int iLayerSize = 0;
                int iNalIdx = pLayerBsInfo->iNalCount - 1;
                do {
                    iLayerSize += pLayerBsInfo->pNalLengthInByte[iNalIdx];
                    --iNalIdx;
                } while (iNalIdx >= 0);

                unsigned char *outBuf = pLayerBsInfo->pBsBuf;
                outFi.write((char *)outBuf, iLayerSize);
            }
        }

        stopwatch.stop();
        //spdlog::info("encode frame {} cost:{}",frame_index,stopwatch.microseconds());
    }

    if (encoder_) {
        encoder_->Uninitialize();
        WelsDestroySVCEncoder (encoder_);
    }

    outFi.close();

    //释放图片文件内存
    stbi_image_free(data);

    return texture2d;
}
