#include <glad/gl.h>

#define GLFW_INCLUDE_NONE

#include <GLFW/glfw3.h>
#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/sinks/basic_file_sink.h"
#include <stdio.h>
#include <iostream>


#define _CRT_SECURE_NO_WARNINGS

#include <tchar.h>
#include <filesystem>
#include <fstream>
#include <chrono>

#include "codec_def.h"
#include "codec_app_def.h"
#include "codec_api.h"
#include "measure_time.h"

using namespace std;

/// 将YUV三个通道的数据写入到文件中
/// \param ofs
/// \param pData
/// \param iStride
/// \param iWidth
/// \param iHeight
void Write2File (std::ofstream& ofs, unsigned char* pData[3], int iStride[2], int iWidth, int iHeight) {
    int i;
    unsigned char* pPtr = NULL;

    pPtr = pData[0];
    for (i = 0; i < iHeight; i++) {
        ofs.write(reinterpret_cast<char*>(pPtr), iWidth);
        pPtr += iStride[0];
    }

    iHeight = iHeight / 2;
    iWidth = iWidth / 2;
    pPtr = pData[1];
    for (i = 0; i < iHeight; i++) {
        ofs.write(reinterpret_cast<char*>(pPtr), iWidth);
        pPtr += iStride[1];
    }

    pPtr = pData[2];
    for (i = 0; i < iHeight; i++) {
        ofs.write(reinterpret_cast<char*>(pPtr), iWidth);
        pPtr += iStride[1];
    }
}

/// 保存一帧的YUV数据为图片
/// \param pDst
/// \param pInfo
/// \param FrameIndex
void SaveOneFrame(void* pDst[3], SBufferInfo* pInfo, int FrameIndex)
{
    // 检查并创建目录Frames，并以f_加上FrameIndex为文件名创建文件，调用Process函数，保存每一帧为图片
    // 检查Frames目录是否存在
    if (!std::filesystem::exists("Frames")) {
        // 创建目录Frames
        std::filesystem::create_directory("Frames");
    }

    // 以f_加上FrameIndex为文件名创建文件
    std::string filename = "Frames/f_" + std::to_string(FrameIndex);
    std::ofstream file(filename, std::ios::binary);

    // 调用Process函数，保存每一帧为图片
    if (file.is_open()) {

        if (pDst[0] && pDst[1] && pDst[2] && pInfo) {
            int iStride[2];
            int iWidth = pInfo->UsrData.sSystemBuffer.iWidth;
            int iHeight = pInfo->UsrData.sSystemBuffer.iHeight;
            iStride[0] = pInfo->UsrData.sSystemBuffer.iStride[0];
            iStride[1] = pInfo->UsrData.sSystemBuffer.iStride[1];

            Write2File (file, (unsigned char**)pDst, iStride, iWidth, iHeight);
        }

        file.close();
    }
}


void H264DecodeInstance(ISVCDecoder *pDecoder, string kpH264FileName) {
    // 初始化各种参数和缓冲区
    unsigned long long uiTimeStamp = 0;

    int32_t iFrameCount = 0;

    std::ifstream file(kpH264FileName, std::ios::binary);

    std::vector<char> packet;
    int nal_deliminator = 0;
    char byte;
    auto last_read_time = std::chrono::steady_clock::now();
    while (true) {
        if (file.read(&byte, 1)) {
            // 更新最后一次读取数据的时间
            last_read_time = std::chrono::steady_clock::now();

            packet.push_back(byte);

            // 检查是否找到了数据包的起始码
            if (byte == 1 && nal_deliminator == 3) {
                // 如果找到了起始码，说明前一帧数据收集完毕。对的，因为在H264不能获取到一帧的数据大小，所以只能用下一帧起始码来判定上一帧已经收集完整。
                // 现在可以将上一帧的数据拿去解码
                // 上一帧的数据就是packet往前4个字节

                SBufferInfo sDstBufInfo;
                memset(&sDstBufInfo, 0, sizeof(SBufferInfo));
                sDstBufInfo.uiInBsTimeStamp = ++uiTimeStamp;

                // 调用解码器的解码函数
                uint8_t *pData[3] = {nullptr};
                int last_frame_data_size=packet.size()-4;
                pDecoder->DecodeFrameNoDelay(reinterpret_cast<const unsigned char *>(packet.data()), last_frame_data_size, pData, &sDstBufInfo);

                // 如果解码成功，处理解码后的数据
                if (sDstBufInfo.iBufferStatus == 1) {
                    SaveOneFrame((void **) sDstBufInfo.pDst, &sDstBufInfo, iFrameCount);

                    // 增加帧数
                    ++iFrameCount;
                }

                // 清空上一帧的数据
                packet.erase(packet.begin(), packet.end() - 4);
                nal_deliminator = 0;
            } else {
                if (byte == 0) {
                    ++nal_deliminator;
                } else {
                    nal_deliminator = 0;
                }
            }
        } else {
            // 如果无法读取更多数据，暂停一段时间，然后再次尝试读取
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            file.clear();  // 清除文件流的错误标志

            // 检查是否已经超过10秒没有读取到数据
            auto now = std::chrono::steady_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::seconds>(now - last_read_time);
            if (duration.count() >= 10) {
                break;
            }
        }
    }

    int32_t iEndOfStreamFlag = 0;
    pDecoder->SetOption(DECODER_OPTION_END_OF_STREAM, (void *) &iEndOfStreamFlag);

    file.close();
}


int32_t main(int32_t iArgC, char *pArgV[]) {
//    InitSpdLog();

    //usage 2: h264dec.exe test.264 test.yuv
    string strInputFile = "../data/video/ue4_1312x728.h264";

    // 创建解码器实例
    ISVCDecoder *pDecoder = nullptr;

    SDecodingParam sDecParam = {0};

    sDecParam.sVideoProperty.size = sizeof(sDecParam.sVideoProperty);
    sDecParam.eEcActiveIdc = ERROR_CON_SLICE_MV_COPY_CROSS_IDR_FREEZE_RES_CHANGE;


    sDecParam.uiTargetDqLayer = (uint8_t) -1;
    sDecParam.eEcActiveIdc = ERROR_CON_SLICE_COPY;
    sDecParam.sVideoProperty.eVideoBsType = VIDEO_BITSTREAM_DEFAULT;

    if (WelsCreateDecoder(&pDecoder) || (NULL == pDecoder)) {
        printf("Create Decoder failed.\n");
        return 1;
    }

    int iLevelSetting = (int) WELS_LOG_WARNING;
    pDecoder->SetOption(DECODER_OPTION_TRACE_LEVEL, &iLevelSetting);

    int32_t iThreadCount = 0;
    pDecoder->SetOption(DECODER_OPTION_NUM_OF_THREADS, &iThreadCount);

    if (pDecoder->Initialize(&sDecParam)) {
        printf("Decoder initialization failed.\n");
        return 1;
    }


    // 设置解码器的错误掩盖方法
    int32_t iErrorConMethod=sDecParam.eEcActiveIdc;
    pDecoder->SetOption(DECODER_OPTION_ERROR_CON_IDC, &iErrorConMethod);

    H264DecodeInstance(pDecoder, strInputFile);

    if (sDecParam.pFileNameRestructed != NULL) {
        delete[]sDecParam.pFileNameRestructed;
        sDecParam.pFileNameRestructed = NULL;
    }

    if (pDecoder) {
        pDecoder->Uninitialize();

        WelsDestroyDecoder(pDecoder);
    }

    return 0;
}
