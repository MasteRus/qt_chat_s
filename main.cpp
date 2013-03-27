#include <QtCore/QCoreApplication>
#include <QDebug>
#include "serverd.h"


int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    qDebug() << "Start";
    quint16 portnumb=32023;

    serverd server;
    //quint16 portnumb=32023;

    //bool success = server.listen(QHostAddress::Any, portnumb);
    bool success = server.listen(QHostAddress::LocalHost, portnumb);
    if(!success)
    {
        qDebug() << "Bad";
    }
    qDebug() << "Ready";


    return a.exec();
}
