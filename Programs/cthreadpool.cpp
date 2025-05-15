#include "Programs/cthreadpool.h"

CThreadPool::CThreadPool(int threadNum) : m_stop(false)
{
    for (int i = 0; i < threadNum; ++i)
    {
        m_threads.emplace_back(&CThreadPool::Worker, this);
    }
}

CThreadPool::~CThreadPool()
{
    Stop();
}

void CThreadPool::addTask(std::function<void()> task)
{
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        // 如果线程池已停止，抛出异常
        if(m_stop)
            throw std::runtime_error("AddTask on stopped ThreadPool");
        
        // 将任务添加到队列
        m_tasks.push(std::move(task));
    }
    // 通知一个等待中的线程
    m_condition.notify_one();
}

void CThreadPool::Stop()
{
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_stop = true;
    }
    
    m_condition.notify_all();
    
    for (auto& thread : m_threads)
    {
        if (thread.joinable())
            thread.join();
    }
}

size_t CThreadPool::getPendingTaskCount()
{
    std::unique_lock<std::mutex> lock(m_mutex);
    return m_tasks.size();
}

void CThreadPool::Worker()
{
    while (true)
    {
        std::function<void()> task;
        {
            std::unique_lock<std::mutex> lock(m_mutex);
            
            m_condition.wait(lock, [this](){
                return !m_tasks.empty() || m_stop;
            });
            
            if (m_stop && m_tasks.empty())
                return;
            
            task = std::move(m_tasks.front());
            m_tasks.pop();
        }
        task();
    }
}