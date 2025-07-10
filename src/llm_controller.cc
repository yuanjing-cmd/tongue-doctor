#include "llm_controller.h"

#include "logger.h"

#include <cstring>

#define PROMPT_TEXT_PREFIX "<｜begin▁of▁sentence｜><｜User｜>"
#define PROMPT_TEXT_POSTFIX "<｜Assistant｜>"
const std::string PROMPT_TEXT_CUSTOM_PREFIX = "以下是描述任务的说明，并附有提供进一步背景的输入。\n\
请写一个恰当的回应，完成该任务。\n\
在回答之前，请仔细思考问题，并创建一个逐步的思维链，以确保逻辑清晰且准确，只需要精准回答，回答时不要包含任何标签或日志信息。\n\
\n\
### 任务描述:\n\
请回答以下医学问题。\n\
\n\
### 问题:\n";
#define PROMPT_TEXT_CUSTOM_POSTFIX "\n### 回答:\n"

LLMController::LLMController(LLMHandle llmHandle, RKLLMPromptCacheParam prompt_cache_params, LLMObserver* observer)
    : m_llmHandle(llmHandle), m_prompt_cache_params(prompt_cache_params), m_observer(observer)
{
}

void LLMController::OnLLMInferenceRunning(const std::string& text)
{
    m_output_text += text;

    if (m_observer)
    {
        m_observer->OnLLMInferenceRunning(text);
    }
}

void LLMController::OnLLMInferenceFinish()
{
    LOG_INFO("RKLLM finish");
    m_finish.store(true);
    m_cond.notify_all();

    if (m_observer)
    {
        m_observer->OnLLMInferenceFinish();
    }
}

void LLMController::OnLLMInferenceError()
{
    LOG_INFO("RKLLM error");
    m_finish.store(true);
    m_cond.notify_all();

    if (m_observer)
    {
        m_observer->OnLLMInferenceError();
    }
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

    RKLLMInput rkllm_input;

    m_input_text =
        PROMPT_TEXT_PREFIX + PROMPT_TEXT_CUSTOM_PREFIX + input_text + PROMPT_TEXT_CUSTOM_POSTFIX + PROMPT_TEXT_POSTFIX;
    rkllm_input.input_type = RKLLM_INPUT_PROMPT;
    rkllm_input.prompt_input = (char*)m_input_text.c_str();

    int ret = rkllm_run_async(m_llmHandle, &rkllm_input, &rkllm_infer_params, this);
    if (ret != 0)
    {
        LOG_INFO("RKLLM run async error");
        return false;
    }
    LOG_INFO("RKLLM run async success");

    return true;
}

void LLMController::Abort()
{
    rkllm_abort(m_llmHandle);
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
