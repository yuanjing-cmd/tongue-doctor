#ifndef _MODEL_MANAGER_H_
#define _MODEL_MANAGER_H_

#include "rkllm.h"
#include "yolov7.h"

#include "llm_controller.h"
#include "yolo_controller.h"

#include <string>

class ModelManager
{
public:
    static ModelManager& Inst()
    {
        static ModelManager inst;
        return inst;
    };

public:
    ModelManager();
    ~ModelManager();

    bool LoadYolov7Model(const std::string& model_path);
    bool LoadLLMModel(const std::string& model_path);

    std::unique_ptr<YoloController> CreateYoloController(YoloController::YoloObserver* observer);
    std::unique_ptr<LLMController> CreateLLMController(LLMController::LLMObserver* observer);

private:
    rknn_app_context_t m_rknn_app_ctx;
    LLMHandle m_llmHandle{nullptr};
};

#endif // _MODEL_MANAGER_H_
