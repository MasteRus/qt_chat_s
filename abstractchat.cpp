#include "abstractchat.h"

abstractchat::abstractchat()
{
}

QByteArray abstractchat::CreateDatagramm(quint8 comm,QString message) const
{
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out << (quint16)0;
    out << comm;
    out << message;
    out.device()->seek(0);
    out << (quint16)(block.size() - sizeof(quint16));
    return block;
}
QByteArray abstractchat::CreateDatagramm(quint8 comm) const
{
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out << (quint16)0;
    out << comm;
    out.device()->seek(0);
    out << (quint16)(block.size() - sizeof(quint16));
    return block;
}

void abstractchat::doSendCommand(quint8 comm, QTcpSocket *client, QString message) const
{
    QByteArray block=CreateDatagramm(comm,message);
    client->write(block);
}
void abstractchat::doSendCommand(quint8 comm, QTcpSocket *client) const
{
    QByteArray block=CreateDatagramm(comm);
    client->write(block);
}
