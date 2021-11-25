#include "sqlite3.h"

//构造函数
// Sqlite3::Sqlite3()
// {

// }
// Sqlite3::~Sqlite3()
// {

// }

/*=======================================================================================
* 函 数 名： Sqlite_read_msg_from_configdb
* 参    数： 
* 功能描述:  从配置数据库读取配置信息
* 返 回 值： 
* 备    注： 
* 作    者： lc
* 创建时间： 2021-07-29 
==========================================================================================*/
configMsgSt Sqlite3::Sqlite_read_msg_from_configdb(void)
{
    configMsgSt pDevMsg;
    
    //打开配置数据库
    QSqlDatabase database;
    database = QSqlDatabase::addDatabase("QSQLITE");
    database.setDatabaseName("/home/meican/base_config.db");
    if (!database.open()) {
        qDebug() << "Error: Failed to connect database." << database.lastError();
    } else {
        qDebug() << "Succeed to connect database.";
    }
    //查询数据
    QSqlQuery query;
    query.exec("select dev_status from config");
    if(!query.exec()) {
        qDebug()<<query.lastError();
    } else {
        //开始就先执行一次next()函数，那么query指向结果集的第一条记录
        if(query.next())
        {
            //获取query所指向的记录在结果集中的编号
            int rowNum = query.at();
            //获取dev_status属性的值
            pDevMsg.devStatus = query.value(0).toInt();
            printf("devstatus = %d \n",pDevMsg.devStatus);
        }
    }
    //查找设备ID
    query.exec("select v2id from config");
    if(!query.exec()) {
        qDebug()<<query.lastError();
    } else {
        //开始就先执行一次next()函数，那么query指向结果集的第一条记录
        if(query.next()) {
            //获取query所指向的记录在结果集中的编号
            int rowNum = query.at();
            //获取设备id
            pDevMsg.devId = query.value(0).toInt();
            printf("dev id = %d \n",pDevMsg.devId);
        }
    }
     //查找菜单等级
    query.exec("select menu_level from config");
    if(!query.exec()) {
        qDebug()<<query.lastError();
    } else {
        //开始就先执行一次next()函数，那么query指向结果集的第一条记录
        if(query.next()) {
            //获取query所指向的记录在结果集中的编号
            int rowNum = query.at();
            //获取设备id
            pDevMsg.toalMenuGrade = query.value(0).toInt();
            printf("toalMenuGrade  = %d \n",pDevMsg.toalMenuGrade);
        }
    }
    //查找设备SN
    query.exec("select sn from config");
    if(!query.exec()) {
        qDebug()<<query.lastError();
    } else {
        //开始就先执行一次next()函数，那么query指向结果集的第一条记录
        if(query.next()) {
            //获取query所指向的记录在结果集中的编号
            int rowNum = query.at();
            //获取dev_status属性的值
            pDevMsg.sn = query.value(0).toString();
            qDebug("dev sn = " + pDevMsg.sn.toLatin1());
        }
    }

    //查找设备硬件版本
    query.exec("select har_ver from config");
    if(!query.exec()) {
        qDebug()<<query.lastError();
    } else {
        //开始就先执行一次next()函数，那么query指向结果集的第一条记录
        if(query.next()) {
            //获取query所指向的记录在结果集中的编号
            int rowNum = query.at();
            //获取dev_status属性的值
            pDevMsg.har_ver = query.value(0).toString();
            qDebug("dev har_ver = " + pDevMsg.har_ver.toLatin1());
        }
    }

    //查找设备软件版本
    query.exec("select soft_ver from config");
    if(!query.exec()) {
        qDebug()<<query.lastError();
    } else {
        //开始就先执行一次next()函数，那么query指向结果集的第一条记录
        if(query.next()) {
            //获取query所指向的记录在结果集中的编号
            int rowNum = query.at();
            //获取dev_status属性的值
            pDevMsg.soft_ver = query.value(0).toString();
            qDebug("dev soft_ver = " + pDevMsg.soft_ver.toLatin1());
        }
    }

    //查找菜单版本
    query.exec("select menu_ver from config");
    if(!query.exec()) {
        qDebug()<<query.lastError();
    } else {
        //开始就先执行一次next()函数，那么query指向结果集的第一条记录
        if(query.next()) {
            //获取query所指向的记录在结果集中的编号
            int rowNum = query.at();
            //获取设备menu_ver
            pDevMsg.menu_ver = query.value(0).toLongLong();
            printf("dev menu_ver = %lld \n",pDevMsg.menu_ver);
        }
    }

    //查找固件更新结果
    query.exec("select otaResult from config");
    if(!query.exec()) {
        qDebug()<<query.lastError();
    } else {
        //开始就先执行一次next()函数，那么query指向结果集的第一条记录
        if(query.next()) {
            //获取query所指向的记录在结果集中的编号
            int rowNum = query.at();
            //获取设备menu_ver
            pDevMsg.otaResult = query.value(0).toString();
            qDebug("otaResult = " + pDevMsg.otaResult.toLatin1());
        }
    }

    //关闭数据库
    database.close();
    return pDevMsg;
}

