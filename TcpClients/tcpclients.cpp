#include "tcpclients.h"
#include "ui_tcpclients.h"
#include <QByteArray>
#include <QDebug>
#include <QMessageBox>
#include <QHostAddress>
#include <QFile>
#include "privatechat.h"

TcpClients::TcpClients(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::TcpClients)
{
    ui->setupUi(this);

    resize(500, 300);

    loadConfig();

    // m_tcpSocket: sending the signal
    // connected(): what signal is sending
    // this: receiving the signal
    // showConnection(): what signal is receiving
    connect(&m_tcpSocket, SIGNAL(connected()), this, SLOT(showConnection()));
    connect(&m_tcpSocket, SIGNAL(readyRead()), this, SLOT(receMsg()));

    // connecting to the host
    m_tcpSocket.connectToHost(QHostAddress(m_strIP), m_usPort);
}

TcpClients::~TcpClients()
{
    delete ui;
}

void TcpClients::loadConfig() {
    QFile file(":/client.config");
    if (file.open(QIODevice::ReadOnly)) {
        QByteArray baData = file.readAll();
        QString strData = baData.toStdString().c_str();
//        qDebug() << strData;
        strData.replace("\n", " ");
//        qDebug() << strData;
        QStringList strList = strData.split(" ");
//        for (int i = 0; i < strList.size(); i++) {
//            qDebug() << " ....> " << strList[i];
//        }
        m_strIP = strList.at(0);
        m_usPort = strList.at(1).toUShort();
//        qDebug() << "ip is: " << m_strIP << "port is: " << m_usPort;



        file.close();
    } else {
        QMessageBox::critical(this, "Open Config", "Fail to open Config");
    }
}

void TcpClients::showConnection(){
    QMessageBox::information(this, "Connecting to the host", "Successfully connect to the host");
}


//void TcpClients::on_pushButton_clicked()
//{
//    QString strMsg = ui->lineEdit->text();
//    if (!strMsg.isEmpty()) {

//        PDU *pdu = mkPDU(strMsg.size() + 1);
//        pdu->uiMsgType = 8888;

//        memcpy(pdu->caMsg, strMsg.toStdString().c_str(), strMsg.size());

//        m_tcpSocket.write((char*)pdu, pdu->uiPDULength);

//        free(pdu);

//        pdu = NULL;

//    } else {
//        QMessageBox::warning(this, "Message Sending", "Message can't be null");
//    }

//}

