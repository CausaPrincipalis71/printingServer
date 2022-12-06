#ifndef MTCPSOCKET_H
#define MTCPSOCKET_H
#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QSettings>
#include "muser.h"

class MTCPSocket : public QObject
{
    Q_OBJECT
public:
    explicit MTCPSocket(QTcpSocket* socket, QString& name, mUserType type, QObject *parent = nullptr);
    ~MTCPSocket();

    QTcpSocket *socket() const;

    const QString &name() const;

    mUserType type() const;

signals:
    void readyRead();
    void disconnected();

private slots:
    void socketReadyRead();
    void socketDisconnected();

private:
    QTcpSocket* m_socket;
    QString m_name;
    mUserType m_type;
};

#endif // MTCPSOCKET_H
