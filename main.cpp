#include <Programs/cproject.h>
#include <Programs/chttpserver.h>
#include <Managers/cglobalmanager.h>
#include <Managers/cdatabasemanager.h>

int main(int argc, char argv[])
{
    std::cout << "Hello, World!" << std::endl;
    CHttpServer server(8080, 30);
    server.start();
    CGlobalManager::GetInstance()->CreateProject();
    // MYSQL_RES* res = nullptr;
    // CDatabaseManager::GetInstance()->GetUsrInfo("lizhenyu", res);
    // // 获取字段数量
    // int num_fields = mysql_num_fields(res);

    // // 获取字段名
    // MYSQL_FIELD* fields = mysql_fetch_fields(res);
    // // 遍历结果集
    // MYSQL_ROW row;
    // while ((row = mysql_fetch_row(res))) {
    //     std::map<std::string, std::string> rowData;
    //     for (int i = 0; i < num_fields; ++i) {
    //         std::cout << fields[i].name << " : " <<  row[i] << std::endl;
    //     }
    // }

    // // while ((row = mysql_fetch_row(res))) {
    // //     std::cout << row[0] << " " << row[1] << " " << row[2] << std::endl;
    // // }
    // mysql_free_result(res);
    while (true)
    {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    return 0;
}