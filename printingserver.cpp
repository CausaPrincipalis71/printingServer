#include "printingserver.h"

printingServer::printingServer(quint16 port, QObject *parent)
    : QObject{parent}
{
    m_port = port;

    m_adminsData = mUser::getAdmins();
    m_usersData = mUser::getUsers();
    m_printersData = mUser::getPrinters();
}

bool printingServer::start()
{
    m_server = new QTcpServer();

    connect(m_server, &QTcpServer::newConnection, this, &printingServer::newConnection);

    return m_server->listen(QHostAddress::Any, m_port);
}

void printingServer::newConnection()
{
    QTcpSocket * tcpSocket = m_server->nextPendingConnection();
    tcpSocket->waitForReadyRead(100);

    auto data = tcpSocket->readAll().split('\n');
    loginState login = NotFound;

    QVector<mUser> *containerData;
    QVector<MTCPSocket*> *container;
    if(data.first() == "ADMIN")
    {
        containerData = &m_adminsData;
        container = &m_admins;
    }
    else if(data.first() == "USER")
    {
        containerData = &m_usersData;
        container = &m_users;
    }
    else if(data.first() == "PRINTER")
    {
        containerData = &m_printersData;
        container = &m_printers;
    }
    else
    {
        qWarning() << "MPrintingServer: Wrong connection from host" << tcpSocket->peerAddress();
        tcpSocket->write("WRONG PROTOCOL");
        tcpSocket->close();
        return;
    }

    QString name = data.at(1).split(' ').last();
    QString pass = data.at(2).split(' ').last();
    mUserType type;

    foreach (const auto &i, *containerData)
    {
        if(name == i.name() && pass == i.pass())
        {
            login = Correct;
            type = i.type();
            break;
        }
        else if(name == i.name() && pass != i.pass())
        {
            login = WrongPass;
            break;
        }
        else
        {
            login = NotFound;
        }
    }

    if(login != Correct)
    {
        qWarning() << "Wrong user data from host" << tcpSocket->peerAddress() << "\n\tTried to login as a" << name << "with pass" << pass;
        tcpSocket->write("WRONG DATA");
        tcpSocket->close();
        return;
    }

    qWarning() << "Have a connection from host" << tcpSocket->peerAddress() << "\n\tas a" << name << "with type" << type;
    tcpSocket->write("CONFIRMED");

    container->push_back(new MTCPSocket(tcpSocket, name, type));
    connect(container->back(), &MTCPSocket::readyRead, this, &printingServer::readyRead);
    connect(container->back(), &MTCPSocket::readyRead, this, &printingServer::clientDisconnected);
}

void printingServer::readyRead()
{
    MTCPSocket* senderTCP = qobject_cast<MTCPSocket*>(sender());

    qWarning() << senderTCP->name() << senderTCP->type();
}

void printingServer::clientDisconnected()
{

}

quint16 printingServer::port() const
{
    return m_port;
}

