#include "cdatabasemanager.h"

CDatabaseConnPool::CDatabaseConnPool(int max_conn, const std::string& host, const std::string& user, const std::string& password, const std::string& db_name)
{
    m_max_conn = max_conn;
    m_host = host;
    m_user = user;
    m_password = password;
    m_db_name = db_name;
    m_cur_conn = 0;
}

CDatabaseConnPool::~CDatabaseConnPool()
{
    while (!m_freeConn.empty()) {
        MYSQL* conn = m_freeConn.front();
        m_freeConn.pop();
        mysql_close(conn);
    }
}

MYSQL* CDatabaseConnPool::GetConnection()
{
    MYSQL* conn = nullptr;
    m_mutex.lock();
    if (m_freeConn.empty()) {
        if (m_cur_conn >= m_max_conn) {
            m_mutex.unlock();
            std::cout << "目前已经达到最大连接数!!!" << std::endl;
            return nullptr;
        }
        conn = mysql_init(nullptr);
        if (conn == nullptr) {
            m_mutex.unlock();
            return nullptr;
        }
        conn = mysql_real_connect(conn, m_host.c_str(), m_user.c_str(), m_password.c_str(), m_db_name.c_str(), 0, nullptr, 0);
        if (conn == nullptr) {
            m_mutex.unlock();
            return nullptr;
        }
        m_cur_conn++;
    }
    else {
        conn = m_freeConn.front();
        m_freeConn.pop();
    }
    m_mutex.unlock();
    return conn;
}

void CDatabaseConnPool::ReleaseConnection(MYSQL* conn)
{
    m_mutex.lock();
    m_freeConn.push(conn);
    m_mutex.unlock();
}

// ------------------------------------------- Manager -------------------------------------------

CDatabaseManager::CDatabaseManager()
{
    m_connPool = std::make_shared<CDatabaseConnPool>(10, "172.25.192.1", "root", "li991208", "lzy");
}

CDatabaseManager::~CDatabaseManager()
{
}

CDatabaseManager* CDatabaseManager::GetInstance()
{
    static CDatabaseManager instance;
    return &instance;
}

bool CDatabaseManager::GetUsrInfo(const std::string& username, MYSQL_RES*& res)
{
    MYSQL* conn = nullptr;
    conn = m_connPool->GetConnection();
    if (conn == nullptr) {
        std::cout << "获取数据库连接失败!!!" << std::endl;
        return false;
    }
    std::string sql = "SELECT * FROM T_usr WHERE Name = '" + username + "';";
    if (mysql_query(conn, sql.c_str())) {
        std::cout << "执行SQL语句失败!!!" << std::endl;
        m_connPool->ReleaseConnection(conn);
        return false;
    }
    res = mysql_store_result(conn);
    if (res == nullptr) {
        std::cout << "获取结果集失败!!!" << std::endl;
        m_connPool->ReleaseConnection(conn);
        return false;
    }
    m_connPool->ReleaseConnection(conn);
    return true;
}
    