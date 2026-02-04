#include "AnonymizationWorker.h"
#include <filesystem>
#include <QFileInfo>

AnonymizationWorker::AnonymizationWorker(std::vector<std::string> files, bool copyAll, IFile* copyFileInfo)
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
        rd = m_templateFile->RecordDay();
        rm = m_templateFile->RecordMonth();
        ry = m_templateFile->RecordYear();
        rth = m_templateFile->RecordTimeHour();
        rtm = m_templateFile->RecordTimeMinute();
        rts = m_templateFile->RecordTimeSecond();
        m_montages = std::vector<GenericMontage>(m_templateFile->Montages());
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
            m_workingFile = IFile::Create(m_files[i]);
            //We update the montages only when processing multiple files since for only one file we work
            //with the data structure of said file directly
            m_workingFile->UpdateMontagesData(m_montages);

            CopyAndAnonFile(m_workingFile.get());
            emit progress((double)i / (m_files.size() - 1));
            m_workingFile.reset();
        }
    }
    else
    {
        CopyAndAnonFile(m_templateFile);
    }
    emit sendLogInfo(QString::fromStdString("Anonymization process finished."));
    emit sendLogInfo(QString::fromStdString(""));

    emit finished();
}

void AnonymizationWorker::CopyAndAnonFile(IFile* f)
{
    if(std::filesystem::exists(f->AnonFilePath()))
    {
        std::filesystem::remove(f->AnonFilePath());
    }
    std::filesystem::copy(f->FilePath(), f->AnonFilePath());
    f->AnonymizePatientData(name, surname, d, m, y);
    f->AnonymizeRecordData(rd, rm, ry, -1, -1, -1);
    f->SaveAnonymizedData();
}
