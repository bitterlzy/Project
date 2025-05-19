#include "Programs/ceventhandler.h"
#include "ceventhandler.h"
#include <json/value.h>
#include <json/writer.h>

/********************** 事件存储 **************************/
std::string EVENT_SET::test_event(std::string args)
{
    std::cout << "test_service success" << std::endl;
    return "test_service success";  // 添加返回值
}

std::string EVENT_SET::helloWorld(std::string args)
{
    std::cout << "test_service success" << std::endl;
    std::string res = "helloWorld"+ args;
    return res;  // 添加返回值
}

CEventHandler::CEventHandler()
{
}

CEventHandler::~CEventHandler()
{
}

void CEventHandler::addEvent(std::string event, std::function<std::string(std::string)> task)
{
    m_events[event] = task;
}

void CEventHandler::init() {
    std::cout << "CEventHandler::init" << std::endl;
    // 添加测试事件
    auto func = std::bind([](std::string args){return EVENT_SET::test_event(args);}, std::placeholders::_1);
    addEvent("test_service", func);
    auto func1 = std::bind([](std::string args){return EVENT_SET::helloWorld(args);}, std::placeholders::_1);
    addEvent("helloWorld", func1);
}

std::string CEventHandler::handleEvent(std::string info)
{
    Json::Value root;
    Json::Value response;  // 修正拼写错误
    Json::Reader reader;

    if (reader.parse(info, root)) {
        std::string event = root["method"].asString();
        std::string args = root["args"].asString();
        std::string result = dispatchEvent(event, args);
        return result;  // 返回实际结果而不是固定字符串
    } else {
        return "error: JSON parse failed";  // 添加解析失败的返回值
    }
}

std::string CEventHandler::dispatchEvent(std::string event, std::string args)
{
    auto it = m_events.find(event);
    if (it == m_events.end()) {
        return "error: event not found";
    }
    // 执行事件处理函数并传递参数
    std::function<std::string(std::string)> eventFunc = it->second;
    std::string res = eventFunc(args);  // 传递 args 参数
    return res;
}