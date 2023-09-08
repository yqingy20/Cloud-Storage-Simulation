#include "privatechat.h"
#include "ui_privatechat.h"
#include "protocol.h"
#include "tcpclients.h"
#include <QMessageBox>

PrivateChat::PrivateChat(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PrivateChat)
{
    ui->setupUi(this);
}

PrivateChat::~PrivateChat()
{
    delete ui;
}

void PrivateChat::on_sendButton_clicked()
{
    QString strMsg = ui->inputMessage->text();
    if (!strMsg.isEmpty()) {
        PDU *pdu = mkPDU(strMsg.size() + 1);
        pdu->uiMsgType = ENUM_MSG_TYPE_PRIVATE_CHAT_REQUEST;
        memcpy(pdu->caData, m_strLoginName.toStdString().c_str(), m_strLoginName.size());
        memcpy(pdu->caData + 32, m_strChatName.toStdString().c_str(), m_strLoginName.size());

        strcpy((char*) pdu->caMsg, strMsg.toStdString().c_str());

        TcpClients::getInstance().getTcpSocket().write((char*) pdu, pdu->uiPDULength);
        free(pdu);
        pdu = NULL;
    } else {
        QMessageBox::warning(this, "Private Chat", "Message sent can't be null");
    }
}

void PrivateChat::setChatName(QString strName) {
    m_strChatName = strName;
    m_strLoginName = TcpClients::getInstance().getLoginName();
}

PrivateChat &PrivateChat::getInstance() {
    static PrivateChat instance;
    return instance;
}

void PrivateChat::updateMessage(const PDU* pdu) {
    if (NULL == pdu)
    {
        return;
    }
    char caSenderName[32] = {'\0'};
    memcpy(caSenderName, pdu->caData, 32);
    QString strMsg = QString("%1: %2").arg(caSenderName).arg((char*)(pdu->caMsg));
    ui->showMessage->append(strMsg);
}

