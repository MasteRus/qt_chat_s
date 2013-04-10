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
    //request Authorization
    static const quint8 comAuthorization = 1;
    //send message Authorization Success
    static const quint8 comAuthorizationSuccess = 2;
    //send message User join/left
    static const quint8 comUserJoin = 3;
    static const quint8 comUserLeft = 4;
    static const quint8 comUsersOnline = 5;

    static const quint8 comMessageToAll = 6;
    static const quint8 comMessageToUsers = 7;




    /*
    static const quint8 comPublicServerMessage = 7;
    static const quint8 comPrivateServerMessage = 8;
    */
    static const quint8 comErrNameInvalid = 201;
    static const quint8 comErrNameUsed = 202;

    bool isNameValid(QString name) const;
    bool isNameUsed(QString name) const;


private slots:
    void readyRead();//8
    void disconnected();
    //void Disconnectclient();

protected:
    void incomingConnection(int socketfd);//1
    void doSendCommand(quint8 comm, QTcpSocket *client,QString message) const;
    void doSendMessageToAll(QString message) const;
    QByteArray CreateDatagramm(quint8 comm,QString message) const;
//    bool CheckName(QString username);
    void SendUserList();
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
