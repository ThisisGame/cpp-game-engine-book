#include <glad/gl.h>

#define GLFW_INCLUDE_NONE

#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtx/transform2.hpp>
#include<glm/gtx/euler_angles.hpp>
#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/sinks/basic_file_sink.h"
#include <stdlib.h>
#include <stdio.h>
#include <iostream>

void InitSpdLog() {
    try {
        auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
        console_sink->set_level(spdlog::level::trace);
        console_sink->set_pattern("[multi_sink_example] [%^%l%$] %v");

        auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>("logs/multisink.txt", true);
        file_sink->set_level(spdlog::level::trace);

        spdlog::sinks_init_list sink_list = {file_sink, console_sink};

        // you can even set multi_sink logger as default logger
        spdlog::set_default_logger(
                std::make_shared<spdlog::logger>("multi_sink", spdlog::sinks_init_list({console_sink, file_sink})));
    }
    catch (const spdlog::spdlog_ex &ex) {
        std::cout << "Log initialization failed: " << ex.what() << std::endl;
    }
}

#define _CRT_SECURE_NO_WARNINGS

#include <windows.h>
#include <tchar.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include "codec_def.h"
#include "codec_app_def.h"
#include "codec_api.h"
#include "measure_time.h"

using namespace std;


void Write2File (FILE* pFp, unsigned char* pData[3], int iStride[2], int iWidth, int iHeight) {
    int   i;
    unsigned char*  pPtr = NULL;

    pPtr = pData[0];
    for (i = 0; i < iHeight; i++) {
        fwrite (pPtr, 1, iWidth, pFp);
        pPtr += iStride[0];
    }

    iHeight = iHeight / 2;
    iWidth = iWidth / 2;
    pPtr = pData[1];
    for (i = 0; i < iHeight; i++) {
        fwrite (pPtr, 1, iWidth, pFp);
        pPtr += iStride[1];
    }

    pPtr = pData[2];
    for (i = 0; i < iHeight; i++) {
        fwrite (pPtr, 1, iWidth, pFp);
        pPtr += iStride[1];
    }
}

int Process (void* pDst[3], SBufferInfo* pInfo, FILE* pFp) {
    if (pFp && pDst[0] && pDst[1] && pDst[2] && pInfo) {
        int iStride[2];
        int iWidth = pInfo->UsrData.sSystemBuffer.iWidth;
        int iHeight = pInfo->UsrData.sSystemBuffer.iHeight;
        iStride[0] = pInfo->UsrData.sSystemBuffer.iStride[0];
        iStride[1] = pInfo->UsrData.sSystemBuffer.iStride[1];

        Write2File (pFp, (unsigned char**)pDst, iStride, iWidth, iHeight);
    }

    return 0;
}

/**
 * @brief 从给定的缓冲区中读取一个位，并更新当前位的位置。
 * @param pBufPtr 指向缓冲区的指针
 * @param curBit 当前位的位置
 * @return 返回当前位的值
 */
int32_t readBit(uint8_t *pBufPtr, int32_t &curBit) {
    int nIndex = curBit / 8;  // 计算当前位在字节中的索引
    int nOffset = curBit % 8 + 1;  // 计算当前位在字节中的偏移量
    curBit++;  // 更新当前位
    return (pBufPtr[nIndex] >> (8 - nOffset)) & 0x01;  // 返回当前位的值
}

/**
 * @brief 从给定的缓冲区中读取n个位，并将它们合并为一个整数。
 * @param pBufPtr 指向缓冲区的指针
 * @param n 要读取的位数
 * @param curBit 当前位的位置
 * @return 返回合并后的整数
 */
int32_t readBits(uint8_t *pBufPtr, int32_t &n, int32_t &curBit) {
    int r = 0;
    for (int i = 0; i < n; i++) {
        r |= (readBit(pBufPtr, curBit) << (n - i - 1));  // 读取n个位，并将它们合并为一个整数
    }
    return r;
}

