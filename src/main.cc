#include "camera_reader.h"
#include "logger.h"
#include "logger_impl.h"
#include "model_manager.h"

#include <iostream>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <thread>

int main(int argc, char** argv)
{
    SetLogger(CLoggerImpl::Inst());
    if (argc != 2)
    {
        LOG_ERROR("Usage: %s <model_path>", argv[0]);
        return -1;
    }

    const char* model_path = argv[1];

    bool ret = false;
    ModelManager model_manager;
    ret = model_manager.LoadYolov7Model(model_path);
    if (!ret)
    {
        return 0;
    }

    ret = model_manager.LoadLLMModel("./model/DeepSeek-R1-Distill-Qwen-1.5B_W8A8_RK3588.rkllm");
    if (!ret)
    {
        return 0;
    }

    while (true)
    {
        LOG_INFO("Press Enter to start");
        std::cin.get();
        for (int i = 3; i > 0; --i)
        {
            LOG_INFO("Start in %d seconds", i);
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }

        CameraReader camera_reader(11);
        image_buffer_t src_image;
        camera_reader.Read(src_image);

        object_detect_result_list od_results;
        ret = model_manager.InferenceYolov7(src_image, od_results);
        if (!ret)
        {
            return 0;
        }

        int max_prop_index = -1;
        float max_prop = 0;

        for (int i = 0; i < od_results.count; i++)
        {
            object_detect_result* det_result = &(od_results.results[i]);
            if (det_result->prop > max_prop)
            {
                max_prop_index = i;
                max_prop = det_result->prop;
            }

            std::string class_name;
            model_manager.GetYoloClassName(det_result->cls_id, class_name);

            // LOG_INFO("%s @ (%d %d %d %d) %.3f", class_name.c_str(), det_result->box.left, det_result->box.top,
            //          det_result->box.right, det_result->box.bottom, det_result->prop);
        }

        if (max_prop_index >= 0)
        {
            object_detect_result* det_result = &(od_results.results[max_prop_index]);
            std::string class_name;
            model_manager.GetYoloClassName(det_result->cls_id, class_name);

            LOG_INFO("%s prop=%.3f", class_name.c_str(), det_result->prop);

            std::string input_format;
            char input_text[1024 * 5];

            switch (det_result->cls_id)
            {
                case 0:
                {
                    // Pink-red
                    input_format = "我的舌质呈淡红色或鲜红色，置信度为%.2f，有什么中医调理建议";
                    break;
                }
                case 1:
                {
                    // Thin-White
                    input_format = "我的舌质呈白而薄，置信度为%.2f，有什么中医调理建议";
                    break;
                }
                case 2:
                {
                    // White-Greasy
                    input_format = "我的舌质呈白而腻，置信度为%.2f，有什么中医调理建议";
                    break;
                }
                case 3:
                {
                    // Yellow-Greasy
                    input_format = "我的舌质呈黄而腻，置信度为%.2f，有什么中医调理建议";
                    break;
                }
                case 4:
                {
                    // Grey-Black
                    input_format = "我的舌质呈灰黑色，置信度为%.2f，有什么中医调理建议";
                    break;
                }
                default:
                    break;
            }

            snprintf(input_text, sizeof(input_text), input_format.c_str(), det_result->prop);

            LLMController* controller = model_manager.CreateLLMController();
            controller->Input(input_text);
            controller->WaitFinish();
            std::string output_text = controller->GetOutputText();
            LOG_INFO("%s", output_text.c_str());
            delete controller;
        }
    }

    // LLMController* controller = model_manager.CreateLLMController();
    // while (true)
    // {
    //     std::string input_text;
    //     std::getline(std::cin, input_text);
    //     if (input_text == "exit")
    //     {
    //         break;
    //     }

    // controller->Input(input_text);
    // controller->WaitFinish();

    // std::string output_text = controller->GetOutputText();

    // LOG_INFO("%s", output_text.c_str());
    // }
    // delete controller;
}