void TcpClients::receMsg() {
    qDebug() << m_tcpSocket.bytesAvailable();
    uint uiPDULen = 0;
    // first four types will give the size of this sending
    m_tcpSocket.read((char*)&uiPDULen, sizeof(uint));
    // acutal size of the sending
    uint uiMsgLen = uiPDULen - sizeof(PDU);
    PDU *pdu = mkPDU(uiMsgLen);
    m_tcpSocket.read((char*)pdu + sizeof(uint), uiPDULen - sizeof(uint));
    //    qDebug() << pdu->uiMsgType << (char*) (pdu->caMsg);

    switch (pdu->uiMsgType) {
    case ENUM_MSG_TYPE_REGISTER_RESPOND: {
        if (strcmp(pdu->caData, REGISTER_S) == 0) {
            QMessageBox::information(this, "Sign up", REGISTER_S);
        } else if (strcmp(pdu->caData, REGISTER_F) == 0) {
            QMessageBox::warning(this, "Sign up", REGISTER_F);
        }
        break;
    }
    case ENUM_MSG_TYPE_LOGIN_RESPOND: {
        if (strcmp(pdu->caData, LOGIN_S) == 0) {
            m_strCurrentPath = QString("/Users/yanqingyu/Desktop/Project/AllFolders/%1").arg(m_strLoginName);
            QMessageBox::information(this, "Log in", LOGIN_S);
            OpeWidget::getInstance().show();
            this->hide();
        } else if (strcmp(pdu->caData, LOGIN_F) == 0) {
            QMessageBox::warning(this, "Log in", LOGIN_F);
        }
        break;

    case ENUM_MSG_TYPE_ALL_ONLINE_RESPOND: {
        OpeWidget::getInstance().getFriend()->showAllOnlineUser(pdu);
        break;
    }
    }

    case ENUM_MSG_TYPE_LOGOFF_RESPOND: {
        QMessageBox::information(this, "Log off", pdu->caData);
        break;
    }

    case ENUM_MSG_TYPE_ALL_SEARCH_USER_RESPOND: {
        if (strcmp(SEARCH_USER_NOT_FOUND, pdu->caData) == 0) {
            QMessageBox::information(this,
                                     "Search User",
                                     QString("%1: User not found").arg(OpeWidget::getInstance().getFriend()->m_strSearchName));
        } else if (strcmp(SEARCH_USER_ONLINE, pdu->caData) == 0) {
            QMessageBox::information(this,
                                     "Search User",
                                     QString("%1: User is online").arg(OpeWidget::getInstance().getFriend()->m_strSearchName));
        } else if (strcmp(SEARCH_USER_OFFLINE, pdu->caData) == 0) {
            QMessageBox::information(this,
                                     "Search User",
                                     QString("%1: User is offline").arg(OpeWidget::getInstance().getFriend()->m_strSearchName));
        }

        break;
    }

    case ENUM_MSG_TYPE_ADD_FRIEND_REQUEST: {
        qDebug() << "reached friend's client";
//        char caName[32] = {'\0'};
//        strncpy(caName, pdu->caData + 32, 32);
//        int temp = QMessageBox::information(this, "Add Friend", QString("%1 want to add you as friend").arg(caName),
//                                            QMessageBox::Yes, QMessageBox::No);
//        PDU* respond_pdu = mkPDU(0);
//        memcpy(respond_pdu->caData, pdu->caData, 32);

//        if (QMessageBox::Yes == temp) {
//            respond_pdu->uiMsgType = ENUM_MSG_TYPE_ADD_FRIEND_AGGREE;
//        } else {
//            respond_pdu->uiMsgType = ENUM_MSG_TYPE_ADD_FRIEND_REFUSE;
//        }
//        m_tcpSocket.write((char*) respond_pdu, respond_pdu->uiPDULength);
//        free(respond_pdu);
//        respond_pdu = NULL;
//        break;
        char caName[32] = {'\0'};
        strncpy(caName, pdu->caData+32, 32);
        int ret = QMessageBox::information(this, "Add Friend", QString("%1 is sending you a friend request").arg(caName)
                                           , QMessageBox::Yes, QMessageBox::No);
        PDU *respdu = mkPDU(0);
        memcpy(respdu->caData, pdu->caData, 64);
        if (QMessageBox::Yes == ret)
        {
            respdu->uiMsgType = ENUM_MSG_TYPE_ADD_FRIEND_AGGREE;
        }
        else
        {
            respdu->uiMsgType = ENUM_MSG_TYPE_ADD_FRIEND_REFUSE;
        }
        m_tcpSocket.write((char*)respdu, respdu->uiPDULength);
        free(respdu);
        respdu = NULL;
        break;
    }
    case ENUM_MSG_TYPE_ADD_FRIEND_RESPOND: {
        QMessageBox::information(this, "Add Friends", pdu->caData);
        break;
    }

    case ENUM_MSG_TYPE_ADD_FRIEND_AGGREE: {
        char caFriName[32] = {'\0'};
        memcpy(caFriName, pdu->caData, 32);
        QMessageBox::information(this, "Add Friends", QString("Successfully adding %1 as friend").arg(caFriName));
        break;
    }

    case ENUM_MSG_TYPE_ADD_FRIEND_REFUSE: {
        char caFriName[32] = {'\0'};
        memcpy(caFriName, pdu->caData, 32);
        QMessageBox::information(this, "Add Friends", QString("Fail to add %1 as friend").arg(caFriName));
        break;
    }

    case ENUM_MSG_TYPE_FLUSH_FRIEND_RESPOND: {
        OpeWidget::getInstance().getFriend()->updateFriendList(pdu);
        break;
    }

    case ENUM_MSG_TYPE_PRIVATE_CHAT_REQUEST: {
        if (PrivateChat::getInstance().isHidden()) {
            PrivateChat::getInstance().show();
        }
        char caSenderName[32] = {'\0'};
        memcpy(caSenderName, pdu->caData, 32);
        QString strSenderName = caSenderName;
        PrivateChat::getInstance().setChatName(strSenderName);
        PrivateChat::getInstance().updateMessage(pdu);

    }

    case ENUM_MSG_TYPE_CREATE_DIR_RESPOND: {
        qDebug() << "reached!";
        QMessageBox::information(this, "Create Folder", pdu->caData);
        break;
    }

    case ENUM_MSG_TYPE_REFRESH_FILE_RESPOND: {
        OpeWidget::getInstance().getBook()->updateFileList(pdu);
        break;
    }

    case ENUM_MSG_TYPE_UPLOAD_FILE_RESPOND: {
        QMessageBox::information(this, "upload file", pdu->caData);
        break;
    }
    case ENUM_MSG_TYPE_DOWNLOAD_FILE_RESPOND: {
        qDebug() << pdu->caData;
        char caFileName[32] = {'\0'};
        sscanf(pdu->caData, "%s %lld", caFileName, &(OpeWidget::getInstance().getBook()->m_iTotal));
        if (strlen(caFileName) > 0 && OpeWidget::getInstance().getBook()->m_iTotal > 0)
        {
            OpeWidget::getInstance().getBook()->setDownloadStatus(true);
            m_file.setFileName(OpeWidget::getInstance().getBook()->getSaveFilePath());
            if (!m_file.open(QIODevice::WriteOnly))
            {
                QMessageBox::warning(this, "Download", "fail to download");
            }
        }
        break;
    }

    default:
        break;
    }

    free(pdu);
    pdu = NULL;



}

