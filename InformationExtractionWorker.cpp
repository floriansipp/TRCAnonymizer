#include "InformationExtractionWorker.h"
#include <QFileInfo>
#include "MicromedFile.h"
#include "EdfFile.h"
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
            IFile *file = GetFile(m_files[i]);

            stream << QString::fromStdString(file->FilePath()) << ";"
                   << QString::fromStdString(file->Name()) << ";"
                   << QString::fromStdString(file->Surname()) << ";"
                   << QString::number(file->Day()) << "." << QString::number(file->Month()) << "." <<QString::number(file->Year()) << ";"
                   << QString::number(file->RecordDay()) << "." << QString::number(file->RecordMonth()) << "." <<QString::number(file->RecordYear()) << ";"
                   << QString::number(file->RecordTimeHour()) << "h" << QString::number(file->RecordTimeMinute()) << "m" <<QString::number(file->RecordTimeSecond())
                   << "\n";

            Utility::DeleteAndNullify(file);
        }
        file.close();
    }

    emit sendLogInfo(QString::fromStdString("Information extraction process finished."));
    emit sendLogInfo(QString::fromStdString(""));

    emit finished();
}

IFile* InformationExtractionWorker::GetFile(std::string path)
{
    QFileInfo f(QString::fromStdString(path));
    if(f.suffix().toLower().contains("trc"))
    {
        return new MicromedFile(path);
    }
    else if(f.suffix().toLower().contains("edf"))
    {
        return new EdfFile(path);
    }
    else
    {
        return nullptr;
    }
}
