#include "printingserver.h"

printingServer::printingServer(quint16 port, QObject *parent)
    : QObject{parent}
{
    m_port = port;

    updateAdmins();
    updateUsers();
    updateAdmins();
    updateCategories();

    initFunctionsVector();
}

void printingServer::updateUsers()
{
    m_usersData = mUser::getUsers();
}

void printingServer::updateAdmins()
{
    m_adminsData = mUser::getAdmins();
}

void printingServer::updatePrinters()
{
    m_printersData = mUser::getPrinters();
}

void printingServer::updateByType(QString type)
{
    if(type == "ADMIN")
        updateAdmins();
    else if(type == "USER")
        updateUsers();
    else if(type == "PRINTER")
        updatePrinters();
}

void printingServer::updateCategories()
{
    m_availableCategories = mUser::getCategories();
}

void printingServer::initFunctionsVector()
{
    //Admin`s setters
    commandFunctionsVector.push_back(commandFunction{"CREATE_USER", int(mUser::ADMIN), &printingServer::createUser});
    commandFunctionsVector.push_back(commandFunction{"CREATE_GROUP", int(mUser::ADMIN), &printingServer::createGroup});
    //Admin`s getters
    commandFunctionsVector.push_back(commandFunction{"GET_USERS", int(mUser::ADMIN), &printingServer::getUsers});
    commandFunctionsVector.push_back(commandFunction{"GET_CATEGORIES", int(mUser::ADMIN), &printingServer::getCategories});
    commandFunctionsVector.push_back(commandFunction{"GET_INFO", int(mUser::ADMIN), &printingServer::getInfo});
    //Admin`s changers
    commandFunctionsVector.push_back(commandFunction{"CHANGE_USER_PASSWORD", int(mUser::ADMIN), &printingServer::changeUserPassword});
    commandFunctionsVector.push_back(commandFunction{"CHANGE_USER_CATEGORIES", int(mUser::ADMIN), &printingServer::changeUserCategories});
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

/*  Example of incoming message:
    "ADMIN\nl_admin\np_admin"
    "USER\nl_user\np_qwerty"

    First is type. Available type are in mUser::mUserType enum
    Second is login, where "l_" is marker
    Third is passwordd, where "p_" is marker
*/
    auto data = tcpSocket->readAll().split('\n');
    loginState login = NotFound;

    QVector<MTCPSocket*> *container = getContainerByType(data.first());
    QVector<mUser*> *containerData = getContainerDataByType(data.first());
    if(container == nullptr && containerData == nullptr)
    {
        wrongConnection(tcpSocket);
        return;
    }

    QString name = data.at(1).split(' ').first();
    QString pass = data.at(2).split(' ').first();
    mUser::mUserType type;

    if(name.startsWith("l_") && pass.startsWith("p_"))
    {
        name.remove(0, 2);
        pass.remove(0, 2);
    }
    else
    {
        wrongConnection(tcpSocket);
        return;
    }

    foreach (const auto &i, *containerData)
    {
        if(name == i->name() && pass == i->pass())
        {
            login = Correct;
            type = i->type();
            break;
        }
        else if(name == i->name() && pass != i->pass())
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
        qWarning() << "MPrintingServer: Wrong user data from host" << tcpSocket->peerAddress() << "\n\tTried to login as a" << name << "with pass" << pass;
        tcpSocket->write("WRONG DATA");
        tcpSocket->close();
        return;
    }

    foreach (const auto &user, *container)
    {
        if(user->name() == name)
        {
            qWarning() << "MPrintingServer: Double connetion from host" << tcpSocket->peerAddress() << "\n\tTried to login as a" << name << "with pass" << pass;
            tcpSocket->write("ALREADY_CONNECTED");
            tcpSocket->close();
            return;
        }
    }

    qWarning() << "MPrintingServer: Have a connection from host" << tcpSocket->peerAddress() << "\n\tas a" << name << "with type" << type << "\n";
    tcpSocket->write(("CONFIRMED\n" + mUser::enumToString(type)).toLatin1());

    container->push_back(new MTCPSocket(tcpSocket, name, type));
    connect(container->back(), &MTCPSocket::readyRead, this, &printingServer::readyRead);
    connect(container->back(), &MTCPSocket::disconnected, this, &printingServer::clientDisconnected);
}

