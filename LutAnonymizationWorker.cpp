#include "LutAnonymizationWorker.h"
#include "Utility.h"
#include "MicromedFile.h"
#include <filesystem>

LutAnonymizationWorker::LutAnonymizationWorker(std::vector<std::string> files,  QHash<std::string, std::string> lut, bool overwriteOriginal)
{
    m_files = std::vector<std::string>(files);
    m_lookUpTable = lut;
    m_overwriteOriginal = overwriteOriginal;
}

LutAnonymizationWorker::~LutAnonymizationWorker()
{

}

void LutAnonymizationWorker::Process()
{
    emit sendLogInfo(QString::fromStdString("Starting anonymization process."));
    for(int i = 0; i < m_files.size(); i++)
    {
        std::string file = m_files[i];
        std::string anon = GetAnonValue(file);
        if(anon != "")
        {
            std::vector<std::string> anonSplit = Utility::split<std::string>(anon, "#");
            std::string name = anonSplit[anonSplit.size() - 1];
            std::string surname = anonSplit[0];
            for(int j = 1; j < anonSplit.size() - 1; j++)
            {
                surname += "_" + anonSplit[j];
            }

            //qDebug() << "Should anonymyze file : " << file.c_str() << " with name : " << name.c_str() << " and surname " << surname.c_str();
            //qDebug() << "------";

            MicromedFile f(file);
            if(!m_overwriteOriginal)
            {
                if(std::filesystem::exists(f.AnonFilePath()))
                {
                    std::filesystem::remove(f.AnonFilePath());
                }
                std::filesystem::copy(f.FilePath(), f.AnonFilePath());
            }
            f.AnonymizePatientData(name, surname, 1, 1, 0);
            //f.AnonymizeRecordData();
            f.SaveAnonymizedData(m_overwriteOriginal);
        }
        else
        {
            emit sendLogInfo(QString::fromStdString("Could not find a value for : " + file + ", it will not be processed"));
        }
    }
    emit sendLogInfo(QString::fromStdString("Anonymization process finished."));
    emit sendLogInfo(QString::fromStdString(""));

    emit finished();
}

std::string LutAnonymizationWorker::GetAnonValue(std::string file)
{
    QString path = QString::fromStdString(file).toLower();

    QHashIterator<std::string, std::string> i(m_lookUpTable);
    while (i.hasNext())
    {
        i.next();

        QString key = QString::fromStdString(i.key()).toLower();
        if(path.contains(key))
        {
            //qDebug() << "Key is " << key;
            //qDebug() << "Value is " << i.value().c_str();
            //qDebug() << "Path is " << path;

            return i.value();
        }
    }

    return "";
}
