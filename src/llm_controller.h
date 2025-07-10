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
    class LLMObserver
    {
    public:
        virtual void OnLLMInferenceRunning(const std::string& text) = 0;
        virtual void OnLLMInferenceFinish() = 0;
        virtual void OnLLMInferenceError() = 0;
    };

public:
    LLMController(LLMHandle llmHandle, RKLLMPromptCacheParam prompt_cache_params, LLMObserver* observer);
    virtual ~LLMController() = default;

    virtual void OnLLMInferenceRunning(const std::string& text);
    virtual void OnLLMInferenceFinish();
    virtual void OnLLMInferenceError();

    bool Input(const std::string& input_text);

    void Abort();

    void WaitFinish();

    const std::string& GetOutputText() const;

private:
    LLMHandle m_llmHandle{nullptr};
    RKLLMPromptCacheParam m_prompt_cache_params;
    LLMObserver* m_observer{nullptr};

    std::string m_output_text;
    std::atomic<bool> m_finish{false};
    std::mutex m_mutex;
    std::condition_variable m_cond;

    std::string m_input_text;
};

#endif // _LLM_CONTROLLER_H_
