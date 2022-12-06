#include "muser.h"

const QString mUser::programName = "MPrintingServer";

QString getTypeText(mUserType type)
{
    QString textType;
    switch(type)
    {
        case ADMIN:
            textType="admin";
            break;

        case USER:
            textType="user";
            break;

        case PRINTER:
            textType="printer";
            break;
    }
    return textType;
}

mUserType getTypeEnum(QString type)
{
    mUserType enumType;

    if(type == "admin")
        enumType=ADMIN;

    else if (type == "user")
        enumType=USER;

    else if (type == "printer")
        enumType=PRINTER;

    return enumType;
}

mUser::mUser(QString &name, QString &pass, mUserType type, QString &category)
{
    m_name = name;
    m_pass = pass;
    m_type = type;
    m_category = category;
}

QVector<mUser> mUser::getByType(QString typeName)
{
    QSettings data(programName, typeName);
    QVector<mUser> dataVector;

    for(const auto& i : data.childGroups())
    {
        data.beginGroup(i);
        QString name = i;
        QString pass = data.value("pass").toString();
        mUserType type = getTypeEnum(data.value("type").toString());
        QString category = data.value("category").toString();
        data.endGroup();

        dataVector.push_back(mUser(name, pass, type, category));
    }

    return dataVector;
}

QVector<mUser> mUser::getAdmins()
{
    return getByType("admin");
}

QVector<mUser> mUser::getUsers()
{
    return getByType("user");
}

QVector<mUser> mUser::getPrinters()
{
    return getByType("printer");
}

mUser mUser::addNewUser(QString name, QString pass, mUserType type, QString category)
{
    QString textType = getTypeText(type);

    QSettings data(programName, textType);

    data.beginGroup(name);
    data.setValue("pass", pass);
    data.setValue("type", textType);
    data.setValue("category", category);
    data.endGroup();

    return mUser(name, pass, type, category);
}

const QString &mUser::name() const
{
    return m_name;
}

const QString &mUser::pass() const
{
    return m_pass;
}

const mUserType mUser::type() const
{
    return m_type;
}

const QString &mUser::category() const
{
    return m_category;
}
