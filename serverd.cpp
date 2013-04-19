#include "serverd.h"

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
        qDebug() << "_blockSize now " << (quint16)(blocksize[client]) <<", abaliable="<<client->bytesAvailable();
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
                    doSendCommand(comErrNameUsed,client,"");

                }
                else //everything is ok
                {
                    doSendCommand(comAuthorizationSuccess,client,"");
                    users[client]=name;//adding to list

                    doSendCommandToAll(comUserJoin,name);

                    doSendUserList(client);

                    /*
                    //doSendCommand(comUserJoin,client);
                    foreach(QTcpSocket *otherClient, clientlist)
                        doSendCommand(comUserJoin,otherClient,name);
                        */
                }

            } else {
                qDebug()<< "bad";
                doSendCommand(comErrNameInvalid,client,"");
                return;
            }
        }
        break;

        case comMessageToAll:
        {
            QString message;
            QString username=users[client];
            in >> message;
            doSendMessageToAll(message);
        }
        break;

        case comMessageToUsers:
        {
            QString message;
            QString username=users[client];
            in >> message;
            //doSendMessage(username+":"+message);
        }
        break;

    }

}

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

void serverd::doSendCommand(quint8 comm, QTcpSocket *client, QString message) const
{
    QByteArray block=CreateDatagramm(comm,message);
    client->write(block);
}

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
        //client->->write(block);
    {
        temp.append(users.value(otherclient)+"|");
        //qDebug() << "value=" <<users.value(otherclient);
    }
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

void serverd::disconnected()
{
    QTcpSocket *client = (QTcpSocket*)sender();
    qDebug() << "Client disconnected:" << client->peerAddress().toString();

    clientlist.remove(client);

    QString user = users[client];
    users.remove(client);
    blocksize.remove(client);

    //SendUserList();
    foreach(QTcpSocket *client, clientlist)
        client->write(QString("Server:" + user + " has left.\n").toUtf8());
}
