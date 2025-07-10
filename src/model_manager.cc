#include <cstddef>
#include <cstdint>

#include "logger.h"
#include "model_manager.h"

#include <cstring>
#include <fstream>
#include <iostream>

ModelManager::ModelManager()
{
}

ModelManager::~ModelManager()
{
    int ret = release_yolov7_model(&m_rknn_app_ctx);
    if (ret != 0)
    {
        LOG_ERROR("release_yolov7_model fail! ret=%d\n", ret);
    }

    rkllm_release_prompt_cache(m_llmHandle);

    rkllm_destroy(m_llmHandle);
}

bool ModelManager::LoadYolov7Model(const std::string& model_path)
{
    memset(&m_rknn_app_ctx, 0, sizeof(rknn_app_context_t));
    init_post_process("./model/tongue_labels_list.txt");

    int ret = 0;
    ret = init_yolov7_model(model_path.c_str(), &m_rknn_app_ctx);
    if (ret != 0)
    {
        deinit_post_process();
        LOG_ERROR("init_yolov7_model fail! ret=%d model_path=%s\n", ret, model_path.c_str());
        return false;
    }

    return true;
}

void callback(RKLLMResult* result, void* userdata, LLMCallState state)
{
    LLMController* controller = (LLMController*)userdata;
    if (state == RKLLM_RUN_FINISH)
    {
        controller->OnLLMInferenceFinish();
    }
    else if (state == RKLLM_RUN_ERROR)
    {
        controller->OnLLMInferenceError();
    }
    else if (state == RKLLM_RUN_GET_LAST_HIDDEN_LAYER)
    {
        /* ================================================================================================================
        若使用GET_LAST_HIDDEN_LAYER功能,callback接口会回传内存指针:last_hidden_layer,token数量:num_tokens与隐藏层大小:embd_size
        通过这三个参数可以取得last_hidden_layer中的数据
        注:需要在当前callback中获取,若未及时获取,下一次callback会将该指针释放
        ===============================================================================================================*/
        if (result->last_hidden_layer.embd_size != 0 && result->last_hidden_layer.num_tokens != 0)
        {
            int data_size = result->last_hidden_layer.embd_size * result->last_hidden_layer.num_tokens * sizeof(float);
            printf("\ndata_size:%d", data_size);
            std::ofstream outFile("last_hidden_layer.bin", std::ios::binary);
            if (outFile.is_open())
            {
                outFile.write(reinterpret_cast<const char*>(result->last_hidden_layer.hidden_states), data_size);
                outFile.close();
                std::cout << "Data saved to output.bin successfully!" << std::endl;
            }
            else
            {
                std::cerr << "Failed to open the file for writing!" << std::endl;
            }
        }
    }
    else if (state == RKLLM_RUN_NORMAL)
    {
        controller->OnLLMInferenceRunning(result->text);
    }
}

bool ModelManager::LoadLLMModel(const std::string& model_path)
{
    // 设置参数及初始化
    RKLLMParam param = rkllm_createDefaultParam();
    param.model_path = model_path.c_str();

    // 设置采样参数
    param.top_k = 1;
    param.top_p = 0.95;
    param.temperature = 0.8;
    param.repeat_penalty = 1.1;
    param.frequency_penalty = 0.0;
    param.presence_penalty = 0.0;

    param.max_new_tokens = 10000;
    param.max_context_len = 10000;
    param.skip_special_token = true;
    param.is_async = true;
    param.extend_param.base_domain_id = 0;

    int ret = rkllm_init(&m_llmHandle, &param, callback);
    if (ret != 0)
    {
        LOG_ERROR("rkllm init failed, ret = %d", ret);
        return false;
    }

    LOG_INFO("rkllm init success");

    RKLLMLoraAdapter lora_adapter;
    memset(&lora_adapter, 0, sizeof(RKLLMLoraAdapter));
    lora_adapter.lora_adapter_path = "./model/DeepSeek-R1-Distill-Qwen-1.5B_W8A8_RK3588_lora.rkllm";
    lora_adapter.lora_adapter_name = "test";
    lora_adapter.scale = 1.0;
    ret = rkllm_load_lora(m_llmHandle, &lora_adapter);
    if (ret != 0)
    {
        LOG_ERROR("rkllm load lora failed, ret = %d", ret);
        return false;
    }

    LOG_INFO("rkllm load lora success");

    // rkllm_load_prompt_cache(m_llmHandle, "./prompt_cache.bin"); // 加载缓存的cache

    return true;
}

std::unique_ptr<YoloController> ModelManager::CreateYoloController(YoloController::YoloObserver* observer)
{
    return std::unique_ptr<YoloController>(new YoloController(&m_rknn_app_ctx, observer));
}

std::unique_ptr<LLMController> ModelManager::CreateLLMController(LLMController::LLMObserver* observer)
{
    RKLLMPromptCacheParam prompt_cache_params;
    prompt_cache_params.save_prompt_cache = true;                 // 是否保存 prompt cache
    prompt_cache_params.prompt_cache_path = "./prompt_cache.bin"; // 若需要保存prompt cache, 指定 cache 文件路径

    return std::unique_ptr<LLMController>(new LLMController(m_llmHandle, prompt_cache_params, observer));
}
