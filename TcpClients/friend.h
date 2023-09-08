#ifndef FRIEND_H
#define FRIEND_H

#include <QWidget>
#include <QTextEdit>
#include <QListWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include "online.h"


class Friend : public QWidget
{
    Q_OBJECT
public:
    explicit Friend(QWidget *parent = nullptr);
    void showAllOnlineUser(PDU *pdu);
    QString m_strSearchName;
    void updateFriendList(PDU* pdu);

signals:

public slots:
    void showOnline();
    void searchUser();
    void flushFriend();
    void privateChat();

private:
    QTextEdit* m_prShowMsgTE;
    QListWidget* m_pFriendListWidget;
    QLineEdit* m_pInputMsgLE;
    QPushButton* m_DelFriendPB;
    QPushButton* m_pFlushFriendPB;
    QPushButton* m_pShowOnlineUserPB;
    QPushButton* m_pSearchUserPB;
    QPushButton* m_pMsgSendGroupPB;
    QPushButton* m_pSMsgSendPrivatePB;

    Online* m_pOnline;

};

#endif // FRIEND_H