TcpClients &TcpClients::getInstance() {
    static TcpClients instance;
    return instance;
}

QTcpSocket &TcpClients::getTcpSocket() {
    return m_tcpSocket;
}

void TcpClients::on_login_clicked()
{
    QString strName = ui->username_text->text();
    QString strPwd = ui->password_text->text();
    if (!strName.isEmpty() && !strPwd.isEmpty()) {

        m_strLoginName = strName;

        PDU *pdu = mkPDU(0);
        pdu->uiMsgType = ENUM_MSG_TYPE_LOGIN_REQUEST;

        // both userename and password is in the caData
        // first 32 bytes is username
        // last 32 bytes is password
        strncpy(pdu->caData, strName.toStdString().c_str(), 32);
        strncpy(pdu->caData + 32, strPwd.toStdString().c_str(), 32);
        m_tcpSocket.write((char*)pdu, pdu->uiPDULength);
        free(pdu);
        pdu = NULL;

    } else {
        QMessageBox::critical(this, "Log in", "Fail to Log in: Username/Password can't be Null");
    }
}


void TcpClients::on_signup_clicked()
{
    QString strName = ui->username_text->text();
    QString strPwd = ui->password_text->text();
    if (!strName.isEmpty() && !strPwd.isEmpty()) {

        PDU *pdu = mkPDU(0);
        pdu->uiMsgType = ENUM_MSG_TYPE_REGISTER_REQUEST;

        // both userename and password is in the caData
        // first 32 bytes is username
        // last 32 bytes is password
        strncpy(pdu->caData, strName.toStdString().c_str(), 32);
        strncpy(pdu->caData + 32, strPwd.toStdString().c_str(), 32);
        m_tcpSocket.write((char*)pdu, pdu->uiPDULength);
        free(pdu);
        pdu = NULL;

    } else {
        QMessageBox::critical(this, "Sign up", "Fail to Register: Username/Password can't be Null");
    }
}


void TcpClients::on_logoff_clicked()
{
    QString strName = ui->username_text->text();
    QString strPwd = ui->password_text->text();
    if(!strName.isEmpty()&& !strPwd.isEmpty())
    {
        PDU *pdu = mkPDU(0);
        pdu->uiMsgType = ENUM_MSG_TYPE_LOGOFF_REQUEST;
        strncpy(pdu->caData,strName.toStdString().c_str(),32);   // 数组前32位放用户名
        strncpy(pdu->caData+32,strPwd.toStdString().c_str(),32); // 数组后32位放密码
//        qDebug() << pdu->caData;
        m_tcpSocket.write((char*)pdu,pdu->uiPDULength);
        free(pdu);
        pdu = NULL;
    }else{
        QMessageBox::critical(this,"Log off","Log off failed ");
    }
}

QString TcpClients::getLoginName() {
    return m_strLoginName;
}

QString TcpClients::getCurrentPath() {
    return m_strCurrentPath;
}

