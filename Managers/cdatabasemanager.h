#ifndef CDATABASEMANAGER_H
#define CDATABASEMANAGER_H

#include "mysql/mysql.h"
#include <iostream>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <memory>

class CDatabaseConnPool
{
    public:    
        CDatabaseConnPool(int max_conn, const std::string& host, const std::string& user, const std::string& password, const std::string& db_name);
        ~CDatabaseConnPool();
        CDatabaseConnPool& operator=(const CDatabaseConnPool&) = delete;
        CDatabaseConnPool(const CDatabaseConnPool&) = delete;
        MYSQL* GetConnection();
        void ReleaseConnection(MYSQL* conn);
    private:
        int m_max_conn;
        std::string m_host;
        std::string m_user;
        std::string m_password;
        std::string m_db_name;
        std::queue<MYSQL*> m_freeConn;
        std::mutex m_mutex;
        size_t m_cur_conn;
};

class CDatabaseManager
{
    public:
        ~CDatabaseManager();
        static CDatabaseManager* GetInstance();
        bool GetUsrInfo(const std::string& username, MYSQL_RES*& res);
    private:
        CDatabaseManager();
        CDatabaseManager(const CDatabaseManager&) = delete;
        CDatabaseManager& operator=(const CDatabaseManager&) = delete;
    
    private:
        std::shared_ptr<CDatabaseConnPool> m_connPool;
};

#endif // CDATABASEMANAGER_H
