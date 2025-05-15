#include "cglobalmanager.h"

CGlobalManager* CGlobalManager::GetInstance()
{
    static CGlobalManager instance;
    return &instance;
}

std::shared_ptr<CProject> CGlobalManager::GetProject() const
{
    return m_project;
}

CGlobalManager::~CGlobalManager()
{
}

void CGlobalManager::CreateProject()
{
    m_project = std::make_shared<CProject>();
}