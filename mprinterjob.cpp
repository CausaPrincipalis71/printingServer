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

void mPrinterJob::executeTask()
{
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
