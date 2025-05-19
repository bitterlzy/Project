#ifndef CEVENTHANDLER_H
#define CEVENTHANDLER_H

#include "Programs/globalfunc.h"
#include "Programs/cthreadpool.h"
#include <json/json.h>
#include <memory>
#include <map>


namespace EVENT_SET
{
    std::string test_event(std::string args);
    std::string helloWorld(std::string args);
};

class CEventHandler // 事件分发器
{
public:
    CEventHandler();
    ~CEventHandler();
    
    void init();
    std::string handleEvent(std::string info);
    private:
    std::string dispatchEvent(std::string event, std::string args);
    void addEvent(std::string event, std::function<std::string(std::string)> task);
private:
    std::map<std::string, std::function<std::string(std::string)>> m_events;
    std::mutex m_mutex;
};


#endif // CEVENTHANDLER_H