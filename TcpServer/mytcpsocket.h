#ifndef MYTCPSOCKET_H
#define MYTCPSOCKET_H

#include <QTcpSocket>
#include "protocol.h"
#include "operatedatabase.h"
#include <QFile>
#include <QTimer>

// to distinguish between which socket is sending the information
// different client can connect to the same server
// therefore need to distinguish between these client

class MyTcpSocket : public QTcpSocket
{
    Q_OBJECT
public:
    MyTcpSocket();
    QString getName();

signals:
    void offline(MyTcpSocket* mysocket);

public slots:
    void recvMsg();
    void clientOffline();

    void sendFile();

private:
    QString m_strName;

    QFile m_file;
    qint64 m_iTotal;
    qint64 m_iReceived;
    bool m_bUpload;

    QTimer* m_pTimer;

};

#endif // MYTCPSOCKET_H