/**
 * @brief 从给定的缓冲区中读取一个无符号指数哥伦布编码。
 * @param pBufPtr 指向缓冲区的指针
 * @param curBit 当前位的位置
 * @return 返回无符号指数哥伦布编码
 */
int32_t bsGetUe(uint8_t *pBufPtr, int32_t &curBit) {
    int r = 0;
    int i = 0;
    while ((readBit(pBufPtr, curBit) == 0) && (i < 32)) {  // 读取连续的0，直到遇到1
        i++;
    }
    r = readBits(pBufPtr, i, curBit);  // 读取一个长度为i的无符号指数哥伦布编码
    r += (1 << i) - 1;
    return r;
}

/**
 * @brief 读取切片中的第一个宏块。
 * @param pSliceNalPtr 指向切片NAL的指针
 * @return 返回切片中的第一个宏块
 */
int32_t readFirstMbInSlice(uint8_t *pSliceNalPtr) {
    int32_t curBit = 0;
    int32_t firstMBInSlice = bsGetUe(pSliceNalPtr + 1, curBit);  // 读取切片中的第一个宏块
    return firstMBInSlice;
}

/**
 * @brief 从输入的H.264流中读取一张图片（或者说一帧）。
 * @param pBuf 指向输入H.264流的指针。
 * @param iFileSize 输入流的大小。
 * @param bufPos 当前在输入流中的位置。
 * @param pSpsBuf 用于存储序列参数集（SPS）的指针。
 * @param sps_byte_count SPS的字节大小。
 * @return 读取的字节数。如果可用的字节数小于4，返回可用的字节数。如果读取的字节数大于等于可用的字节数减4，返回可用的字节数。
 * @note 这个函数会从pBuf指向的位置开始，读取一个完整的编码帧，并将这个帧的大小返回。同时，如果这个帧包含一个新的SPS，那么这个函数还会更新pSpsBuf和sps_byte_count。
 */
