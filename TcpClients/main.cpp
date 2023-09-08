#include "tcpclients.h"
#include <QMessageBox>

#include <QApplication>
//#include "opewidget.h"
//#include "online.h"

//#include "friend.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    TcpClients::getInstance().show();


//    Online w;
//    w.show();

//    OpeWidget w;
//    w.show();

//    Friend w;
//    w.show();

    return a.exec();
}
