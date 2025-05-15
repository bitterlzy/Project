#ifndef CEVENTHANDLER_H
#define CEVENTHANDLER_H

#include "Programs/globalfunc.h"
#include "Programs/cthreadpool.h"
#include <memory>
#include <map>

class CEventHandler // 事件分发器
{
public:
    CEventHandler();
    ~CEventHandler();
    bool dispatchEvent(std::string event);
    
    void init();
private:
    void addEvent(std::string event, std::function<void()> task);
private:
    std::shared_ptr<CThreadPool> m_pThreadPool;
    std::map<std::string, std::function<void()>> m_events;
    std::mutex m_mutex;
};


#endif // CEVENTHANDLER_H