#include "logger_impl.h"

#include <cstdarg>
#include <cstdio>

bool CLoggerImpl::Init()
{
    return true;
}

void CLoggerImpl::LogDebug(const char* file, uint32_t line, const char* format, va_list args)
{
    if (m_Level > LOG_LEVEL::DEBUG)
    {
        return;
    }

    fprintf(stdout, "[DEBUG]: %s:%u: ", file, line);
    vfprintf(stdout, format, args);
    fprintf(stdout, "\n");
}

void CLoggerImpl::LogRun(const char* file, uint32_t line, const char* format, va_list args)
{
    if (m_Level > LOG_LEVEL::RUN)
    {
        return;
    }

    fprintf(stdout, "[RUN]: %s:%u: ", file, line);
    vfprintf(stdout, format, args);
    fprintf(stdout, "\n");
}

void CLoggerImpl::LogInfo(const char* file, uint32_t line, const char* format, va_list args)
{
    if (m_Level > LOG_LEVEL::INFO)
    {
        return;
    }

    fprintf(stdout, "[INFO]: %s:%u: ", file, line);
    vfprintf(stdout, format, args);
    fprintf(stdout, "\n");
}

void CLoggerImpl::LogWarn(const char* file, uint32_t line, const char* format, va_list args)
{
    if (m_Level > LOG_LEVEL::WARN)
    {
        return;
    }

    fprintf(stdout, "[WARN]: %s:%u: ", file, line);
    vfprintf(stdout, format, args);
    fprintf(stdout, "\n");
}

void CLoggerImpl::LogError(const char* file, uint32_t line, const char* format, va_list args)
{
    if (m_Level > LOG_LEVEL::ERROR)
    {
        return;
    }

    fprintf(stdout, "[ERROR]: %s:%u: ", file, line);
    vfprintf(stdout, format, args);
    fprintf(stdout, "\n");
}
