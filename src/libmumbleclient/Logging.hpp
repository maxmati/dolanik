#ifndef _LIBMUMBLECLIENT_LOGGING_H_
#define _LIBMUMBLECLIENT_LOGGING_H_

#include <cstdint>
#include <sstream>

#include "Visibility.hpp"

namespace MumbleClient {

namespace logging {

void DLL_PUBLIC SetLogLevel(int32_t level);
int32_t DLL_PUBLIC GetLogLevel();

typedef int32_t LogSeverity;
const LogSeverity LOG_INFO = 0;
const LogSeverity LOG_WARNING = 1;
const LogSeverity LOG_ERROR = 2;
const LogSeverity LOG_FATAL = 3;
const LogSeverity LOG_NUM_SEVERITIES = 4;

#define MC_LOG_INFO ::MumbleClient::logging::LogMessage(__FILE__, __LINE__, ::MumbleClient::logging::LOG_INFO)
#define MC_LOG_WARNING ::MumbleClient::logging::LogMessage(__FILE__, __LINE__, ::MumbleClient::logging::LOG_WARNING)
#define MC_LOG_ERROR ::MumbleClient::logging::LogMessage(__FILE__, __LINE__, ::MumbleClient::logging::LOG_ERROR)
#define MC_LOG_FATAL ::MumbleClient::logging::LogMessage(__FILE__, __LINE__, ::MumbleClient::logging::LOG_FATAL)

#define LOG(severity) MC_LOG_ ## severity.stream()

class DLL_PUBLIC LogMessage {
public:
    LogMessage(const char* file, int32_t line);
    LogMessage(const char* file, int32_t line, LogSeverity severity);

    ~LogMessage();

    std::ostream& stream() { return stream_; }

private:
    void Init(const char* file, int32_t line);

    LogSeverity severity_;
    std::ostringstream stream_;
    std::streamoff message_start_;

    LogMessage(const LogMessage&);
    void operator=(const LogMessage&);
};


}  // namespace logging

}  // namespace MumbleClient

#endif  // LOGGING_H_
