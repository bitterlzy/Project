#ifndef CGLOBALMANAGER_H
#define CGLOBALMANAGER_H

#include <iostream>
#include <Programs/cproject.h>
#include <memory>
class CGlobalManager
{
    public:
        static CGlobalManager* GetInstance();
        void CreateProject();
        std::shared_ptr<CProject> GetProject() const;
        ~CGlobalManager();
    private:
        CGlobalManager() = default;
        CGlobalManager(const CGlobalManager&) = delete;
        CGlobalManager& operator=(const CGlobalManager&) = delete;
    
    private:
        std::shared_ptr<CProject> m_project;
};

#endif // CGLOBALMANAGER_H
