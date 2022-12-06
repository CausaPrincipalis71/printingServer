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

    QVector<MTCPSocket*> m_users;
    QVector<MTCPSocket*> m_admins;
    QVector<MTCPSocket*> m_printers;

    QVector<mUser> m_printersData;
    QVector<mUser> m_adminsData;
    QVector<mUser> m_usersData;

    quint16 m_port;

    enum loginState {NotFound, WrongPass, Correct};
signals:

};

#endif // PRINTINGSERVER_H