/*=======================================================================================
* 函 数 名： Sqlite_update_ota_result
* 参    数： 
* 功能描述:  修改配置数据库ota更新状态
* 返 回 值： 
* 备    注： 
* 作    者： lc
* 创建时间： 2021-07-29 
==========================================================================================*/
void Sqlite3::Sqlite_update_ota_result(void)
{
    configMsgSt pDevMsg;

    //打开配置数据库
    QSqlDatabase database;
    database = QSqlDatabase::addDatabase("QSQLITE");
    database.setDatabaseName("/home/meican/base_config.db");
    if (!database.open()) {
        qDebug() << "Error: Failed to connect database." << database.lastError();
    } else {
        qDebug() << "Succeed to connect database.";
    }
    //修改ota_result数据
    QSqlQuery query;
    query.exec("update config set otaResult=''");
    if(!query.exec()) {
        qDebug()<<query.lastError();
    } else {
    }
    //关闭数据库
    database.close();
}

/*=======================================================================================
* 函 数 名： Sqlite_read_context_from_menudb
* 参    数： 
* 功能描述:  从菜单数据库读取菜品信息列
* 返 回 值： 
* 备    注： 
* 作    者： lc
* 创建时间： 2021-07-29
==========================================================================================*/
QStringList Sqlite3::Sqlite_read_context_from_menudb(QString menuname)
{
    QStringList tempList;
    QSqlDatabase database;

    
    database = QSqlDatabase::addDatabase("QSQLITE");
    QString str = "/home/meican/menudb/";
    str.append(menuname);
    str.append(".db");
    qDebug("str = "+ str.toLatin1());
    database.setDatabaseName(str);
    if (!database.open()) {
        qDebug() << "Error: Failed to connect menu database" << database.lastError();
    } else {
        qDebug() << "Succeed to connect menu database";
    }
    //查询数据
    QSqlQuery query;
    query.exec("select context from menu");
    QSqlRecord recode = query.record();		//recode保存查询到一些内容信息，如表头、列数等等
    int column = recode.count();			//获取读取结果的列数	
    QString s1 = recode.fieldName(0);		//获取第0列的列名

    while (query.next())
    {
        QString context;
	    context = query.value("context").toString();
        tempList << context;
        //qDebug() << context;
    }
    //关闭数据库
    database.close();
    return tempList;
}

/*=======================================================================================
* 函 数 名： Sqlite_read_spell_from_menudb
* 参    数： 
* 功能描述:  从菜单数据库读取菜品首字母列
* 返 回 值： 
* 备    注： 
* 作    者： lc
* 创建时间： 2021-07-29
==========================================================================================*/
QStringList Sqlite3::Sqlite_read_spell_from_menudb(QString menuname)
{
    QStringList tempList;
    QSqlDatabase database;

    database = QSqlDatabase::addDatabase("QSQLITE");
    QString str = "/home/meican/menudb/";
    str.append(menuname);
    str.append(".db");
    qDebug("str = "+ str.toLatin1());
    database.setDatabaseName(str);
    if (!database.open()) {
        qDebug() << "Error: Failed to connect menu database" << database.lastError();
    } else {
        qDebug() << "Succeed to connect menu database";
    }
    //查询数据
    QSqlQuery query;
    query.exec("select spell from menu");
    QSqlRecord recode = query.record();		//recode保存查询到一些内容信息，如表头、列数等等
    int column = recode.count();			//获取读取结果的列数	
    QString s1 = recode.fieldName(0);		//获取第0列的列名

    while (query.next())
    {
        QString context;
	    context = query.value("spell").toString();
        tempList << context;
        //qDebug() << context;
    }
    //关闭数据库
    database.close();
    return tempList;
}

/*=======================================================================================
* 函 数 名： Sqlite_read_menu_from_menudb
* 参    数： 
* 功能描述:  按照菜单名字，从菜单数据库读取菜品信息
* 返 回 值： 
* 备    注： 
* 作    者： lc
* 创建时间： 2021-07-29
==========================================================================================*/
menuSt Sqlite3::Sqlite_read_menu_from_menudb(QString menuname, QString contextstr)
{
    menuSt pMenu;
    QSqlDatabase database;

    database = QSqlDatabase::addDatabase("QSQLITE");
    QString str = "/home/meican/menudb/";
    str.append(menuname);
    str.append(".db");
    qDebug("str = "+ str.toLatin1());
    database.setDatabaseName(str);
    if (!database.open()) {
        qDebug() << "Error: Failed to connect menu database" << database.lastError();
    } else {
        qDebug() << "Succeed to connect menu database";
    }
    //查询数据
    QSqlQuery query;
    str = "select * from menu where context = '";
    str.append(contextstr);
    str.append("'");
    qDebug() << str;

    query.exec(str);
    while (query.next()) 
    {
        pMenu.id       = query.value("id").toInt(); 
        pMenu.premenu  = query.value("premenu").toString();
        pMenu.nextmenu = query.value("nextmenu").toString();
        pMenu.grade    = query.value("grade").toInt(); 
        pMenu.context  = query.value("context").toString();
        pMenu.dish_id  = query.value("dish_id").toInt();
        if (pMenu.id) {
            break;
        }
    };
    //关闭数据库
    database.close();
    return pMenu;
}

