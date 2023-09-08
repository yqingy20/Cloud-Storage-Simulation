#include "friend.h"
#include "protocol.h"
#include "tcpclients.h"
#include <QInputDialog>
#include <QDebug>
#include "privatechat.h"
#include <QMessageBox>

Friend::Friend(QWidget *parent)
    : QWidget{parent}
{

    m_prShowMsgTE = new QTextEdit;
    m_pFriendListWidget = new QListWidget;

    m_pInputMsgLE = new QLineEdit;

    m_DelFriendPB = new QPushButton("Delete friend");
    m_pFlushFriendPB = new QPushButton("Refresh friend");
    m_pShowOnlineUserPB = new QPushButton("Show online user");
    m_pSearchUserPB = new QPushButton("Search user");
    m_pMsgSendGroupPB = new QPushButton("Group chat");
    m_pSMsgSendPrivatePB = new QPushButton("Private chat");

    m_pOnline = new Online;

    QVBoxLayout *pRightPBVBL = new QVBoxLayout;
    pRightPBVBL->addWidget(m_DelFriendPB);
    pRightPBVBL->addWidget(m_pFlushFriendPB);
    pRightPBVBL->addWidget(m_pShowOnlineUserPB);
    pRightPBVBL->addWidget(m_pSearchUserPB);
    pRightPBVBL->addWidget(m_pSMsgSendPrivatePB);
//    pRightPBVBL->addWidget(m_pOnline);

    QHBoxLayout *pTOPHBL = new QHBoxLayout;
    pTOPHBL->addWidget(m_prShowMsgTE);
    pTOPHBL->addWidget(m_pFriendListWidget);
//    pTOPHBL->addWidget(m_pOnline);
    pTOPHBL->addLayout(pRightPBVBL);

    QHBoxLayout *pMsgHBL = new QHBoxLayout;
    pMsgHBL->addWidget(m_pInputMsgLE);
    pMsgHBL->addWidget(m_pMsgSendGroupPB);

//    QHBoxLayout *pOnline = new QHBoxLayout;
//    pOnline->addWidget(m_pOnline);

    QVBoxLayout *pMain = new QVBoxLayout;
    pMain->addLayout(pTOPHBL);
    pMain->addLayout(pMsgHBL);
    pMain->addWidget(m_pOnline);
    m_pOnline->hide();
//    pMain->addLayout(pOnline);
//    pOnline->hide()


    setLayout(pMain);

    connect(m_pShowOnlineUserPB, SIGNAL(clicked(bool)), this, SLOT(showOnline()));

    connect(m_pSearchUserPB, SIGNAL(clicked(bool)), this, SLOT(searchUser()));

    connect(m_pFlushFriendPB, SIGNAL(clicked(bool)), this, SLOT(flushFriend()));

    connect(m_pSMsgSendPrivatePB, SIGNAL(clicked(bool)), this, SLOT(privateChat()) );



}

void Friend::showOnline() {
    if (m_pOnline->isHidden()) {
        m_pOnline->show();

        PDU *pdu = mkPDU(0);
        pdu->uiMsgType = ENUM_MSG_TYPE_ALL_ONLINE_REQUEST;
        TcpClients::getInstance().getTcpSocket().write((char*)pdu, pdu->uiPDULength);
        free(pdu);
        pdu = NULL;

    } else {
        m_pOnline->hide();
    }
}

void Friend::showAllOnlineUser(PDU* pdu) {
    if (pdu == NULL) {
        return;
    }
    m_pOnline->showUser(pdu);



}

void Friend::searchUser() {
    m_strSearchName = QInputDialog::getText(this, "Search User", "Username: ");
    if(!m_strSearchName.isEmpty()) {
        qDebug() << m_strSearchName;
        PDU *pdu = mkPDU(0);
        memcpy(pdu->caData, m_strSearchName.toStdString().c_str(), m_strSearchName.size());
        pdu->uiMsgType = ENUM_MSG_TYPE_ALL_SEARCH_USER_REQUEST;
        TcpClients::getInstance().getTcpSocket().write((char*)pdu, pdu->uiPDULength);
        free(pdu);
        pdu = NULL;
    }
}

void Friend::flushFriend() {
    m_pFriendListWidget->clear();

    QString strName = TcpClients::getInstance().getLoginName();
    PDU *pdu = mkPDU(0);
    pdu->uiMsgType = ENUM_MSG_TYPE_FLUSH_FRIEND_REQUEST;
    memcpy(pdu->caData, strName.toStdString().c_str(), strName.size());
    TcpClients::getInstance().getTcpSocket().write((char*)pdu, pdu->uiPDULength);
    free(pdu);
    pdu = NULL;

}

void Friend::privateChat() {
    if (m_pFriendListWidget->currentItem() != NULL) {
        QString strChatName = m_pFriendListWidget->currentItem()->text();
        PrivateChat::getInstance().setChatName(strChatName);
        if (PrivateChat::getInstance().isHidden()) {
            PrivateChat::getInstance().show();
        }
    } else {
        QMessageBox::warning(this, "Private Chat", "Please choose a friend to start chat");
    }
}

void Friend::updateFriendList(PDU* pdu){
    if (pdu == NULL)
    {
        return;
    }
    uint uiSize = pdu->uiMsgLength / 32;
    char caName[32] = {'\0'};
    for (uint i=0; i<uiSize; i++)
    {
        memcpy(caName, (char*)(pdu->caMsg)+i*32, 32);
        m_pFriendListWidget->addItem(caName);
    }
}
