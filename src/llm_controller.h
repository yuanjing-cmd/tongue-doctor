#ifndef _LLM_CONTROLLER_H_
#define _LLM_CONTROLLER_H_

#include <atomic>
#include <condition_variable>
#include <mutex>
#include <string>

#include "rkllm.h"

class LLMController
{
public:
    LLMController(LLMHandle llmHandle, RKLLMPromptCacheParam prompt_cache_params);
    virtual ~LLMController() = default;

    virtual void OnLLMInferenceRunning(const std::string& text);
    virtual void OnLLMInferenceFinish();
    virtual void OnLLMInferenceError();

    bool Input(const std::string& input_text);

    void WaitFinish();

    const std::string& GetOutputText() const;

private:
    LLMHandle m_llmHandle{nullptr};
    RKLLMPromptCacheParam m_prompt_cache_params;

    std::string m_output_text;
    std::atomic<bool> m_finish{false};
    std::mutex m_mutex;
    std::condition_variable m_cond;
};

#endif // _LLM_CONTROLLER_H_
