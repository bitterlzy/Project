#include "globalfunc.h"
#include <sstream>
#include <iomanip>
#include <ctime>

// 获取当前时间(格式: 2025-03-07 10:00:00)
std::string CGlobalFunc::getCurrentTime() 
{
    return CGlobalFunc::getTimeBySeconds(time(nullptr));
}

// 将时间戳转换为字符串格式(格式: 2025-03-07 10:00:00)
std::string CGlobalFunc::getTimeBySeconds(time_t seconds) 
{
    struct tm tm;
    #ifdef _WIN32
        if (localtime_s(&tm, &seconds) != 0) {
            throw std::runtime_error("Failed to convert time");
        }
    #else
        if (!localtime_r(&seconds, &tm)) {
            throw std::runtime_error("Failed to convert time");
        }
    #endif
    char buffer[80];
    if (strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", &tm) == 0) {
        throw std::runtime_error("Buffer too small");
    }
    return buffer;
}

// 将字符串格式时间转换为时间戳(格式: 1715136000)
time_t CGlobalFunc::getSecondsByTime(const std::string& time) 
{
    struct tm tm = {};
    std::istringstream ss(time);
    ss >> std::get_time(&tm, "%Y-%m-%d %H:%M:%S");
    if (ss.fail()) {
        throw std::invalid_argument("Invalid time format");
    }
    tm.tm_isdst = -1;
    time_t result = mktime(&tm);
    if (result == -1) {
        throw std::runtime_error("Invalid time value");
    }
    return result;
}

// 获取当前时间戳(格式: 1715136000)
time_t CGlobalFunc::getCurrentTimeInSeconds() 
{
    return time(nullptr);
}

size_t CGlobalFunc::getCPUCoreCount()
{
    return std::thread::hardware_concurrency();
}