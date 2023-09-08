#include "mytcpserver.h"
#include <QDebug>
#include <stdio.h>
#include "mytcpserver.h"

MyTcpServer::MyTcpServer()
{

}


MyTcpServer &MyTcpServer::getInstanceObject(){
    static MyTcpServer tcp_instance;
    return tcp_instance;
}

void MyTcpServer::incomingConnection(qintptr socketDescriptor){

    qDebug() << "New Client Connected";

    MyTcpSocket *pTcpSocket = new MyTcpSocket;
    pTcpSocket->setSocketDescriptor(socketDescriptor);
    m_tcpSocketList.append(pTcpSocket);

    connect(pTcpSocket, SIGNAL(offline(MyTcpSocket*)), this, SLOT(deleteSocket(MyTcpSocket*)));
}

void MyTcpServer::deleteSocket(MyTcpSocket* mysocket) {
    QList<MyTcpSocket*>::iterator iter = m_tcpSocketList.begin();
    for(; iter != m_tcpSocketList.end(); iter++) {
        if (mysocket == *iter) {
            delete *iter;
            *iter = NULL;
            m_tcpSocketList.erase(iter);
            break;
        }
    }

    for (int i = 0; i < m_tcpSocketList.size(); i++) {
        qDebug() << m_tcpSocketList.at(i)->getName();
    }
}

void MyTcpServer::share(const char* friname, PDU* pdu) {
//    if (friname == NULL || pdu == NULL) {
//        return;
//    }

//    QString strName = friname;

//    for (int i = 0; i < m_tcpSocketList.size(); i++) {
//        if (strName == m_tcpSocketList.at(i)->getName()) {
////            pdu->uiMsgType = ENUM_MSG_TYPE_ADD_FRIEND_REQUEST;
//            m_tcpSocketList.at(i)->write((char*) pdu, pdu->uiMsgLength);
//            qDebug() << "reached mytcpserver";
//            break;
//        }
//    }
    if (NULL == friname || NULL == pdu)
    {
        return;
    }
    QString strName = friname;
    for (int i=0; i<m_tcpSocketList.size(); i++)
    {
        if (strName == m_tcpSocketList.at(i)->getName())
        {
            m_tcpSocketList.at(i)->write((char*)pdu, pdu->uiPDULength);
            break;
        }
    }

}
