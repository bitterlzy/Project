#include "ctimer.h"

CTimer::CTimer() : m_expired(true), m_try_to_expire(false) {}

CTimer::~CTimer() { Expire(); }

CTimer::CTimer(const CTimer& t) {m_expired = t.m_expired.load(); m_try_to_expire = t.m_try_to_expire.load(); }

void CTimer::StartTimer(int interval, std::function<void()> task)
{
    if (m_expired == false) 
        return;
    m_expired = false;
    std::thread([this, interval, task](){
        while (!this->m_try_to_expire)
        {
            std::this_thread::sleep_for(std::chrono::seconds(interval));
            task();
        }
        {
            std::unique_lock<std::mutex> lock(m_mutex);
            m_expired = true;
            m_cond.notify_one();
        }
    }).detach();
}

bool CTimer::Expire()
    {
        if (m_expired)
            return false;
        if (m_try_to_expire)
            return false;
        m_try_to_expire = true;
        {
            std::unique_lock<std::mutex> lock(m_mutex);
            m_cond.wait(lock, [this] {return this->m_expired == true;});
            if (m_expired == true)
            {
                m_try_to_expire = false;
            }
        }
        return true;
    }