#include "logger.h"

#include <cstdarg>
#include <exception>
#include <functional>

namespace
{

CLogger* g_pLogger = nullptr;
using LogFunc = std::function<void(const char* file, uint32_t line, const char* format, va_list ag)>;

void LogCore(const LogFunc& func, const char* file, uint32_t line, const char* format, va_list args)
{
    if (g_pLogger == nullptr)
    {
        return;
    }

    func(file, line, format, args);
}

} // namespace

void SetLogger(CLogger* pLogger)
{
    g_pLogger = pLogger;
    pLogger->Init();
}

void LogDebug(const char* file, uint32_t line, const char* format, ...)
{
    va_list args;
    va_start(args, format);
    LogCore([](const char* file, uint32_t line, const char* format, va_list args)
            { g_pLogger->LogDebug(file, line, format, args); }, file, line, format, args);
    va_end(args);
}

void LogRun(const char* file, uint32_t line, const char* format, ...)
{
    va_list args;
    va_start(args, format);
    LogCore([](const char* file, uint32_t line, const char* format, va_list args)
            { g_pLogger->LogRun(file, line, format, args); }, file, line, format, args);
    va_end(args);
}

void LogInfo(const char* file, uint32_t line, const char* format, ...)
{
    va_list args;
    va_start(args, format);
    LogCore([](const char* file, uint32_t line, const char* format, va_list args)
            { g_pLogger->LogInfo(file, line, format, args); }, file, line, format, args);
    va_end(args);
}

void LogWarn(const char* file, uint32_t line, const char* format, ...)
{
    va_list args;
    va_start(args, format);
    LogCore([](const char* file, uint32_t line, const char* format, va_list args)
            { g_pLogger->LogWarn(file, line, format, args); }, file, line, format, args);
    va_end(args);
}

void LogError(const char* file, uint32_t line, const char* format, ...)
{
    va_list args;
    va_start(args, format);
    LogCore([](const char* file, uint32_t line, const char* format, va_list args)
            { g_pLogger->LogError(file, line, format, args); }, file, line, format, args);
    va_end(args);
}

void SetLogLevel(LOG_LEVEL level)
{
    g_pLogger->SetLevel(level);
}

void DynamicAssert(bool valid, const char* file, uint32_t line)
{
    if (valid)
    {
        return;
    }

    LogError(file, line, "Assertion Failed! [%s]:[%u]", file, line);
    std::terminate();
}
