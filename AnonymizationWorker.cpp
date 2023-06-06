#include "AnonymizationWorker.h"
#include <filesystem>

AnonymizationWorker::AnonymizationWorker(std::vector<std::string> files, bool copyAll, MicromedFile* copyFileInfo)
{
    m_files = std::vector<std::string>(files);
    m_copyDataAll = copyAll;
    m_templateFile = copyFileInfo;
    if(m_templateFile != nullptr)
    {
        name = m_templateFile->Name();
        surname = m_templateFile->Surname();
        d = m_templateFile->Day();
        m = m_templateFile->Month();
        y = m_templateFile->Year();
        m_montages = std::vector<montagesOfTrace>(m_templateFile->Montages());
    }
}

AnonymizationWorker::~AnonymizationWorker()
{

}

void AnonymizationWorker::Process()
{
    emit sendLogInfo(QString::fromStdString("Starting anonymization process."));
    if(m_copyDataAll)
    {
        for(int i = 0; i < m_files.size(); i++)
        {
            emit sendLogInfo(QString::fromStdString("Processing " + m_files[i]));
            MicromedFile f(m_files[i]);
            CopyAndAnonFile(f);
            emit progress((double)i / (m_files.size() - 1));
        }
    }
    else
    {
        CopyAndAnonFile(*m_templateFile);
    }
    emit sendLogInfo(QString::fromStdString("Anonymization process finished."));
    emit sendLogInfo(QString::fromStdString(""));

    emit finished();
}

void AnonymizationWorker::CopyAndAnonFile(MicromedFile f)
{
    if(std::filesystem::exists(f.AnonFilePath()))
    {
        std::filesystem::remove(f.AnonFilePath());
    }
    std::filesystem::copy(f.FilePath(), f.AnonFilePath());
    f.AnonymizeHeaderData(name, surname, d, m, y);
    f.Montages() = std::vector<montagesOfTrace>(m_montages);
    f.SaveAnonymizedData();
}
