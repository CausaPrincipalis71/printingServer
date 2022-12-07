#include "muser.h"

const QString mUser::programName = "MPrintingServer";

mUser::mUser(const QString &name, const QString &pass, mUser::mUserType type, const QString &category, QObject *parent) : QObject(parent),
    m_name(name),
    m_pass(pass),
    m_type(type),
    m_category(category)
{}

QVector<mUser*> mUser::getByType(QString typeName)
{
    QSettings data(programName, typeName.toLower());
    QVector<mUser*> dataVector;

    for(const auto& i : data.childGroups())
    {
        data.beginGroup(i);
        QString name = i;
        QString pass = data.value("pass").toString();
        mUserType type = stringToEnum(data.value("type").toString());
        QString category = data.value("category").toString();
        data.endGroup();

        dataVector.push_back(new mUser(name, pass, type, category));
    }

    return dataVector;
}

QVector<mUser*> mUser::getAdmins()
{
    return getByType("ADMIN");
}

QVector<mUser*> mUser::getUsers()
{
    return getByType("USER");
}

QVector<mUser*> mUser::getPrinters()
{
    return getByType("PRINTER");
}

QStringList mUser::getCategories()
{
    QSettings data(programName, "categories");
    return data.childGroups();
}

mUser mUser::addNewUser(QString name, QString pass, mUserType type, QString category)
{
    QString textType = enumToString(type);

    QSettings data(programName, textType.toLower());

    data.beginGroup(name);
    data.setValue("type", textType);
    data.setValue("category", category);
    data.setValue("pass", pass);
    data.endGroup();

    return mUser(name, pass, type, category);
}

QString mUser::addNewCategory(QString name)
{
    QSettings data(programName, "categories");

    data.beginGroup(name);
    data.setValue("active", "true");
    data.endGroup();

    return name;
}

const QString &mUser::name() const
{
    return m_name;
}

const QString &mUser::pass() const
{
    return m_pass;
}

const mUser::mUserType mUser::type() const
{
    return m_type;
}

const QString &mUser::category() const
{
    return m_category;
}

QString mUser::enumToString(const mUser::mUserType value)
{
    return QMetaEnum::fromType<mUser::mUserType>().valueToKey(value);
}

mUser::mUserType mUser::stringToEnum(const QString &key)
{
    return static_cast<mUserType>(QMetaEnum::fromType<mUser::mUserType>().keyToValue(key.toLatin1().data()));
}

bool mUser::checkCategories(const QString &data)
{
    QStringList availableCategories = getCategories();
    auto categories = data.split(',');

    foreach(const auto &checkedCategory, categories)
    {
        foreach(const auto &availableCategory, availableCategories)
        {
            if(checkedCategory == availableCategory)
                break;
            else if(checkedCategory != availableCategory && availableCategory == availableCategories.last())
                return false;
        }
    }

    return true;
}

void mUser::setPass(const QString &newPass)
{
    m_pass = newPass;

    QString textType = enumToString(m_type);

    QSettings data(programName, textType.toLower());
    data.beginGroup(m_name);
    data.setValue("pass", newPass);
    data.endGroup();

    return;
}

void mUser::setCategory(const QString &newCategory)
{
    m_category = newCategory;
}

mUser::mUserType operator|(mUser::mUserType a, mUser::mUserType b)
{
    return static_cast<mUser::mUserType>(static_cast<int>(a) | static_cast<int>(b));
}

mUser::mUserType operator&(mUser::mUserType a, mUser::mUserType b)
{
    return static_cast<mUser::mUserType>(static_cast<int>(a) & static_cast<int>(b));
}
