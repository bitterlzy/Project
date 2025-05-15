#ifndef CTIMER_H
#define CTIMER_H

#include <unistd.h>
#include <chrono>
#include <mutex>
#include <thread>
#include <atomic>
#include <condition_variable>
#include <functional>


class CTimer 
{
    public:
        CTimer();
        ~CTimer();
        CTimer(const CTimer& t);
    
        void StartTimer(int interval, std::function<void()> task);
        bool Expire();
    private:
        std::atomic<bool>           m_expired;        // 定时器是否过期
        std::atomic<bool>           m_try_to_expire;  // 定时器是否正在尝试过期
        std::mutex                  m_mutex;          // 互斥锁
        std::condition_variable     m_cond;           // 条件变量
};

#endif
