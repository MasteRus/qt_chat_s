#ifndef ABSTRACTCHAT_H
#define ABSTRACTCHAT_H
#include <QTcpServer>
#include <QTcpSocket>

#include <QList>
#include <QMap>
#include <QSet>

class abstractchat
{
public:
    abstractchat();
    QByteArray CreateDatagramm(quint8 comm,QString message) const;
    QByteArray CreateDatagramm(quint8 comm) const;

    void doSendCommand(quint8 comm, QTcpSocket *client, QString message) const;
    void doSendCommand(quint8 comm, QTcpSocket *client) const;
};

#endif // ABSTRACTCHAT_H
