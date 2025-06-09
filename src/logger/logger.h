#ifndef _LOGGER_H_
#define _LOGGER_H_

#include <cstdarg>
#include <cstdint>

enum class LOG_LEVEL : uint16_t
{
    DEBUG = 0,
    RUN,
    INFO,
    WARN,
    ERROR
};

class CLogger
{
public:
    virtual ~CLogger() = default;

    virtual bool Init() = 0;

    virtual void LogDebug(const char* file, uint32_t line, const char* format, va_list args) = 0;
    virtual void LogRun(const char* file, uint32_t line, const char* format, va_list args) = 0;
    virtual void LogInfo(const char* file, uint32_t line, const char* format, va_list args) = 0;
    virtual void LogWarn(const char* file, uint32_t line, const char* format, va_list args) = 0;
    virtual void LogError(const char* file, uint32_t line, const char* format, va_list args) = 0;

    virtual void SetLevel(LOG_LEVEL level)
    {
        m_Level = level;
    }

protected:
    LOG_LEVEL m_Level{LOG_LEVEL::DEBUG};
};

void SetLogger(CLogger* logger);

#define _FORMAT_N 3
#define _FORMAT_M 4
[[gnu::format(printf, _FORMAT_N, _FORMAT_M)]] void LogDebug(const char* file, uint32_t line, const char* format, ...);

[[gnu::format(printf, _FORMAT_N, _FORMAT_M)]] void LogRun(const char* file, uint32_t line, const char* format, ...);

[[gnu::format(printf, _FORMAT_N, _FORMAT_M)]] void LogInfo(const char* file, uint32_t line, const char* format, ...);

[[gnu::format(printf, _FORMAT_N, _FORMAT_M)]] void LogWarn(const char* file, uint32_t line, const char* format, ...);

[[gnu::format(printf, _FORMAT_N, _FORMAT_M)]] void LogError(const char* file, uint32_t line, const char* format, ...);

void SetLogLevel(LOG_LEVEL level);
void DynamicAssert(bool valid, const char* file, uint32_t line);
#define DYNAMIC_ASSERT(valid) DynamicAssert(valid, __FILE__, __LINE__)

#define LOG_DEBUG(fmt, ...) LogDebug(__FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define LOG_RUN(fmt, ...) LogRun(__FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define LOG_INFO(fmt, ...) LogInfo(__FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define LOG_WARN(fmt, ...) LogWarn(__FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define LOG_ERROR(fmt, ...) LogError(__FILE__, __LINE__, fmt, ##__VA_ARGS__)

#define LOG_ASSERT(a)                                                                                                  \
    do                                                                                                                 \
    {                                                                                                                  \
        if (!(a))                                                                                                      \
        {                                                                                                              \
            LogError(__FILE__, __LINE__, #a);                                                                          \
        }                                                                                                              \
    } while (0)

#endif // _LOGGER_H_
