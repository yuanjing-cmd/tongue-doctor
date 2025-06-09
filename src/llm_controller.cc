#include "llm_controller.h"

#include "logger.h"

#include <cstring>

#define PROMPT_TEXT_PREFIX "<｜begin▁of▁sentence｜><｜User｜>"
#define PROMPT_TEXT_POSTFIX "<｜Assistant｜>"

LLMController::LLMController(LLMHandle llmHandle, RKLLMPromptCacheParam prompt_cache_params)
    : m_llmHandle(llmHandle), m_prompt_cache_params(prompt_cache_params)
{
}

void LLMController::OnLLMInferenceRunning(const std::string& text)
{
    printf("%s", text.c_str());
    m_output_text += text;
}

void LLMController::OnLLMInferenceFinish()
{
    LOG_INFO("RKLLM finish");
    m_finish.store(true);
    m_cond.notify_all();
}

void LLMController::OnLLMInferenceError()
{
    LOG_INFO("RKLLM error");
    m_finish.store(true);
    m_cond.notify_all();
}

bool LLMController::Input(const std::string& input_text)
{
    m_output_text.clear();
    m_finish.store(false);

    // 初始化 infer 参数结构体
    RKLLMInferParam rkllm_infer_params;
    memset(&rkllm_infer_params, 0, sizeof(RKLLMInferParam)); // 将所有内容初始化为 0

    // rkllm_infer_params.prompt_cache_params = &m_prompt_cache_params;
    rkllm_infer_params.mode = RKLLM_INFER_GENERATE;

    std::string text;
    RKLLMInput rkllm_input;

    text = PROMPT_TEXT_PREFIX + input_text + PROMPT_TEXT_POSTFIX;
    rkllm_input.input_type = RKLLM_INPUT_PROMPT;
    rkllm_input.prompt_input = (char*)text.c_str();

    int ret = rkllm_run(m_llmHandle, &rkllm_input, &rkllm_infer_params, this);
    if (ret != 0)
    {
        return false;
    }

    return true;
}

void LLMController::WaitFinish()
{
    std::unique_lock<std::mutex> lock(m_mutex);
    m_cond.wait_for(lock, std::chrono::seconds(30), [&] { return m_finish.load(); });
}

const std::string& LLMController::GetOutputText() const
{
    return m_output_text;
}
