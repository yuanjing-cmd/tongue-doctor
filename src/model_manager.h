#ifndef _MODEL_MANAGER_H_
#define _MODEL_MANAGER_H_

#include "rkllm.h"
#include "yolov7.h"

#include "postprocess.h"

#include "llm_controller.h"

#include <string>

class ModelManager
{
public:
    ModelManager();
    ~ModelManager();

    bool LoadYolov7Model(const std::string& model_path);
    bool LoadLLMModel(const std::string& model_path);

    bool InferenceYolov7(image_buffer_t& src_image, object_detect_result_list& od_results);
    bool GetYoloClassName(int class_id, std::string& class_name);

    LLMController* CreateLLMController();

private:
    rknn_app_context_t m_rknn_app_ctx;
    LLMHandle m_llmHandle{nullptr};
};

#endif // _MODEL_MANAGER_H_