int32_t readPicture(uint8_t *pBuf, const int32_t &iFileSize, const int32_t &bufPos, uint8_t *&pSpsBuf,
                    int32_t &sps_byte_count) {
    // 计算可用的字节数
    int32_t bytes_available = iFileSize - bufPos;
    // 如果可用的字节数小于4，直接返回
    if (bytes_available < 4) {
        return bytes_available;
    }
    // 初始化指针位置
    uint8_t *ptr = pBuf + bufPos;
    // 初始化读取的字节数
    int32_t read_bytes = 0;
    // 初始化SPS、PPS、非IDR图像、IDR图像和NAL分隔符的数量
    int32_t sps_count = 0;
    int32_t pps_count = 0;
    int32_t non_idr_pict_count = 0;
    int32_t idr_pict_count = 0;
    int32_t nal_deliminator = 0;
    // 初始化SPS缓冲区和SPS字节数
    pSpsBuf = nullptr;
    sps_byte_count = 0;
    // 遍历所有可用的字节
    while (read_bytes < bytes_available - 4) {
        // 检查是否存在4字节的起始码
        bool has4ByteStartCode = ptr[0] == 0 && ptr[1] == 0 && ptr[2] == 0 && ptr[3] == 1;
        // 如果不存在4字节的起始码，检查是否存在3字节的起始码
        bool has3ByteStartCode = false;
        if (!has4ByteStartCode) {
            has3ByteStartCode = ptr[0] == 0 && ptr[1] == 0 && ptr[2] == 1;
        }
        // 如果存在4字节或3字节的起始码
        if (has4ByteStartCode || has3ByteStartCode) {
            // 计算NAL单元类型
            int32_t byteOffset = has4ByteStartCode ? 4 : 3;
            uint8_t nal_unit_type = has4ByteStartCode ? (ptr[4] & 0x1F) : (ptr[3] & 0x1F);
            // 如果NAL单元类型为1（非IDR图像）
            if (nal_unit_type == 1) {
                // 读取第一个宏块在切片中的位置
                int32_t firstMBInSlice = readFirstMbInSlice(ptr + byteOffset);
                // 如果非IDR图像数量大于等于1，且IDR图像数量大于等于1，且第一个宏块在切片中的位置为0，返回读取的字节数
                if (++non_idr_pict_count >= 1 && idr_pict_count >= 1 && firstMBInSlice == 0) {
                    return read_bytes;
                }
            } else if (nal_unit_type == 5) {
                // 如果NAL单元类型为5（IDR图像）
                // 读取第一个宏块在切片中的位置
                int32_t firstMBInSlice = readFirstMbInSlice(ptr + byteOffset);
                // 如果IDR图像数量大于等于1，且非IDR图像数量大于等于1，且第一个宏块在切片中的位置为0，返回读取的字节数
                if (++idr_pict_count >= 1 && non_idr_pict_count >= 1 && firstMBInSlice == 0) {
                    return read_bytes;
                }
                // 如果IDR图像数量大于等于2，且第一个宏块在切片中的位置为0，返回读取的字节数
                if (idr_pict_count >= 2 && firstMBInSlice == 0) {
                    return read_bytes;
                }
            } else if (nal_unit_type == 7) {
                // 如果NAL单元类型为7（SPS）
                // 设置SPS缓冲区的位置
                pSpsBuf = ptr + (has4ByteStartCode ? 4 : 3);
                // 如果SPS数量大于等于1，且非IDR图像数量大于等于1或IDR图像数量大于等于1，返回读取的字节数
                if ((++sps_count >= 1) && (non_idr_pict_count >= 1 || idr_pict_count >= 1)) {
                    return read_bytes;
                }
                // 如果SPS数量等于2，返回读取的字节数
                if (sps_count == 2) {
                    return read_bytes;
                }
            } else if (nal_unit_type == 8) {
                // 如果NAL单元类型为8（PPS）
                // 如果PPS数量等于1且SPS数量等于1，设置SPS字节数
                if (++pps_count == 1 && sps_count == 1) {
                    sps_byte_count = int32_t(ptr - pSpsBuf);
                }
                // 如果PPS数量大于等于1，且非IDR图像数量大于等于1或IDR图像数量大于等于1，返回读取的字节数
                if (pps_count >= 1 && (non_idr_pict_count >= 1 || idr_pict_count >= 1)) {
                    return read_bytes;
                }
            } else if (nal_unit_type == 9) {
                // 如果NAL单元类型为9（NAL分隔符）
                // 如果NAL分隔符数量等于2，返回读取的字节数
                if (++nal_deliminator == 2) {
                    return read_bytes;
                }
            }
            // 如果读取的字节数大于等于可用的字节数减4，返回可用的字节数
            if (read_bytes >= bytes_available - 4) {
                return bytes_available;
            }
            // 读取下一个字节
            read_bytes += 4;
            ptr += 4;
        } else {
            // 如果不存在起始码，移动到下一个字节
            ++ptr;
            ++read_bytes;
        }
    }
    // 返回可用的字节数
    return bytes_available;
}

/**
 * @brief 清空解码器中的所有待处理帧，并处理所有已解码的帧。
 * @param pDecoder 指向解码器的指针。
 * @param iTotal 用于记录解码时间的变量。
 * @param pYuvFile 指向YUV文件的指针，用于存储解码后的帧。
 * @param pOptionFile 指向选项文件的指针，用于存储帧数、宽度和高度。
 * @param iFrameCount 用于记录帧数的变量。
 * @param uiTimeStamp 时间戳。
 * @param iWidth 用于记录宽度的变量。
 * @param iHeight 用于记录高度的变量。
 * @param iLastWidth 用于记录上一帧的宽度的变量。
 * @param iLastHeight 用于记录上一帧的高度的变量。
 * @note 这个函数首先获取解码器中剩余的帧数，然后遍历这些帧，对每一帧进行解码，并将解码后的帧写入到YUV文件中。如果选项文件存在，且帧的宽度或高度发生变化，那么这个函数还会将帧数、宽度和高度写入到选项文件中。
 */
