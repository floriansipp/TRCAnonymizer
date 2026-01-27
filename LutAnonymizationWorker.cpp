#include "LutAnonymizationWorker.h"
#include "Utility.h"
#include <filesystem>
#include <QFileInfo>
#include "MicromedFile.h"
#include "EdfFile.h"

LutAnonymizationWorker::LutAnonymizationWorker(std::vector<std::string> files, QHash<std::string, std::string> lut, bool overwriteOriginal,
                                               AnonymizationMode mode, bool preserveTimeline, bool useAutoReference, QDate referenceDate)
{
    m_files = std::vector<std::string>(files);
    m_lookUpTable = lut;
    m_overwriteOriginal = overwriteOriginal;
    m_mode = mode;
    m_preserveTimeline = preserveTimeline;
    m_useAutoReference = useAutoReference;
    m_referenceDate = referenceDate;
}

LutAnonymizationWorker::~LutAnonymizationWorker()
{

}

void LutAnonymizationWorker::Process()
{
    emit sendLogInfo(QString::fromStdString("Starting anonymization process."));

    // For Full Anonymization with timeline preservation and auto-detect,
    // find reference date PER PATIENT FOLDER (each folder = one patient)
    QHash<QString, QDate> folderReferenceDates;  // Map: folder path -> reference date
    QDate baseDate(1900, 1, 1);

    if (m_mode == AnonymizationMode::FullAnonymization && m_preserveTimeline && m_useAutoReference)
    {
        emit sendLogInfo(QString::fromStdString("Auto-detecting reference dates per patient folder..."));

        // First pass: find earliest date per folder
        for (int i = 0; i < m_files.size(); i++)
        {
            QFileInfo fileInfo(QString::fromStdString(m_files[i]));
            QString folderPath = fileInfo.absolutePath();

            IFile* f = GetFile(m_files[i]);
            if (f != nullptr)
            {
                QDate fileDate(f->RecordYear(), f->RecordMonth(), f->RecordDay());
                if (fileDate.isValid())
                {
                    if (!folderReferenceDates.contains(folderPath) || fileDate < folderReferenceDates[folderPath])
                    {
                        folderReferenceDates[folderPath] = fileDate;
                    }
                }
                Utility::DeleteAndNullify(f);
            }
        }

        // Log reference dates per folder
        for (auto it = folderReferenceDates.begin(); it != folderReferenceDates.end(); ++it)
        {
            QFileInfo folderInfo(it.key());
            emit sendLogInfo(QString::fromStdString("  " + folderInfo.fileName().toStdString() + " reference: " + it.value().toString("dd/MM/yyyy").toStdString()));
        }
    }

    // Process each file
    for (int i = 0; i < m_files.size(); i++)
    {
        std::string file = m_files[i];
        std::string name, surname;

        if (m_mode == AnonymizationMode::FullAnonymization)
        {
            // Full anonymization: fixed values
            name = "Ymous";
            surname = "Anon";
        }
        else // Pseudonymization
        {
            std::string anon = GetAnonValue(file);
            if (anon == "")
            {
                emit sendLogInfo(QString::fromStdString("Could not find a value for : " + file + ", it will not be processed"));
                continue;
            }
            std::vector<std::string> anonSplit = Utility::split<std::string>(anon, "#");
            name = anonSplit[anonSplit.size() - 1];
            surname = anonSplit[0];
            for (int j = 1; j < anonSplit.size() - 1; j++)
            {
                surname += "_" + anonSplit[j];
            }
        }

        IFile* f = GetFile(m_files[i]);
        if (f == nullptr)
        {
            emit sendLogInfo(QString::fromStdString("Could not open file: " + file));
            continue;
        }

        if (!m_overwriteOriginal)
        {
            if (std::filesystem::exists(f->AnonFilePath()))
            {
                std::filesystem::remove(f->AnonFilePath());
            }
            std::filesystem::copy(f->FilePath(), f->AnonFilePath());
        }

        // Anonymize patient data (DOB always 1/1/1900)
        f->AnonymizePatientData(name, surname, 1, 1, 1900);

        // Handle recording date based on mode
        if (m_mode == AnonymizationMode::FullAnonymization)
        {
            if (m_preserveTimeline)
            {
                // Get reference date for this file's folder
                QFileInfo fileInfo(QString::fromStdString(file));
                QString folderPath = fileInfo.absolutePath();
                QDate referenceDate = m_useAutoReference ? folderReferenceDates.value(folderPath) : m_referenceDate;

                if (referenceDate.isValid())
                {
                    // Date shifting: calculate new date based on offset from this folder's reference
                    qint64 dateOffset = referenceDate.toJulianDay() - baseDate.toJulianDay();
                    QDate originalDate(f->RecordYear(), f->RecordMonth(), f->RecordDay());
                    QDate shiftedDate = QDate::fromJulianDay(originalDate.toJulianDay() - dateOffset);
                    f->AnonymizeRecordData(shiftedDate.day(), shiftedDate.month(), shiftedDate.year(), -1, -1, -1);
                    emit sendLogInfo(QString::fromStdString("  " + file + " : " + originalDate.toString("dd/MM/yyyy").toStdString() + " -> " + shiftedDate.toString("dd/MM/yyyy").toStdString()));
                }
                else
                {
                    // Fallback: simple mode if no valid reference date
                    f->AnonymizeRecordData(1, 1, 1900, -1, -1, -1);
                }
            }
            else
            {
                // Simple mode: all dates -> 01/01/1900
                f->AnonymizeRecordData(1, 1, 1900, -1, -1, -1);
            }
        }
        // For Pseudonymization, record date is left unchanged (current behavior)

        f->SaveAnonymizedData(m_overwriteOriginal);
        Utility::DeleteAndNullify(f);

        emit progress((double)(i + 1) / m_files.size());
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

        QString key = "/" + QString::fromStdString(i.key()).toLower() + "/";
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

IFile* LutAnonymizationWorker::GetFile(std::string path)
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
