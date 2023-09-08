#include "tcpserver.h"
#include <QApplication>
//#include "operatedatabase.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    OperateDatabase::getInstance().init();

    TcpServer w;
    w.show();

    return a.exec();
}
