#ifndef OPERATEDATABASE_H
#define OPERATEDATABASE_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QStringList>

class OperateDatabase : public QObject
{
    Q_OBJECT
public:
    explicit OperateDatabase(QObject *parent = nullptr);
    static OperateDatabase& getInstance();

    void init();
    ~OperateDatabase();

    bool handleRegisteration(const char *name, const char *pwd);
    bool handleLogin(const char *name, const char *pwd);
    void handleOffline(const char *name);
//    bool handleDelete(const char* name, const char* pwd);
    QStringList handleAllOnline();
    int handleSearchUser(const char *name);
    int handleAddFriend(const char *friname, const char *myname);
    void handleAddFriendAgree(const char *friname, const char *myname);
    QStringList handleFlushFriend(const char* name);

signals:

public slots:
private:
    QSqlDatabase m_db;          // to connect to the database
};

#endif // OPERATEDATABASE_H
