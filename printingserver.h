#ifndef PRINTINGSERVER_H
#define PRINTINGSERVER_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QSettings>
#include "muser.h"
#include <mtcpsocket.h>

class printingServer : public QObject
{
    Q_OBJECT
public:
    explicit printingServer(quint16 port, QObject *parent = nullptr);

    bool start();

    quint16 port() const;

private slots:
    void newConnection();
    void readyRead();
    void clientDisconnected();

private:
    QTcpServer * m_server;
    quint16 m_port;

    QVector<MTCPSocket*> m_users;
    QVector<MTCPSocket*> m_admins;
    QVector<MTCPSocket*> m_printers;

    QVector<mUser*> m_printersData;
    QVector<mUser*> m_adminsData;
    QVector<mUser*> m_usersData;

    QStringList m_availableCategories;

    QVector<MTCPSocket *> *getContainerByType(QByteArray &type);
    QVector<mUser*> *getContainerDataByType(QByteArray &type);

    MTCPSocket *getUserByName(QByteArray &name, QByteArray &type);
    mUser *getUserDataByName(QByteArray &name, QByteArray &type);

    // Updating data on subjects and objects of the system
    void updateUsers();
    void updateAdmins();
    void updatePrinters();
    void updateByType(QString type);
    void updateCategories();

    enum loginState {NotFound, WrongPass, Correct};

    void wrongConnection(QTcpSocket* tcpSocket);

    // A structure that stores the command specifier, what types of users are allowed to execute it, and what function is called to process it
    struct commandFunction
    {
        QString id;
        int availableTypes;
        bool (printingServer::*function)(QList<QByteArray> data, MTCPSocket* socket);   //Each function must return a bool value, and as an argument, take a command sent by the client and a socket to communicate with this client
    };

    QVector<commandFunction> commandFunctionsVector;
    void initFunctionsVector();

    // Client Command Processing Functions
    // CREATION FUNCTIONS FOR ADMINS:

    // createUser           Syntax of command:   CREATE_USER NAME PASSWORD TYPE CATEGORIES
    //                                           CREATE_USER user qwerty USER hall,factory1,office
    //                                           CREATE_USER printer1 1 PRINTER factory1
    bool createUser(QList<QByteArray> data, MTCPSocket* socket);
    // createGroup           Syntax of command:  CREATE_GROUP NAME
    //                                           CREATE_GROUP hall
    //                                           CREATE_GROUP office factory
    bool createGroup(QList<QByteArray> data, MTCPSocket* socket);

    // GETTERS FUNCTIONS FOR ADMINS
    // getUsers          Syntax of command:  GET_USERS
    //                                       Without arguments
    bool getUsers(QList<QByteArray> data, MTCPSocket* socket);
    // getCategories     Syntax of command:  GET_CATEGORIES
    //                                       Without arguments
    bool getCategories(QList<QByteArray> data, MTCPSocket* socket);
    // getInfo           Syntax of command: GET_INFO USER NAME {CATEGORY, CONNECTION_STATE}
    //                                      GET_INFO PRINTER NAME {CATEGORY, CONNECTION_STATE, JOB_STATUS, JOB_PROGRESS}
    bool getInfo(QList<QByteArray> data, MTCPSocket* socket);

    // CHANGING FUNCTIONS FOR BOTH ADMINS:
    // changeUserPassword   Syntax of comand:   CHANGE_USER_PASSWORD NAME NEW_PASSWORD
    //                                          CHANGE_USER_PASSWORD leonid trewq
    bool changeUserPassword(QList<QByteArray> data, MTCPSocket* socket);
    // changeUserCategories Syntax of comand:   CHANGE_USER_CATEGORIES NAME NEW_CATEGORIES
    //                                          CHANGE_USER_CATEGORIES leonid all,office,hill
    bool changeUserCategories(QList<QByteArray> data, MTCPSocket* socket);

signals:

};

#endif // PRINTINGSERVER_H