/*=======================================================================================
* 函 数 名： Sqlite_read_premenu_id_from_menudb
* 参    数： 
* 功能描述:  根据premenu 回读菜单id，用于返回前菜单保存显示
* 返 回 值： 
* 备    注： 
* 作    者： lc
* 创建时间： 2021-07-29
==========================================================================================*/
int Sqlite3::Sqlite_read_premenu_id_from_menudb(QString menuname, QString premenu)
{
    int id;
    QSqlDatabase database;

    database = QSqlDatabase::addDatabase("QSQLITE");
    QString str = "/home/meican/menudb/";
    str.append(menuname);
    str.append(".db");
    qDebug("str = "+ str.toLatin1());
    database.setDatabaseName(str);
    if (!database.open()) {
        qDebug() << "Error: Failed to connect menu database" << database.lastError();
    } else {
        qDebug() << "Succeed to connect menu database";
    }
    //查询数据
    QSqlQuery query;
    str = "select * from menu where nextmenu = '";
    str.append(premenu);
    str.append("'");
    qDebug() << str;

    query.exec(str);
    while (query.next()) 
    {
        id = query.value("id").toInt(); 
        if (id) {
            break;
        }     
    };
    //关闭数据库
    database.close();
    return id;
}

/*=======================================================================================
* 函 数 名： Sqlite_read_nextmenu_context_from_menudb
* 参    数： 
* 功能描述:  根据premenu 回读菜单id，用于返回前菜单保存显示
* 返 回 值： 
* 备    注： 
* 作    者： lc
* 创建时间： 2021-07-29
==========================================================================================*/
QString Sqlite3::Sqlite_read_nextmenu_context_from_menudb(QString menuname, QString premenu)
{
    QString precontext;
    QSqlDatabase database;

    database = QSqlDatabase::addDatabase("QSQLITE");
    QString str = "/home/meican/menudb/";
    str.append(menuname);
    str.append(".db");
    qDebug("str = "+ str.toLatin1());
    database.setDatabaseName(str);
    if (!database.open()) {
        qDebug() << "Error: Failed to connect menu database" << database.lastError();
    } else {
        qDebug() << "Succeed to connect menu database";
    }
    //查询数据
    QSqlQuery query;
    str = "select * from menu where nextmenu = '";
    str.append(premenu);
    str.append("'");
    qDebug() << str;

    query.exec(str);
    while (query.next()) 
    {
        precontext = query.value("context").toString(); 
        int id = query.value("id").toInt();   
        if (id) {
            break;
        } 
    };
    //关闭数据库
    database.close();
    return precontext;
}

/*=======================================================================================
* 函 数 名： Sqlite_read_grade_from_menudb
* 参    数： 
* 功能描述:  从菜单数据库读取菜单等级
* 返 回 值： 
* 备    注： 
* 作    者： lc
* 创建时间： 2021-07-29
==========================================================================================*/
int Sqlite3::Sqlite_read_grade_from_menudb(QString menuname)
{
    int grade = 0;
    QSqlDatabase database;

    database = QSqlDatabase::addDatabase("QSQLITE");
    QString str = "/home/meican/menudb/";
    str.append(menuname);
    str.append(".db");
    //qDebug("str = "+ str.toLatin1());
    database.setDatabaseName(str);
    if (!database.open()) {
        //qDebug() << "Error: Failed to connect menu database" << database.lastError();
    } else {
        //qDebug() << "Succeed to connect menu database" ;
    }
    //查询数据
    QSqlQuery query;
    query.exec("select grade from menu where id = 1");
    while (query.next()) 
    {
        grade = query.value("grade").toInt(); 
        //qDebug() << "grade = "<< grade;
        if (grade) {
            break;
        }     
    }
    //关闭数据库
    database.close();
    return grade;
}

/*=======================================================================================
* 函 数 名： Sqlite_update_process_db_run
* 参    数： 
* 功能描述:  修改进程数据库运行标记
* 返 回 值： 
* 备    注： 
* 作    者： lc
* 创建时间： 2021-07-29 
==========================================================================================*/
void Sqlite3::Sqlite_update_process_db_run(void)
{
    //打开配置数据库
    QSqlDatabase database;
    database = QSqlDatabase::addDatabase("QSQLITE");
    database.setDatabaseName("/home/meican/process_protection.db");
    if (!database.open()) {
        qDebug() << "Error: Failed to connect database." << database.lastError();
    } else {
        qDebug() << "Succeed to connect database.";
    }
    //修改ota_result数据
    QSqlQuery query;
    query.exec("update process_protection_db set qt_run ='ok'");
    if(!query.exec()) {
        qDebug()<<query.lastError();
    } else {
    }
    //关闭数据库
    database.close();
}
