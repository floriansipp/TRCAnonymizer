#ifndef LUTANONYMIZATIONWORKER_H
#define LUTANONYMIZATIONWORKER_H

#include <QCoreApplication>
#include <QObject>
#include <QHash>
#include <vector>
#include <QString>
#include "IFile.h"

// Qt6 requires qHash for std::string when using QHash<std::string, ...>
inline size_t qHash(const std::string &key, size_t seed = 0) {
    return qHash(QByteArray::fromRawData(key.data(), static_cast<qsizetype>(key.size())), seed);
}

class LutAnonymizationWorker : public QObject
{
    Q_OBJECT

public:
    LutAnonymizationWorker(std::vector<std::string> files,  QHash<std::string, std::string> lut, bool overwriteOriginal);
    ~LutAnonymizationWorker();
    void Process();

private:
    std::string GetAnonValue(std::string file);
    IFile* GetFile(std::string path);

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
