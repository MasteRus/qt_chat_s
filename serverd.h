#ifndef SERVERD_H
#define SERVERD_H

#include <QTcpServer>
#include <QTcpSocket>

#include <QList>
#include <QMap>
#include <QSet>

class serverd: public QTcpServer
{
    Q_OBJECT
public:
    serverd(QObject *parent=0);

    static const quint8 comAuthorization = 1;
    static const quint8 comUsersOnline = 2;
    static const quint8 comUserJoin = 3;
    static const quint8 comUserLeft = 4;
    static const quint8 comMessageToAll = 5;
    static const quint8 comMessageToUsers = 6;
    static const quint8 comPublicServerMessage = 7;
    static const quint8 comPrivateServerMessage = 8;
    static const quint8 comAuthorizationSuccess = 9;
    static const quint8 comErrNameInvalid = 201;
    static const quint8 comErrNameUsed = 202;


private slots:
    void readyRead();//8
    void disconnected();
    //void Disconnectclient();

protected:
    void incomingConnection(int socketfd);//1
    void doSendCommand(quint8 comm, QTcpSocket *client) const;
//    bool CheckName(QString username);
//    void SendUserList();
//    void AddUserToList(QString username);
//    void RemoveUserFromList(QString username);
//    //
//    void SendMessageToAll();
//    void SendMessageToUser();


private:
    QSet<QTcpSocket*> clientlist;
    QMap<QTcpSocket*,quint16> blocksize;
    QMap<QTcpSocket*,QString> users;
};


#endif // SERVERD_H
