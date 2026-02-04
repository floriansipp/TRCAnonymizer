#include "InformationExtractionWorker.h"
#include <QFileInfo>
#include "Utility.h"

InformationExtractionWorker::InformationExtractionWorker(QString csvPath, std::vector<std::string> files)
{
    m_csvPath = csvPath;
    m_files = std::vector<std::string>(files);
}

InformationExtractionWorker::~InformationExtractionWorker()
{

}

void InformationExtractionWorker::Process()
{
    emit sendLogInfo(QString::fromStdString("Starting information extraction process."));

    QFile file(m_csvPath);
    if (file.open(QFile::WriteOnly))
    {
        QTextStream stream(&file);
        stream << "File_Path;Name;Surname;Birth_Date;Record_Date;Record_Time" << "\n";
        for(int i = 0; i < m_files.size(); i++)
        {
            auto eegFile = IFile::Create(m_files[i]);
            if (eegFile == nullptr) continue;

            stream << QString::fromStdString(eegFile->FilePath()) << ";"
                   << QString::fromStdString(eegFile->Name()) << ";"
                   << QString::fromStdString(eegFile->Surname()) << ";"
                   << QString::number(eegFile->Day()) << "." << QString::number(eegFile->Month()) << "." <<QString::number(eegFile->Year()) << ";"
                   << QString::number(eegFile->RecordDay()) << "." << QString::number(eegFile->RecordMonth()) << "." <<QString::number(eegFile->RecordYear()) << ";"
                   << QString::number(eegFile->RecordTimeHour()) << "h" << QString::number(eegFile->RecordTimeMinute()) << "m" <<QString::number(eegFile->RecordTimeSecond())
                   << "\n";
        }
        file.close();
    }

    emit sendLogInfo(QString::fromStdString("Information extraction process finished."));
    emit sendLogInfo(QString::fromStdString(""));

    emit finished();
}
