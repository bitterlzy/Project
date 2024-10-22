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
    // 构造函数，初始化m_expired和m_try_to_expire为true
    CTimer() : m_expired(true), m_try_to_expire(false) {}
    // 析构函数，调用Expire函数
    ~CTimer() { Expire(); };
    // 拷贝构造函数，拷贝m_expired和m_try_to_expire
    CTimer(const CTimer& t) {m_expired = t.m_expired.load(); m_try_to_expire = t.m_try_to_expire.load(); }

    // 启动定时器，参数为时间间隔和任务
    void StartTimer(int interval, std::function<void()> task)
    {
        // 如果定时器已经启动，则直接返回
        if (m_expired == false) 
            return;
        // 设置定时器为已启动
        m_expired = false;
        // 创建一个线程，执行任务
        std::thread([this, interval, task](){
            // 循环执行任务，直到m_try_to_expire为true
            while (!this->m_try_to_expire)
            {
                // 休眠一段时间
                std::this_thread::sleep_for(std::chrono::seconds(interval));
                // 执行任务
                task();
            }
            // 释放锁，设置定时器为已过期
            {
                std::unique_lock<std::mutex> lock(m_mutex);
                m_expired = true;
                m_cond.notify_one();
            }
        }).detach();
    }

    // 使定时器过期
    bool Expire()
    {
        // 如果定时器已经过期，则直接返回
        if (m_expired)
            return false;
        // 如果定时器正在尝试过期，则直接返回
        if (m_try_to_expire)
            return false;
        // 设置定时器为正在尝试过期
        m_try_to_expire = true;
        // 加锁
        {
            std::unique_lock<std::mutex> lock(m_mutex);
            // 等待定时器过期
            m_cond.wait(lock, [this] {return this->m_expired == true;});
            // 如果定时器已经过期，则设置定时器为未尝试过期
            if (m_expired == true)
            {
                m_try_to_expire = false;
            }
        }
        return true;
    }

private:
    // 定时器是否过期
    std::atomic<bool>           m_expired;
    // 定时器是否正在尝试过期
    std::atomic<bool>           m_try_to_expire;
    // 互斥锁
    std::mutex                  m_mutex;
    // 条件变量
    std::condition_variable     m_cond;
};

#endif
