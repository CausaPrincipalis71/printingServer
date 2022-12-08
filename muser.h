#ifndef MUSER_H
#define MUSER_H

#include <QObject>
#include <QSettings>
#include <QVector>
#include <QMetaEnum>

class mUser : public QObject
{
Q_OBJECT

public:
    enum mUserType {ADMIN = 1 << 0, USER = 1 << 1, PRINTER = 1 << 2};
    Q_ENUM(mUserType)

    mUser(const QString &name, const QString &pass, mUserType type, const QString &category, QObject *parent = nullptr);

    static QVector<mUser*> getAdmins();
    static QVector<mUser*> getUsers();
    static QVector<mUser*> getPrinters();
    static QStringList getCategories();

    static mUser addNewUser(QString name, QString pass, mUserType type, QString category);
    static QString addNewCategory(QString name);

    const QString &name() const;
    const QString &pass() const;
    const mUserType type() const;
    const QString &category() const;

    static const QString programName;

    static QString enumToString (const mUserType value);
    static mUserType stringToEnum (const QString &key);

    static bool checkCategories(const QString &categories);

    void setPass(const QString &newPass);

    void setCategory(const QString &newCategory);

    bool isBusy() const;
    void setIsBusy(bool newIsBusy);

    int jobState() const;
    void setJobState(int newJobState);

    const QString &jobName() const;
    void setJobName(const QString &newJobName);

private:
    QString m_name;
    QString m_pass;
    mUserType m_type;
    QString m_category;

    bool m_isBusy;
    int m_jobState;
    QString m_jobName;

    static QVector<mUser *> getByType(QString typeName);
};

#endif // MUSER_H
