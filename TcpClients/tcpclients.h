#ifndef TCPCLIENTS_H
#define TCPCLIENTS_H

#include <QWidget>
#include <QFile>
#include <QTcpSocket>

#include "protocol.h"
#include "opewidget.h"

QT_BEGIN_NAMESPACE
namespace Ui { class TcpClients; }
QT_END_NAMESPACE

class TcpClients : public QWidget
{
    Q_OBJECT

public:
    TcpClients(QWidget *parent = nullptr);
    ~TcpClients();
    void loadConfig();

    static TcpClients &getInstance();
    QTcpSocket &getTcpSocket();
    QString getLoginName();
    QString getCurrentPath();

public slots:
    void showConnection();
    void receMsg();

private slots:
//    void on_pushButton_clicked();

    void on_login_clicked();

    void on_signup_clicked();

    void on_logoff_clicked();

private:
    Ui::TcpClients *ui;
    QString m_strIP;
    quint16 m_usPort;

    // connecting to the server, and server's data
    QTcpSocket m_tcpSocket;
    QString m_strLoginName;

    QString m_strCurrentPath;

    QFile m_file;

};
#endif // TCPCLIENTS_H