void FlushFrames(ISVCDecoder *pDecoder, int64_t &iTotal, FILE *pYuvFile, FILE *pOptionFile, int32_t &iFrameCount,
                 unsigned long long &uiTimeStamp, int32_t &iWidth, int32_t &iHeight, int32_t &iLastWidth,
                 int32_t iLastHeight) {
    // 初始化数据和目标缓冲区
    uint8_t *pData[3] = {nullptr};
    uint8_t *pDst[3] = {nullptr};
    // 初始化缓冲区信息
    SBufferInfo sDstBufInfo;
    // 初始化缓冲区中的帧数
    int32_t num_of_frames_in_buffer = 0;
    // 获取缓冲区中剩余的帧数
    pDecoder->GetOption(DECODER_OPTION_NUM_OF_FRAMES_REMAINING_IN_BUFFER, &num_of_frames_in_buffer);
    // 遍历缓冲区中的所有帧
    for (int32_t i = 0; i < num_of_frames_in_buffer; ++i) {
        // 记录开始时间
        int64_t iStart = WelsTime();
        // 清空数据缓冲区和缓冲区信息
        pData[0] = nullptr;
        pData[1] = nullptr;
        pData[2] = nullptr;
        memset(&sDstBufInfo, 0, sizeof(SBufferInfo));
        // 设置时间戳
        sDstBufInfo.uiInBsTimeStamp = uiTimeStamp;
        // 刷新帧
        pDecoder->FlushFrame(pData, &sDstBufInfo);
        // 如果缓冲区状态为1，表示解码成功
        if (sDstBufInfo.iBufferStatus == 1) {
            // 获取解码后的数据
            pDst[0] = sDstBufInfo.pDst[0];
            pDst[1] = sDstBufInfo.pDst[1];
            pDst[2] = sDstBufInfo.pDst[2];
        }
        // 记录结束时间，并累加总时间
        int64_t iEnd = WelsTime();
        iTotal += iEnd - iStart;
        // 如果缓冲区状态为1，表示解码成功
        if (sDstBufInfo.iBufferStatus == 1) {
            // 处理解码后的数据
            Process((void **) pDst, &sDstBufInfo, pYuvFile);
            // 获取解码后的宽度和高度
            iWidth = sDstBufInfo.UsrData.sSystemBuffer.iWidth;
            iHeight = sDstBufInfo.UsrData.sSystemBuffer.iHeight;
            // 增加帧数
            ++iFrameCount;
        }
    }
}

