#ifndef MUSER_H
#define MUSER_H

#include <QObject>
#include <QSettings>
#include <QVector>

enum mUserType {ADMIN, USER, PRINTER};

class mUser
{
public:
    mUser(QString &name, QString &pass, mUserType type, QString &category);

    static QVector<mUser> getAdmins();
    static QVector<mUser> getUsers();
    static QVector<mUser> getPrinters();

    static mUser addNewUser(QString name, QString pass, mUserType type, QString category);

    const QString &name() const;
    const QString &pass() const;
    const mUserType type() const;
    const QString &category() const;

    static const QString programName;
private:
    QString m_name;
    QString m_pass;
    mUserType m_type;
    QString m_category;

    static QVector<mUser> getByType(QString typeName);
};

#endif // MUSER_H
