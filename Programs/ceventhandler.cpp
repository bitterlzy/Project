#include "Programs/ceventhandler.h"
#include "ceventhandler.h"

CEventHandler::CEventHandler()
{
    m_pThreadPool = std::make_shared<CThreadPool>(std::thread::hardware_concurrency());
}

CEventHandler::~CEventHandler()
{
}

void CEventHandler::addEvent(std::string event, std::function<void()> task)
{
    std::unique_lock<std::mutex> lock(m_mutex);
    m_events[event] = task;
}

void CEventHandler::init() {
    std::cout << "CEventHandler::init" << std::endl;
    // 添加测试事件
    addEvent("test_service", []() {
        std::cout << "test_service success" << std::endl;
    });
}

bool CEventHandler::dispatchEvent(std::string event)
{
    std::unique_lock<std::mutex> lock(m_mutex);
    auto it = m_events.find(event);
    if (it != m_events.end()) {
        m_pThreadPool->addTask(it->second);
    }
    else {
        return false;
    }
    return true;
}