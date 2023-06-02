#include "LutAnonymizationWorker.h"
#include "Utility.h"
#include "MicromedFile.h"

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
    for(int i = 0; i < m_files.size(); i++)
    {
        std::string file = m_files[i];
        std::string anon = GetAnonValue(file);
        if(anon != "")
        {
            std::vector<std::string> anonSplit = Utility::split<std::string>(anon, "_");
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
                std::filesystem::copy(f.FilePath(), f.AnonFilePath());
            }
            f.AnonymizeHeaderData(name, surname, 1, 1, 0);
            f.SaveAnonymizedData(m_overwriteOriginal);
        }
    }
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
