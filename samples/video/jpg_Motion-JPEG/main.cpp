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
#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"
#include <stdlib.h>
#include <stdio.h>

#include <opencv2/opencv.hpp>
#include <filesystem>


void InitSpdLog() {
    try
    {
        auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
        console_sink->set_level(spdlog::level::trace);
        console_sink->set_pattern("[multi_sink_example] [%^%l%$] %v");

        auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>("logs/multisink.txt", true);
        file_sink->set_level(spdlog::level::trace);

        spdlog::sinks_init_list sink_list = { file_sink, console_sink };

        // you can even set multi_sink logger as default logger
        spdlog::set_default_logger(std::make_shared<spdlog::logger>("multi_sink", spdlog::sinks_init_list({console_sink, file_sink})));
    }
    catch (const spdlog::spdlog_ex& ex)
    {
        std::cout << "Log initialization failed: " << ex.what() << std::endl;
    }
}


int main(void)
{
    InitSpdLog();

    std::string image_file_path="../data/images/fbi.jpg";
    std::string video_file_path="output.avi";

    //        cv::Mat frame = cv::imread(image_file_path);

    int width,height=0;
    unsigned short texture_format=0;

    int channels_in_file;//通道数
    unsigned char* data = stbi_load(image_file_path.c_str(), &(width), &(height), &channels_in_file, 0);
    if (data!= nullptr) {
        //根据颜色通道数，判断颜色格式。
        switch (channels_in_file) {
            case 1:{
                texture_format = GL_ALPHA;
                break;
            }
            case 3:{
                texture_format = GL_RGB;
                break;
            }
            case 4:{
                texture_format = GL_RGBA;
                break;
            }
        }
    }

    // Create a VideoWriter object
    cv::VideoWriter writer;

    // Initialize the VideoWriter object
    int codec = cv::VideoWriter::fourcc('M', 'J', 'P', 'G');  // Select codec
    double fps = 60.0;  // Frame rate
    cv::Size frameSize(width, height);  // Frame size
    writer.open(video_file_path, codec, fps, frameSize, true);

    // Check if the writer is opened successfully
    if (!writer.isOpened()) {
        std::cerr << "Could not open the output video file for write\n";
        return -1;
    }

    // Write frames
    for (int i = 0; i < 100; i++) {
        // Convert to cv::Mat
        cv::Mat frame(height, width, CV_8UC3, data);

        // stb读出来的是rgb，但是opencv的是bgr，所以要转换一下。在UE4中就直接创建BGR的纹理好了。
        cv::Mat frame_bgr;
        cv::cvtColor(frame, frame_bgr, cv::COLOR_RGB2BGR);

        // Check if the frame is valid
        if (frame_bgr.empty()) {
            std::cerr << "Can't read frames\n";
            break;
        }

        // Write the frame into the file
        writer.write(frame_bgr);
    }

    // Release the VideoWriter
    writer.release();

    // 解码
    // 检查Frames目录是否存在
    if (!std::filesystem::exists("Frames")) {
        // 创建目录Frames
        std::filesystem::create_directory("Frames");
    }

    cv::VideoCapture cap(video_file_path);

    if (!cap.isOpened()) {
        std::cerr << "Error opening video file\n";
        return -1;
    }

    cv::Mat frame;
    int frameNumber = 0;
    auto start = std::chrono::high_resolution_clock::now();
    while (true) {
        // Read the next frame
        if (!cap.read(frame)) {
            // If read failed, wait for a moment and try again
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
            auto now = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::seconds>(now - start);
            if (duration.count() > 10) {
                std::cerr << "Timeout waiting for video frames\n";
                break;
            }
            continue;
        }

        // Save the frame as an image
        std::string filename = "Frames/frame" + std::to_string(frameNumber) + ".jpg";
        cv::imwrite(filename, frame);

        frameNumber++;
        start = std::chrono::high_resolution_clock::now(); // Reset the start time after successfully reading a frame
    }

    cap.release();

    exit(EXIT_SUCCESS);
}