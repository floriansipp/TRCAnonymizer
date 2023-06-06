#ifndef LUTANONYMIZATIONWORKER_H
#define LUTANONYMIZATIONWORKER_H

#include <QCoreApplication>
#include <QObject>
#include <iostream>
#include <vector>
#include <QString>
#include <filesystem>

class LutAnonymizationWorker : public QObject
{
    Q_OBJECT

public:
    LutAnonymizationWorker(std::vector<std::string> files,  QHash<std::string, std::string> lut, bool overwriteOriginal);
    ~LutAnonymizationWorker();
    void Process();

private:
    std::string GetAnonValue(std::string file);

signals:
    void finished();
    void sendLogInfo(QString s);
    void sendErrorLogInfo(QString s);
    void progress(double percentage);

private:
    std::vector<std::string> m_files;
    QHash<std::string, std::string> m_lookUpTable;
    bool m_overwriteOriginal = false;
};

#endif // LUTANONYMIZATIONWORKER_H