void printingServer::readyRead()
{
    MTCPSocket* senderTCP = qobject_cast<MTCPSocket*>(sender());

    QList<QByteArray> data = senderTCP->socket()->readAll().split('\n');
    foreach(auto const &request, data)
    {
        auto requestCommand = request.split(' ');
        bool commandFound = false;
        for(int i = 0; i < commandFunctionsVector.size(); i++)
        {
            if (requestCommand.at(0) == commandFunctionsVector.at(i).id)
            {
                if ((commandFunctionsVector.at(i).availableTypes & senderTCP->type()) != 0)      // If sender`s type is contains inside command type
                {
                    requestCommand.pop_front();
                    (this->*commandFunctionsVector.at(i).function)(requestCommand, senderTCP);
                    commandFound = true;
                    break;
                }
                else if((commandFunctionsVector.at(i).availableTypes & senderTCP->type()) == 0)
                {
                    senderTCP->socket()->write(requestCommand.at(0) + " RESTRICTED");
                    commandFound = true;
                    break;
                }
            }
        }
        if(commandFound == false)
        {
            senderTCP->socket()->write((QString("ERROR: ") + requestCommand.at(0) + " IS_UNKNOWN_COMMAND").toLatin1());
        }
    }
}

void printingServer::clientDisconnected()
{
    MTCPSocket* senderTCP = qobject_cast<MTCPSocket*>(sender());
    QByteArray type = mUser::enumToString(senderTCP->type()).toLatin1();

    QVector<MTCPSocket*> * container = getContainerByType(type);

    for(int i = 0; i < container->size(); i++)
    {
        if(container->at(i)->name() == senderTCP->name())
        {
            container->removeAt(i);
            break;
        }
    }
}

QVector<MTCPSocket *> *printingServer::getContainerByType(QByteArray &type)
{
    QVector<MTCPSocket *> *container = nullptr;
    if(type == "ADMIN")
    {
        container = &m_admins;
    }
    else if(type == "USER")
    {
        container = &m_users;
    }
    else if(type == "PRINTER")
    {
        container = &m_printers;
    }

    return container;
}

QVector<mUser *> *printingServer::getContainerDataByType(QByteArray &type)
{
    QVector<mUser *> *container = nullptr;
    if(type == "ADMIN")
    {
        container = &m_adminsData;
    }
    else if(type == "USER")
    {
        container = &m_usersData;
    }
    else if(type == "PRINTER")
    {
        container = &m_printersData;
    }

    return container;
}

MTCPSocket *printingServer::getUserByName(QByteArray &name, QByteArray &type)
{
    QVector<MTCPSocket*> *container = getContainerByType(type);
    MTCPSocket *user = nullptr;

    foreach(const auto &i, *container)
    {
        if(i->name() == name)
        {
            user = i;
            break;
        }
    }

    return user;
}

mUser *printingServer::getUserDataByName(QByteArray &name, QByteArray &type)
{
    QVector<mUser*> *containerData = getContainerDataByType(type);
    mUser *userData = nullptr;
    foreach(const auto &i, *containerData)
    {
        if(i->name() == name)
        {
            userData = i;
            break;
        }
    }

    return userData;
}

void printingServer::wrongConnection(QTcpSocket *tcpSocket)
{
    qWarning() << "MPrintingServer: Wrong connection from host" << tcpSocket->peerAddress();
    tcpSocket->write("WRONG PROTOCOL");
    tcpSocket->close();
    return;
}

bool printingServer::createUser(QList<QByteArray> data, MTCPSocket *socket)
{
    if(data.size() < 4)
    {
        socket->socket()->write("ERROR NOT_ENOUGH_DATA");
        return false;
    }

    QByteArray name = data.at(0);
    QByteArray pass = data.at(1);
    QByteArray type = data.at(2);
    QByteArray category = data.at(3);

    if(name.isEmpty() || pass.isEmpty() || type.isEmpty() || category.isEmpty())
    {
        socket->socket()->write("ERROR NOT_ENOUGH_DATA");
        return false;
    }

    auto categoriesDeclared = category.split(',');

    auto containerData = getContainerDataByType(type);
    if(containerData == nullptr)
    {
        socket->socket()->write("ERROR UNKNOWN_TYPE");
        return false;
    }

    foreach(auto const &user, *containerData)
    {
        if(name == user->name())
        {
            socket->socket()->write("ERROR USER_EXISTS");
            return false;
        }
    }

    bool categoriesCorrect = true;
    foreach(auto const &part, categoriesDeclared)
    {
        foreach(auto const &categoryAvailable, m_availableCategories)
        {
            if(part == categoryAvailable)
            {
                categoriesCorrect = categoriesCorrect && true;
                break;
            }
            else if(categoryAvailable == m_availableCategories.back() && part != categoryAvailable)
            {
               categoriesCorrect = categoriesCorrect && false;
               socket->socket()->write("ERROR: UNKNOWN_CATEGORY");
               return false;
            }
        }
    }

    mUser::addNewUser(name, pass, mUser::stringToEnum(type), category);
    updateByType(type);                                                     //Find new user
    socket->socket()->write("DONE");
    return true;
}