void H264DecodeInstance(ISVCDecoder *pDecoder, const char *kpH264FileName, const char *kpOuputFileName,
                        int32_t &iWidth, int32_t &iHeight, const char *pOptionFileName, const char *pLengthFileName,
                        int32_t iErrorConMethod,
                        bool bLegacyCalling) {
    // 初始化各种文件指针
    FILE *pH264File = nullptr;
    FILE *pYuvFile = nullptr;

    // 如果解码器为空，则直接返回
    if (pDecoder == nullptr) return;

    // 初始化各种参数和缓冲区
    int32_t pInfo[4];
    unsigned long long uiTimeStamp = 0;
    int64_t iStart = 0, iEnd = 0, iTotal = 0;
    int32_t iSliceSize;
    int32_t iSliceIndex = 0;
    uint8_t *pBuf = nullptr;
    uint8_t uiStartCode[4] = {0, 0, 0, 1};

    uint8_t *pData[3] = {nullptr};
    uint8_t *pDst[3] = {nullptr};
    SBufferInfo sDstBufInfo;

    int32_t iBufPos = 0;
    int32_t iFileSize;
    int32_t iLastWidth = 0, iLastHeight = 0;
    int32_t iFrameCount = 0;
    int32_t iEndOfStreamFlag = 0;

    // 设置解码器的错误掩盖方法
    pDecoder->SetOption(DECODER_OPTION_ERROR_CON_IDC, &iErrorConMethod);

    double dElapsed = 0;
    uint8_t uLastSpsBuf[32];
    int32_t iLastSpsByteCount = 0;

    // 获取解码器的线程数
    int32_t iThreadCount = 1;
    pDecoder->GetOption(DECODER_OPTION_NUM_OF_THREADS, &iThreadCount);

    // 则打开文件
    pH264File = fopen(kpH264FileName, "rb");

    // 输出文件
    pYuvFile = fopen(kpOuputFileName, "wb");

    printf("------------------------------------------------------\n");

    // 获取H264文件的大小
    fseek(pH264File, 0L, SEEK_END);
    iFileSize = (int32_t) ftell(pH264File);
    if (iFileSize <= 4) {
        fprintf(stderr, "Current Bit Stream File is too small, read error!!!!\n");
        goto label_exit;
    }
    fseek(pH264File, 0L, SEEK_SET);

    // 分配缓冲区
    pBuf = new uint8_t[iFileSize + 4];

    // 读取H264文件到缓冲区，一次性读取整个文件了！
    if (fread(pBuf, 1, iFileSize, pH264File) != (uint32_t) iFileSize) {
        fprintf(stderr, "Unable to read whole file\n");
        goto label_exit;
    }

    // 设置缓冲区的结束标志
    memcpy(pBuf + iFileSize, &uiStartCode[0], 4); //confirmed_safe_unsafe_usage

    // 开始解码过程
    while (true) {
        // 如果缓冲区位置大于等于文件大小，表示已经读取完所有数据
        if (iBufPos >= iFileSize) {
            pDecoder->SetOption(DECODER_OPTION_END_OF_STREAM, (void *) &iEndOfStreamFlag);
            break;
        }
        // 如果线程数大于等于1，读取图片
        if (iThreadCount >= 1) {
            uint8_t *uSpsPtr = nullptr;
            int32_t iSpsByteCount = 0;
            iSliceSize = readPicture(pBuf, iFileSize, iBufPos, uSpsPtr, iSpsByteCount);

            // 如果新序列与前序列不同，必须刷新所有待处理的帧，然后才能开始解码新序列
            if (iLastSpsByteCount > 0 && iSpsByteCount > 0) {
                if (iSpsByteCount != iLastSpsByteCount || memcmp(uSpsPtr, uLastSpsBuf, iLastSpsByteCount) != 0) {
                    //whenever new sequence is different from preceding sequence. All pending frames must be flushed out before the new sequence can start to decode.
                    FlushFrames(pDecoder, iTotal, pYuvFile, nullptr, iFrameCount, uiTimeStamp, iWidth, iHeight,
                                iLastWidth,
                                iLastHeight);
                }
            }
            // 如果SPS字节计数大于0，复制SPS缓冲区
            if (iSpsByteCount > 0 && uSpsPtr != nullptr) {
                if (iSpsByteCount > 32) iSpsByteCount = 32;
                iLastSpsByteCount = iSpsByteCount;
                memcpy(uLastSpsBuf, uSpsPtr, iSpsByteCount);
            }
        } else {
            // 如果线程数小于1，查找下一个NALU的开始位置
            int i = 0;
            for (i = 0; i < iFileSize; i++) {
                if ((pBuf[iBufPos + i] == 0 && pBuf[iBufPos + i + 1] == 0 && pBuf[iBufPos + i + 2] == 0 &&
                     pBuf[iBufPos + i + 3] == 1
                     && i > 0) ||
                    (pBuf[iBufPos + i] == 0 && pBuf[iBufPos + i + 1] == 0 && pBuf[iBufPos + i + 2] == 1 && i > 0)) {
                    break;
                }
            }
            iSliceSize = i;
        }
        // 如果NALU的大小小于4，表示没有有效数据，忽略
        if (iSliceSize < 4) { //too small size, no effective data, ignore
            iBufPos += iSliceSize;
            continue;
        }

        // 记录开始时间
        iStart = WelsTime();
        pData[0] = nullptr;
        pData[1] = nullptr;
        pData[2] = nullptr;
        uiTimeStamp++;
        memset(&sDstBufInfo, 0, sizeof(SBufferInfo));
        sDstBufInfo.uiInBsTimeStamp = uiTimeStamp;

        // 调用解码器的解码函数
        pDecoder->DecodeFrameNoDelay(pBuf + iBufPos, iSliceSize, pData, &sDstBufInfo);


        // 如果解码成功，处理解码后的数据
        if (sDstBufInfo.iBufferStatus == 1) {
            pDst[0] = sDstBufInfo.pDst[0];
            pDst[1] = sDstBufInfo.pDst[1];
            pDst[2] = sDstBufInfo.pDst[2];
        }
        // 记录结束时间，并累加总时间
        iEnd = WelsTime();
        iTotal += iEnd - iStart;
        // 如果解码成功，处理解码后的数据
        if (sDstBufInfo.iBufferStatus == 1) {
            Process((void **) pDst, &sDstBufInfo, pYuvFile);
            iWidth = sDstBufInfo.UsrData.sSystemBuffer.iWidth;
            iHeight = sDstBufInfo.UsrData.sSystemBuffer.iHeight;

            // 增加帧数
            ++iFrameCount;
        }

        // 更新缓冲区位置
        iBufPos += iSliceSize;
        // 增加切片索引
        ++iSliceIndex;
    }

    // 刷新所有待处理的帧
    FlushFrames(pDecoder, iTotal, pYuvFile, nullptr, iFrameCount, uiTimeStamp, iWidth, iHeight, iLastWidth,
                iLastHeight);

    // 计算解码所用的时间
    dElapsed = iTotal / 1e6;
    // 打印解码信息
    fprintf(stderr, "-------------------------------------------------------\n");
    fprintf(stderr, "iWidth:\t\t%d\nheight:\t\t%d\nFrames:\t\t%d\ndecode time:\t%f sec\nFPS:\t\t%f fps\n",
            iWidth, iHeight, iFrameCount, dElapsed, (iFrameCount * 1.0) / dElapsed);
    fprintf(stderr, "-------------------------------------------------------\n");

    // 清理工作，释放内存，关闭文件
    label_exit:
    // 如果缓冲区不为空，则删除缓冲区并将指针设为NULL
    if (pBuf) {
        delete[] pBuf;
        pBuf = NULL;
    }
    // 如果H264文件打开，则关闭文件并将指针设为NULL
    if (pH264File) {
        fclose(pH264File);
        pH264File = NULL;
    }
    // 如果YUV文件打开，则关闭文件并将指针设为NULL
    if (pYuvFile) {
        fclose(pYuvFile);
        pYuvFile = NULL;
    }
}


int32_t main(int32_t iArgC, char *pArgV[]) {
    InitSpdLog();

    ISVCDecoder *pDecoder = nullptr;

    SDecodingParam sDecParam = {0};
    string strInputFile, strOutputFile(""), strOptionFile(""), strLengthFile("");

    sDecParam.sVideoProperty.size = sizeof(sDecParam.sVideoProperty);
    sDecParam.eEcActiveIdc = ERROR_CON_SLICE_MV_COPY_CROSS_IDR_FREEZE_RES_CHANGE;

    //usage 2: h264dec.exe test.264 test.yuv
    strInputFile = "../data/video/man_1008x888.h264";
    strOutputFile = "man_1008x888.yuv420";
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


    int32_t iWidth = 0;
    int32_t iHeight = 0;

    H264DecodeInstance(pDecoder, strInputFile.c_str(), strOutputFile.c_str(), iWidth,iHeight,NULL,NULL,(int32_t) sDecParam.eEcActiveIdc,false);

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
