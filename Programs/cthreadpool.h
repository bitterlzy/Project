#ifndef CTHREADPOOL_H
#define CTHREADPOOL_H
#include <iostream>
#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <memory>


class CThreadPool
{
public:
    CThreadPool(int threadNum);
    ~CThreadPool();
    
    // 添加任务
    void addTask(std::function<void()> task);
    
    // 停止所有线程
    void Stop();
    
    // 获取当前等待中的任务数
    size_t getPendingTaskCount();
private:
    // 工作线程函数
    void Worker();
    // 线程池
    std::vector<std::thread> m_threads;
    // 任务队列
    std::queue<std::function<void()>> m_tasks;
    // 同步相关
    std::mutex m_mutex;
    std::condition_variable m_condition;
    bool m_stop;
};

#endif // CTHREADPOOL_H