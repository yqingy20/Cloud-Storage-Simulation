#include "operatedatabase.h"
#include <QMessageBox>
#include <QDebug>

OperateDatabase::OperateDatabase(QObject *parent)
    : QObject{parent}
{
    m_db = QSqlDatabase::addDatabase("QSQLITE");
}

OperateDatabase &OperateDatabase::getInstance() {
    static OperateDatabase instance;
    return instance;
}


void OperateDatabase::init() {
    m_db.setHostName("localhost");
    m_db.setDatabaseName("/Users/yanqingyu/Desktop/Project/TcpServer/cloud.db");
    if(m_db.open()) {
        QSqlQuery query;
        query.exec("select * from userInfo");
        while(query.next()) {
            QString data = QString("%1,%2,%3").arg(query.value(0).toString()).arg(query.value(1).toString()).arg(query.value(2).toString());
            qDebug() << data;
        }
    } else {
        QMessageBox::critical(NULL, "Open the Database", "Fail to Open the Database");
    }
}

OperateDatabase::~OperateDatabase(){
    m_db.close();
}

bool OperateDatabase::handleRegisteration(const char *name, const char *pwd) {
    if (name == NULL || NULL == pwd) {
        return false;
        qDebug() << "name/pwd is NULL";
    }
    QString sql = QString("insert into userInfo(name, pwd) values(\'%1\',\'%2\')").arg(name).arg(pwd);
    qDebug() << sql;
    QSqlQuery query;
    return query.exec(sql);

}

bool OperateDatabase::handleLogin(const char *name, const char *pwd) {
    if (name == NULL || NULL == pwd) {
        qDebug() << "name/pwd is NULL";
        return false;

    }
    QString sql = QString("select * from userInfo where name = \'%1\' and pwd = \'%2\' and isOnline = 0").arg(name).arg(pwd);
    qDebug() << sql;
    QSqlQuery query;
    query.exec(sql);
    if(query.next()) {
        QString sql = QString("update userInfo set isOnline = 1 where name = \'%1\' and pwd = \'%2\'").arg(name).arg(pwd);
//        qDebug() << sql;
        QSqlQuery query;
        query.exec(sql);
        return true;
    } else {
        qDebug() << "Cannot find in the Database" ;
        return false;

    }

}


void OperateDatabase::handleOffline(const char *name) {
    if (name == NULL) {
        qDebug() << "name is NULL";
        return;

    }

    QString sql = QString("update userInfo set isOnline = 0 where name = \'%1\'").arg(name);
    qDebug() << sql;
    QSqlQuery query;
    query.exec(sql);
}

//bool OperateDatabase::handleDelete(const char* name, const char* pwd) {

//}

QStringList OperateDatabase::handleAllOnline() {

    QString sql = QString("select name from UserInfo where isOnline = 1");
    QSqlQuery query;
    query.exec(sql);

    QStringList result;
    result.clear();

    while(query.next()) {
        result.append(query.value(0).toString());
    }

    return result;
}

int OperateDatabase::handleSearchUser(const char *name) {
    if (name == NULL) {
        return -1;      // represent user not found
    }


    QString sql = QString("select isOnline from UserInfo where name = \'%1\'").arg(name);
    QSqlQuery query;
    query.exec(sql);

    if (query.next()) {
        int result = query.value(0).toInt();
        if (result == 1) {
            return 1;
        } else if (result == 0) {
            return 0;
        }
    } else {
        return -1;      // represent user not found
    }

}

int OperateDatabase::handleAddFriend(const char *friname, const char *myname) {
//    if (friname == NULL || myname == NULL) {
//        return -1;              // fail to add friends
//    }

//    QString sql = QString("select * from friend where (id = (select id from userInfo where name = \'%1\') and friendId = (select id from userInfo where name = \'%2\'))"
//                          "or (id = (select id from userInfo where name = \'%3\') and friendId = (select id from userInfo where name = \'%4\'))").arg(friname).arg(myname).arg(myname).arg(friname);

//    qDebug() << sql;

//    QSqlQuery query;
//    query.exec(sql);

//    if (query.next()) {
//        return 0;               // already friends
//    } else {
//        QString sql = QString("select isOnline from UserInfo where name = \'%1\'").arg(friname);
//        QSqlQuery query;
//        query.exec(sql);

//        if (query.next()) {
//            int result = query.value(0).toInt();
//            if (result == 1) {
//                return 1;       // not friends, is online
//            } else if (result == 0) {
//                return 2;       // not friends, not online
//            }
//        } else {
//            return 3;           // person does not exist
//    }
//    }
    if (NULL == friname || NULL == myname)
    {
        return -1;
    }
    QString data = QString("select * from friend where (id = (select id from userInfo where name=\'%1\') and friendId = (select id from userInfo where name=\'%2\')) "
                           "or (id = (select id from userInfo where name=\'%3\') and friendId = (select id from userInfo where name=\'%4\'))").arg(friname).arg(myname).arg(myname).arg(friname);
//    qDebug() << data;
    QSqlQuery query;
    query.exec(data);
    if (query.next())
    {
        return 0;                // already friends
    }
    else
    {
        data = QString("select isOnline from userInfo where name=\'%1\'").arg(friname);
        QSqlQuery query;
        query.exec(data);
        if (query.next())
        {
            int ret = query.value(0).toInt();
            if (1 == ret)
            {
                return 1;   //  not friends, is online
            }
            else if (0 == ret)
            {
                return 2;  //  not friends, not online
            }
        }
        else
        {
            return 3;   //  person does not exist
        }
    }

}

void OperateDatabase::handleAddFriendAgree(const char *friname, const char *myname)
{
    if (NULL == friname || NULL == myname)
    {
        return;
    }
    QString data = QString("insert into friend(id, friendId) values((select id from userInfo where name=\'%1\'), (select id from userInfo where name=\'%2\'))").arg(friname).arg(myname);
    QSqlQuery query;
    query.exec(data);
}

QStringList OperateDatabase::handleFlushFriend(const char* name) {
    QStringList strFriendList;
    strFriendList.clear();
    if (NULL == name)
    {
    return strFriendList;
    }
    QString sql = QString("select name from userInfo where isOnline = 1 and id in (select id from friend where friendId=(select id from userInfo where name=\'%1\'))").arg(name);
    QSqlQuery query;
    query.exec(sql);
    while (query.next())
    {
    strFriendList.append(query.value(0).toString());
    qDebug() << query.value(0).toString();
    }

    sql = QString("select name from userInfo where isOnline = 1 and id in (select friendId from friend where id=(select id from userInfo where name=\'%1\'))").arg(name);
    query.exec(sql);
    while (query.next())
    {
    strFriendList.append(query.value(0).toString());
    qDebug() << query.value(0).toString();
    }
    return strFriendList;
}

