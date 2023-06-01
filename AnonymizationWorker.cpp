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
    if(m_copyDataAll)
    {
        for(int i = 0; i < m_files.size(); i++)
        {
            MicromedFile f(m_files[i]);
            CopyAndAnonFile(f);
            emit progress((double)i / (m_files.size() - 1));
        }
    }
    else
    {
        CopyAndAnonFile(*m_templateFile);
    }
}

void AnonymizationWorker::CopyAndAnonFile(MicromedFile f)
{
    std::filesystem::copy(f.FilePath(), f.AnonFilePath());
    f.AnonymizeHeaderData(name, surname, d, m, y);
    f.Montages() = std::vector<montagesOfTrace>(m_montages);
    f.SaveAnonymizedData();
}
