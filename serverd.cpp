#include "serverd.h"
#include "abstractchat.h"
#include <QTcpSocket>
#include <QRegExp>
#include <QStringList>


serverd::serverd(QObject *parent) : QTcpServer(parent)
{
}
void serverd::incomingConnection(int socketfd)
{
    QTcpSocket *client = new QTcpSocket(this);
    client->setSocketDescriptor(socketfd);
    clientlist.insert(client);
    blocksize[client]=0;

    qDebug() << "New client from:" << client->peerAddress().toString();

    connect(client, SIGNAL(readyRead()), this, SLOT(readyRead()));
    connect(client, SIGNAL(disconnected()), this, SLOT(disconnected()));
}


bool serverd::isNameValid(QString name) const
{
    qDebug() << "IsValid: name=" << name;

    if (name.length() > 20 || name.length() <= 3)
        return false;
    QRegExp r("[A-Za-z0-9_]+");
    qDebug() << "IsVanidName=" << name<< ", "<< r.exactMatch(name);
    return r.exactMatch(name);
}
bool serverd::isNameUsed(QString name) const
{
    return (users.key(name,NULL));
}

void serverd::readyRead()
{
    QTcpSocket *client = (QTcpSocket*)sender();
    QDataStream in(client);
    qDebug() << "_blockSize start" << blocksize[client];
    if (blocksize[client] == 0) {
        if (client->bytesAvailable() < (int)sizeof(quint16))
            return;
        in >> blocksize[client];
        qDebug() << "_blockSize now " << (quint16)(blocksize[client]) <<", avaliable="<<client->bytesAvailable();
    }
    if (client->bytesAvailable() < blocksize[client])
        return;
    else
        blocksize[client] = 0;

    quint8 command;
    in >> command;
    qDebug() << "Received command " << command;

    switch(command)
    {
        case comAuthorization:
        {
            QString name;
            in >> name;
            qDebug()  << "Received command comAuthorization" << command<< " , name=" <<name;

            if (isNameValid(name))
            {
                qDebug()<< "nameValid";
                if (isNameUsed(name))
                {
                    doSendCommand(comErrNameUsed,client);
                }
                else //everything is ok
                {
                    users[client]=name;//adding to list

                    doSendCommandToAll(comUserJoin,name);
                    doSendCommand(comAuthorizationSuccess,client);

                    doSendUserList(client);
                }
            } else {
                qDebug()<< "bad";
                doSendCommand(comErrNameInvalid,client);
                return;
            }
        }
        break;

        case comMessageToAll:
        {
            QString message;
            QString username=users[client];
            in >> message;
            doSendMessageToAll(username+":"+message);
        }
        break;

        case comMessageToUsers:
        {
            QString message;
            QString username=users[client];
            in >> message;
            QString temp=message.section(":",1);
            QString usersstring=message.section(":",0,0);
            usersstring.append(","+username);
            qDebug() << "Users:" <<usersstring <<" message=" << message  ;
            QStringList userslist=usersstring.split(",");
            //use
            doSendMessageToUsers("[private] "+username+":"+temp,userslist);
        }
        break;

    }
    qDebug() << "END_blockSize now " << (quint16)(blocksize[client]) <<", avaliable="<<client->bytesAvailable()<< '\n';
}
/*
QByteArray serverd::CreateDatagramm(quint8 comm,QString message) const
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
QByteArray serverd::CreateDatagramm(quint8 comm) const
{
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out << (quint16)0;
    out << comm;
    out.device()->seek(0);
    out << (quint16)(block.size() - sizeof(quint16));
    return block;
}

void serverd::doSendCommand(quint8 comm, QTcpSocket *client, QString message) const
{
    QByteArray block=CreateDatagramm(comm,message);
    client->write(block);
}
void serverd::doSendCommand(quint8 comm, QTcpSocket *client) const
{
    QByteArray block=CreateDatagramm(comm);
    client->write(block);
}
*/
void serverd::doSendCommandToAll(quint8 comm, QString message) const
{
    QByteArray block=CreateDatagramm(comm,message);
    foreach(QTcpSocket *otherClient, clientlist)
        otherClient->write(block);

}

void serverd::doSendUserList(QTcpSocket *client) const
{
    QString temp;
    foreach(QTcpSocket *otherclient, clientlist)
    {
        temp.append(users.value(otherclient)+"|");
    }
    temp.remove(temp.length()-1, 1);
    qDebug() << "clientlist" << temp ;

    QByteArray block=CreateDatagramm(comUsersOnline,temp);

    foreach(QTcpSocket *otherClient, clientlist)
        otherClient->write(block);
}

void serverd::doSendMessageToAll(QString message) const
{
    QByteArray block=CreateDatagramm(comMessageToAll,message);

    foreach(QTcpSocket *client, clientlist)
        client->write(block);
}

void serverd::doSendMessageToUsers(QString message, QStringList userlist) const
{
    QByteArray block=CreateDatagramm(comMessageToUsers,message);

    foreach(QTcpSocket *client, clientlist)
        if (userlist.contains(users[client]))
            client->write(block);
}


void serverd::disconnected()
{
    QTcpSocket *client = (QTcpSocket*)sender();
    qDebug() << "Client disconnected:" << client->peerAddress().toString();

    clientlist.remove(client);

    QString user = users[client];
    users.remove(client);
    blocksize.remove(client);

    doSendCommandToAll(comUserLeft,user);
}
