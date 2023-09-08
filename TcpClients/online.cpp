#include "online.h"
#include "ui_online.h"
#include <QDebug>
#include "TcpClients.h"

Online::Online(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Online)
{
    ui->setupUi(this);
}

Online::~Online()
{
    delete ui;
}

void Online::showUser(PDU* pdu) {
    if (pdu == NULL) {
        return;
    }

    uint uiSize = pdu->uiMsgLength / 32;
    char caTemp[32];
    ui->frined_list->clear();
    for (uint i = 0; i < uiSize; i++) {
        memcpy(caTemp, (char*) (pdu->caMsg) + i*32, 32);
//        qDebug()<< caTemp;
        ui->frined_list->addItem(caTemp);
    }
}

void Online::on_addFriend_label_clicked()
{
    QListWidgetItem *pItem = ui->frined_list->currentItem();
//    qDebug() << pItem->text();
    QString strFriUsername = pItem->text();
    QString strMyUsername = TcpClients::getInstance().getLoginName();
    PDU *pdu = mkPDU(0);
    pdu->uiMsgType = ENUM_MSG_TYPE_ADD_FRIEND_REQUEST;
    memcpy(pdu->caData, strFriUsername.toStdString().c_str(), strFriUsername.size());
    memcpy(pdu->caData + 32, strMyUsername.toStdString().c_str(), strMyUsername.size());

    TcpClients::getInstance().getTcpSocket().write((char*)pdu, pdu->uiPDULength);
    free(pdu);
    pdu = NULL;

}

