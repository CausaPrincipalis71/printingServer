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
    enum mWorkType {QUEUE, CURRENT, COMPLETED};
    Q_ENUM(mWorkType)

    mPrinterJob(QByteArray &name, const QByteArray &category,  int maxSplits, QObject *parent = nullptr);

    const QByteArray &category() const;

    const QVector<MTCPSocket *> &performerPrinters() const;

    const QByteArray &name() const;

    void setPerformerPrinters(const QVector<MTCPSocket *> &newPerformerPrinters);

    void executeTask(const QVector<MTCPSocket *> &newPerformerPrinters);
    void updateState(mUser *performer, int currentState);

    int maxSplits() const;

    const QVector<mUser *> &performerPrintersData() const;
    void setPerformerPrintersData(const QVector<mUser *> &newPerformerPrintersData);

    const QByteArray getPerformerPrintersNames() const;

    float progress() const;

    static QByteArray enumToString (const mWorkType value);
    static mWorkType stringToEnum (const QByteArray &key);


    mWorkType workType() const;

    int actualSplits() const;

private:
    QByteArray m_name;
    int m_maxSplits;
    QByteArray m_category;
    QVector<MTCPSocket*> m_performerPrinters;
    QVector<mUser*> m_performerPrintersData;

    float m_progress = 0;
    mWorkType m_workType;
    int m_actualSplits = 0;

signals:

};

#endif // MPRINTERJOB_H
