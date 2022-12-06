#include "mtcpsocket.h"

MTCPSocket::MTCPSocket(QTcpSocket* socket, QString &name, mUserType type, QObject *parent)
    : QObject{parent}
{
    m_socket = socket;
    m_name = name;
    m_type = type;

    connect(socket, &QTcpSocket::readyRead, this, &MTCPSocket::socketReadyRead);
    connect(socket, &QTcpSocket::disconnected, this, &MTCPSocket::socketDisconnected);
}

MTCPSocket::~MTCPSocket()
{
    if(m_socket != 0)
        delete m_socket;
}

QTcpSocket *MTCPSocket::socket() const
{
    return m_socket;
}

const QString &MTCPSocket::name() const
{
    return m_name;
}

mUserType MTCPSocket::type() const
{
    return m_type;
}

void MTCPSocket::socketReadyRead()
{
    emit readyRead();
}

void MTCPSocket::socketDisconnected()
{
    emit disconnected();
}
