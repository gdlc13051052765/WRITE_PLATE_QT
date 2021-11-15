#ifndef SQLITE3_H__
#define SQLITE3_H__

#include <stdlib.h>
#include <qdebug.h>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QtSql/QSqlRecord>

//配置信息结构体
struct configMsgSt
{
    int devStatus;//设备状态
    int devId;//设备ID，用于出厂二维码显示
    QString sn;//设备sn码
    long long int menu_ver;//菜单版本
    QString har_ver;//硬件版本
    QString soft_ver;//软件版本
    QString otaResult;//固件更新结果
};
//菜单信息结构体
struct menuSt
{
    int id;//菜品序号
    QString premenu;//上级菜单
    QString nextmenu;//下级菜单
    int grade;//菜单等级
    QString context;//菜品名称
    int dish_id;//菜品id
};

// //菜品 list
// struct menuList
// {
//     QStringList name;//菜品名称
//     QStringList spell;//菜品首字母

// }
//写盘器数据库读取接口函数
class Sqlite3
{
    public:
        configMsgSt Sqlite_read_msg_from_configdb(void);//配置数据库读取信息
        void Sqlite_update_ota_result(void);
        QStringList Sqlite_read_context_from_menudb(QString menuname);//从菜单数据库读取菜单名称列全部信息
        QStringList Sqlite_read_spell_from_menudb(QString menuname);//从菜单数据库读取首字母列全部信息
        menuSt Sqlite_read_menu_from_menudb(QString menuname, QString contextstr);//从菜单数据库读取菜单行信息
        int Sqlite_read_premenu_id_from_menudb(QString menuname, QString premenu);
        int Sqlite_read_grade_from_menudb(QString menuname);//菜单数据库读取菜单等级
        QString Sqlite_read_nextmenu_context_from_menudb(QString menuname, QString contextstr);//根据数据库名字读取前一级数据库的菜单名字
        void Sqlite_update_process_db_run();//修改进程运行数据库运行标记位
};

#endif // SQLITE3_H__
