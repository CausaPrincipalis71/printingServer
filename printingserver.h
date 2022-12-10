#ifndef PRINTINGSERVER_H
#define PRINTINGSERVER_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QSettings>
#include <QTimer>
#include "muser.h"
#include "mtcpsocket.h"
#include "mprinterjob.h"

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

    void updateJobs();

private:
    QTcpServer * m_server;
    quint16 m_port;

    QVector<MTCPSocket*> m_users;
    QVector<MTCPSocket*> m_admins;
    QVector<MTCPSocket*> m_printers;
    QVector<QVector<MTCPSocket*> *> m_all = {&m_admins, &m_users, &m_printers};

    QVector<mUser*> m_printersData;
    QVector<mUser*> m_adminsData;
    QVector<mUser*> m_usersData;

    QStringList m_availableCategories;

    QVector<MTCPSocket *> *getContainerByType(const QByteArray &type);
    QVector<mUser*> *getContainerDataByType(const QByteArray &type);

    MTCPSocket *getUserByName(const QByteArray &name, const QByteArray &type);

    QVector<MTCPSocket*> getUsersByCategory(const QByteArray &category, const QByteArray &type);

    mUser *getUserDataByName(const QByteArray *name, const QByteArray *type);
    mUser *getUserDataByName(const QByteArray name, const QByteArray type);
    mUser *getUserDataByName(const QString &name, const QString &type);

    QVector<mUser*> getUsersDataByCategory(const QByteArray &category, const QByteArray &type);

    QVector<mPrinterJob *> m_queryJobs;
    QVector<mPrinterJob *> m_currentJobs;
    QVector<mPrinterJob *> m_completedJobs;
    QVector<QVector <mPrinterJob *> *> m_jobVectors;

    QVector<mPrinterJob *> *getJobsContainerByState(mPrinterJob::mWorkType state);
    QVector<mPrinterJob *> *getJobsContainerByState(QByteArray state);
    mPrinterJob *getJobByName(QByteArray &workState, QByteArray name);
    mPrinterJob *getJobByName(QByteArray &workState, QString &name);
    mPrinterJob *getJobByName(mPrinterJob::mWorkType workState, QByteArray &name);

    mPrinterJob *getAnyJobByName(QByteArray &name);

    QTimer *updater;

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
    // createCategory       Syntax of command:   CREATE_CATEGORY NAME
    //                                           CREATE_CATEGORY hall
    //                                           CREATE_CATEGORY office factory
    bool createCategory(QList<QByteArray> data, MTCPSocket* socket);

    // GETTERS FUNCTIONS FOR ADMINS
    // getUsers          Syntax of command:  GET_USERS
    //                                       Without arguments
    bool getUsers(QList<QByteArray> data, MTCPSocket* socket);
    // getCategories     Syntax of command:  GET_CATEGORIES
    //                                       Without arguments
    bool getCategories(QList<QByteArray> data, MTCPSocket* socket);
    // getInfo           Syntax of command: GET_USER_INFO  NAME {CATEGORY, CONNECTION_STATE}
    bool getUserInfo(QList<QByteArray> data, MTCPSocket* socket);
    // getOnline         Syntax of command: GET_ONLINE {ALL, USER, ADMIN, PRINTER}
    bool getOnline(QList<QByteArray> data, MTCPSocket* socket);

    // GETTERS FUNCTIONS FOR BOTH ADMINS AND USERS
    // getUsers          Syntax of command:  GET_PRINTERS
    //                                       Without arguments
    bool getPrinters(QList<QByteArray> data, MTCPSocket* socket);
    // getPrinterInfo    Syntax of command:  GET_USER_INFO PRINTER NAME {CATEGORY, CONNECTION_STATE, JOB_STATUS, JOB_PROGRESS, ALL}
    bool getPrinterInfo(QList<QByteArray> data, MTCPSocket* socket);

    // CHANGING FUNCTIONS FOR BOTH ADMINS AND USERS:
    // changeUserPassword   Syntax of comand:   CHANGE_USER_PASSWORD NAME NEW_PASSWORD
    //                                          CHANGE_USER_PASSWORD leonid trewq
    bool changeUserPassword(QList<QByteArray> data, MTCPSocket* socket);
    // changeUserCategory   Syntax of comand:   CHANGE_USER_CATEGORY NAME NEW_CATEGORIES
    //                                          CHANGE_USER_CATEGORY leonid all,office,hill
    bool changeUserCategory(QList<QByteArray> data, MTCPSocket* socket);

    // JOB FUNCTIONS FOR BOTH ADMINS AND USERS:
    // createJob            Syntax of comand:   CREATE_JOB NAME CATEGORY MAX_PARTS
    //                                          CREATE_JOB body_1231 hall 4
    bool createJob(QList<QByteArray> data, MTCPSocket* socket);
    // getQueryJobs         Syntax of command:  GET_QUERY_JOBS
    //                                          Without arguments
    bool getQueryJobs(QList<QByteArray> data, MTCPSocket* socket);
    // getCurrentJobs       Syntax of command:  GET_CURRENT_JOBS
    //                                          Without arguments
    bool getCurrentJobs(QList<QByteArray> data, MTCPSocket* socket);
    // getCompletedJobs     Syntax of command:  GET_COMPLETED_JOBS
    //                                          Without arguments
    bool getCompletedJobs(QList<QByteArray> data, MTCPSocket* socket);

    // getJobInfo           Syntax of command:  GET_JOB_INFO NAME {PERFORMERS, MAX_SPLITS, ACTUAL_SPLITS, JOB_STATE, PROGRESS, ALL}
    bool getJobInfo(QList<QByteArray> data, MTCPSocket* socket);

    // JOB FUNCTIONS FOR PRINTERS
    // sendProgress         Syntax of command:  SEND_PROGRESS TASK_NAME STATE
    //                                          SEND_PROGRESS body 55
    bool sendProgress(QList<QByteArray> data, MTCPSocket* socket);


signals:

};

#endif // PRINTINGSERVER_H
