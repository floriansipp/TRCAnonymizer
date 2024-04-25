#ifndef DUPLICATECHECKWORKER_H
#define DUPLICATECHECKWORKER_H

#include <QCoreApplication>
#include <QObject>
#include <QString>

class DuplicateCheckWorker : public QObject
{
    Q_OBJECT

public:
    DuplicateCheckWorker(QString rootDir);
    ~DuplicateCheckWorker();
    void Process();

signals:
    void finished();
    void sendLogInfo(QString s);
    void sendErrorLogInfo(QString s);
    void progress(double percentage);

private:
    QString m_rootDir = "";
};

#endif // DUPLICATECHECKWORKER_H
