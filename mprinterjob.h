#ifndef MPRINTERJOB_H
#define MPRINTERJOB_H

#include <QObject>
#include <QString>
#include "muser.h"
#include "mtcpsocket.h"

class mPrinterJob : public QObject
{
    Q_OBJECT
public:
    mPrinterJob(QByteArray &name, const QByteArray &category,  int maxSplits, QObject *parent = nullptr);

    const QByteArray &category() const;

    const QVector<MTCPSocket *> &performerPrinters() const;

    const QByteArray &name() const;

    void setPerformerPrinters(const QVector<MTCPSocket *> &newPerformerPrinters);

    void executeTask();

    int maxSplits() const;

    const QVector<mUser *> &performerPrintersData() const;
    void setPerformerPrintersData(const QVector<mUser *> &newPerformerPrintersData);

private:
    QByteArray m_name;
    int m_maxSplits;
    QByteArray m_category;
    QVector<MTCPSocket*> m_performerPrinters;
    QVector<mUser*> m_performerPrintersData;

    int m_jobState;
    int m_actualSplits;

signals:

};

#endif // MPRINTERJOB_H
