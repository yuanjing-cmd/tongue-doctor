#ifndef _LOGGER_IMPL_H_
#define _LOGGER_IMPL_H_

#include "logger.h"

class CLoggerImpl final : public CLogger
{
public:
    static CLoggerImpl* Inst()
    {
        static CLoggerImpl Inst;
        return &Inst;
    }

    CLoggerImpl(const CLoggerImpl&) = delete;
    CLoggerImpl(CLoggerImpl&&) = delete;
    CLoggerImpl& operator=(const CLoggerImpl&) = delete;
    CLoggerImpl& operator=(CLoggerImpl&&) = delete;

    bool Init() override;

    void LogDebug(const char* file, uint32_t line, const char* format, va_list args) override;
    void LogRun(const char* file, uint32_t line, const char* format, va_list args) override;
    void LogInfo(const char* file, uint32_t line, const char* format, va_list args) override;
    void LogWarn(const char* file, uint32_t line, const char* format, va_list args) override;
    void LogError(const char* file, uint32_t line, const char* format, va_list args) override;

private:
    CLoggerImpl() = default;
    ~CLoggerImpl() = default;
};

#endif // _LOGGER_IMPL_H_
