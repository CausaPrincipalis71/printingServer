#include "mprinterjob.h"

mPrinterJob::mPrinterJob(QByteArray &name,  const QByteArray &category, int maxSplits, QObject *parent) : QObject(parent),
    m_maxSplits(maxSplits),
    m_category(category),
    m_name(name)
{}

const QByteArray &mPrinterJob::category() const
{
    return m_category;
}

const QVector<MTCPSocket *> &mPrinterJob::performerPrinters() const
{
    return m_performerPrinters;
}

const QByteArray &mPrinterJob::name() const
{
    return m_name;
}

void mPrinterJob::setPerformerPrinters(const QVector<MTCPSocket *> &newPerformerPrinters)
{
    m_performerPrinters = newPerformerPrinters;
}

void mPrinterJob::executeTask(const QVector<MTCPSocket *> &newPerformerPrinters)
{
    setPerformerPrinters(newPerformerPrinters);

    m_workType = CURRENT;

    m_actualSplits = m_performerPrinters.size();

    for(int i = 0; i < m_actualSplits; i++)
    {
        MTCPSocket *printer = m_performerPrinters.at(i);
        mUser *user = m_performerPrintersData.at(i);
        user->setIsBusy(true);
        user->setJobName(m_name);
        printer->socket()->write("NEW_JOB " + m_actualSplits);
    }
}

void mPrinterJob::updateState(mUser *performer, int currentState)
{
    int updateState = currentState - performer->jobState();
    performer->setJobState(currentStates);
    m_progress += updateState/m_actualSplits;

    if(round(m_progress) >= 100)
    {
        m_workType = COMPLETED;
        for(int i = 0; i < performerPrinters().size(); i++)
        {
            m_performerPrinters.at(i)->socket()->write("COMPLETED\n");
            m_performerPrintersData.at(i)->setIsBusy(false);
        }
    }
}

int mPrinterJob::maxSplits() const
{
    return m_maxSplits;
}

const QVector<mUser *> &mPrinterJob::performerPrintersData() const
{
    return m_performerPrintersData;
}

void mPrinterJob::setPerformerPrintersData(const QVector<mUser *> &newPerformerPrintersData)
{
    m_performerPrintersData = newPerformerPrintersData;
}

const QByteArray mPrinterJob::getPerformerPrintersNames() const
{
    QByteArray names = "";
    foreach(const auto &printer, m_performerPrintersData)
    {
        names += printer->name().toLatin1() + " ";
    }
    return names;
}

float mPrinterJob::progress() const
{
    return m_progress;
}

mPrinterJob::mWorkType mPrinterJob::workType() const
{
    return m_workType;
}

int mPrinterJob::actualSplits() const
{
    return m_actualSplits;
}

QByteArray mPrinterJob::enumToString(const mPrinterJob::mWorkType value)
{
    return QMetaEnum::fromType<mPrinterJob::mWorkType>().valueToKey(value);
}

mPrinterJob::mWorkType mPrinterJob::stringToEnum(const QByteArray &key)
{
    return static_cast<mWorkType>(QMetaEnum::fromType<mPrinterJob::mWorkType>().keyToValue(key));
}