bool printingServer::createGroup(QList<QByteArray> data, MTCPSocket *socket)
{
    if(data.size() < 1 || data.at(0).isEmpty())
    {
        socket->socket()->write("ERROR NOT_ENOUGH_DATA");
        return false;
    }

    foreach(const auto &category, data)
    {
        if(category.isEmpty())
            continue;
        mUser::addNewCategory(category);
        updateCategories();                 //add new categories
    }

    socket->socket()->write("DONE");
    return true;
}

bool printingServer::getUsers(QList<QByteArray> data, MTCPSocket *socket)
{
    foreach(const auto &user, m_usersData)
        socket->socket()->write((user->name() + "\n").toLatin1());

    socket->socket()->write("DONE");
    return true;
}

bool printingServer::getCategories(QList<QByteArray> data, MTCPSocket *socket)
{
    foreach(const auto &category, m_availableCategories)
        socket->socket()->write((category + "\n").toLatin1());

    socket->socket()->write("DONE");
    return true;
}

bool printingServer::getInfo(QList<QByteArray> data, MTCPSocket *socket)
{
    if(data.size() < 2)
    {
        socket->socket()->write("ERROR NOT_ENOUGH_DATA");
        return false;
    }
    auto type = data.at(0);
    data.pop_front();

    auto name = data.at(0);
    data.pop_front();

    mUser *userData = getUserDataByName(name, type);
    MTCPSocket *user = getUserByName(name, type);
    QString userState = "OFFLINE";

    if(userData == nullptr)
    {
        socket->socket()->write("ERROR USER_NOT_FOUND");
        return false;
    }

    if(user != nullptr)
        userState = "ONLINE";

    if(type == "USER")
    {
        foreach(const auto &request, data)
        {
            if(request == "CATEGORY")
            {
                socket->socket()->write((userData->category() + "\n").toLatin1());
            }
            else if(request == "CONNECTION_STATE")
            {
                socket->socket()->write((userState + "\n").toLatin1());
            }
            else
            {
                socket->socket()->write("WARNING: " + request + " IS_UNKNOWN\n");
            }
        }
    }
    else if(type == "PRINTER")
    {
        foreach(const auto &request, data)
        {
            if(request == "CATEGORY")
            {
                socket->socket()->write((userData->category().split(',').at(0) + "\n").toLatin1());
            }
            else if(request == "CONNECTION_STATE")
            {
                socket->socket()->write((userState + "\n").toLatin1());
            }
            else
            {
                socket->socket()->write("WARNING: " + request + " IS_UNKNOWN\n");
            }
        }
    }

    socket->socket()->write("DONE");
    return true;
}

bool printingServer::changeUserPassword(QList<QByteArray> data, MTCPSocket *socket)
{
    if(data.size() < 2)
    {
        socket->socket()->write("ERROR NOT_ENOUGH_DATA");
        return false;
    }

    QByteArray name = data.takeFirst();
    QByteArray newPass = data.takeFirst();
    QByteArray type = "USER";

    if(name.isEmpty() || newPass.isEmpty())
    {
        socket->socket()->write("ERROR NOT_ENOUGH_DATA");
        return false;
    }

    mUser *userData = getUserDataByName(name, type);

    if(userData == nullptr)
    {
        socket->socket()->write("ERROR USER_NOT_FOUND");
        return false;
    }

    userData->setPass(newPass);
    updateUsers();
    socket->socket()->write("DONE");
    return true;
}

bool printingServer::changeUserCategories(QList<QByteArray> data, MTCPSocket *socket)
{
    if(data.size() < 2)
    {
        socket->socket()->write("ERROR NOT_ENOUGH_DATA");
        return false;
    }

    QByteArray name = data.takeFirst();
    QByteArray newCategories = data.takeFirst();
    QByteArray type = "USER";

    if(name.isEmpty() || newCategories.isEmpty())
    {
        socket->socket()->write("ERROR NOT_ENOUGH_DATA");
        return false;
    }

    mUser *userData = getUserDataByName(name, type);

    if(userData == nullptr)
    {
        socket->socket()->write("ERROR USER_NOT_FOUND");
        return false;
    }

    if(mUser::checkCategories(newCategories) == false)
    {
        socket->socket()->write("ERROR: UNKNOWN_CATEGORY");
        return false;
    }

    userData->setCategory(newCategories);
    updateUsers();
    socket->socket()->write("DONE");
    return true;
}

quint16 printingServer::port() const
{
    return m_port;
}

