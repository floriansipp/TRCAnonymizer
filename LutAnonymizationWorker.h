#ifndef LUTANONYMIZATIONWORKER_H
#define LUTANONYMIZATIONWORKER_H

#include <QCoreApplication>
#include <QObject>
#include <QHash>
#include <QDate>
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
    LutAnonymizationWorker(std::vector<std::string> files, QHash<std::string, std::string> lut, bool overwriteOriginal,
                           AnonymizationMode mode = AnonymizationMode::Pseudonymization,
                           bool preserveTimeline = false, bool useAutoReference = true,
                           QDate referenceDate = QDate(), std::string noteFilter = "");
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
    AnonymizationMode m_mode = AnonymizationMode::Pseudonymization;
    bool m_preserveTimeline = false;
    bool m_useAutoReference = true;
    QDate m_referenceDate;
    std::string m_noteFilter;
};

#endif // LUTANONYMIZATIONWORKER_H
