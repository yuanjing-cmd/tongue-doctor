#include "logger_impl.h"

#include <chrono>
#include <cstdarg>
#include <cstdio>
#include <iomanip>
#include <sstream>

std::string getCurrentTimeWithMs()
{
    auto now = std::chrono::system_clock::now();
    auto now_time = std::chrono::system_clock::to_time_t(now);

    // 获取毫秒部分
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;

    std::tm* local_time = std::localtime(&now_time);

    std::ostringstream oss;
    oss << std::put_time(local_time, "%Y-%m-%d %H:%M:%S") << '.' << std::setfill('0') << std::setw(3) << ms.count();

    return oss.str();
}

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

    std::string time = getCurrentTimeWithMs();
    fprintf(stdout, "[D][%s] ", time.c_str());
    vfprintf(stdout, format, args);
    fprintf(stdout, "[%s:%u]\n", file, line);
}

void CLoggerImpl::LogRun(const char* file, uint32_t line, const char* format, va_list args)
{
    if (m_Level > LOG_LEVEL::RUN)
    {
        return;
    }

    std::string time = getCurrentTimeWithMs();
    fprintf(stdout, "[R][%s] ", time.c_str());
    vfprintf(stdout, format, args);
    fprintf(stdout, "[%s:%u]\n", file, line);
}

void CLoggerImpl::LogInfo(const char* file, uint32_t line, const char* format, va_list args)
{
    if (m_Level > LOG_LEVEL::INFO)
    {
        return;
    }

    std::string time = getCurrentTimeWithMs();
    fprintf(stdout, "[I][%s] ", time.c_str());
    vfprintf(stdout, format, args);
    fprintf(stdout, "[%s:%u]\n", file, line);
}

void CLoggerImpl::LogWarn(const char* file, uint32_t line, const char* format, va_list args)
{
    if (m_Level > LOG_LEVEL::WARN)
    {
        return;
    }

    std::string time = getCurrentTimeWithMs();
    fprintf(stdout, "[W][%s] ", time.c_str());
    vfprintf(stdout, format, args);
    fprintf(stdout, "[%s:%u]\n", file, line);
}

void CLoggerImpl::LogError(const char* file, uint32_t line, const char* format, va_list args)
{
    if (m_Level > LOG_LEVEL::ERROR)
    {
        return;
    }

    std::string time = getCurrentTimeWithMs();
    fprintf(stdout, "[E][%s] ", time.c_str());
    vfprintf(stdout, format, args);
    fprintf(stdout, "[%s:%u]\n", file, line);
}
