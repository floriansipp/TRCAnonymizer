#include "MicromedFile.h"
#include "Utility.h"
#include <QString>

MicromedFile::MicromedFile()
{
}

MicromedFile::MicromedFile(std::string filepath)
{
    m_filePath = filepath;
    m_anonFilePath = filepath;
    m_anonFilePath.replace(m_anonFilePath.end() - 4, m_anonFilePath.end(), "_anon.trc");

    try
    {
        std::ifstream sr(m_filePath, std::ios::binary);
        //== Read header basic informations
        ReadHeader(sr);

        //== Get Notes Area info
        sr.seekg(TRC::NOTES_AREA_DESCRIPTOR_OFFSET + TRC::AREA_START_OFFSET_DELTA);
        sr.read((char *)&m_notesStartOffset, sizeof(uint32_t));
        sr.seekg(TRC::NOTES_AREA_DESCRIPTOR_OFFSET + TRC::AREA_LENGTH_DELTA);
        sr.read((char *)&m_notesLength, sizeof(uint32_t));

        //== Get Montages Area info
        sr.seekg(TRC::MONTAGE_AREA_DESCRIPTOR_OFFSET + TRC::AREA_START_OFFSET_DELTA);
        sr.read((char *)&m_montageStartOffset, sizeof(uint32_t));
        sr.seekg(TRC::MONTAGE_AREA_DESCRIPTOR_OFFSET + TRC::AREA_LENGTH_DELTA);
        sr.read((char *)&m_montageLength, sizeof(uint32_t));

        //== Read all montages
        GetNotes(sr, m_notesStartOffset, m_notesLength, m_notesList);
        GetMontages(sr, m_montageStartOffset, m_montageLength, m_montagesList);

        //==
        sr.close();
    }
    catch (std::system_error& e)
    {
        std::cerr << "Error opening .TRC File" << std::endl;
        std::cerr << e.code().message().c_str() << std::endl;
    }
}

MicromedFile::~MicromedFile()
{

}

void MicromedFile::UpdateNote(int position, int sample, std::string description)
{
    int oldSample = m_notesList[position].Sample();
    m_notesList[position].Sample(sample);
    m_notesList[position].Description(description);
    if (sample > oldSample)
    {
        for (auto it = m_notesList.begin() + position + 1; it < m_notesList.end(); ++it)
        {
            if (sample > it->Sample())
            {
                std::iter_swap(it - 1, it);
            }
            else
            {
                break;
            }
        }
    }
    else if (oldSample > sample)
    {
        int rposition = m_notesList.size() - 1 - position;
        for (auto it = m_notesList.rbegin() + rposition + 1; it < m_notesList.rend(); ++it)
        {
            if (sample < it->Sample())
            {
                std::iter_swap(it - 1, it);
            }
            else
            {
                break;
            }
        }
    }
}

void MicromedFile::RemoveMontage(int position)
{
    if(position < m_montagesList.size())
    {
        m_montagesLabels.erase(m_montagesLabels.begin() + position);
        m_montagesList.erase(m_montagesList.begin() + position);
    }
}

void MicromedFile::UpdateMontageLabel(int position, std::string label)
{
    if(position < m_montagesList.size())
    {
        m_montagesLabels[position].Name(label);
        std::strncpy(m_montagesList[position].description, label.c_str(), TRC::MONTAGE_DESC_LENGTH);
    }
}

void MicromedFile::UpdateMontagesData(std::vector<GenericMontage> montages)
{
    std::vector<montagesOfTrace> updatedList;
    for(int i = 0; i < montages.size(); i++)
    {
        updatedList.push_back(m_montagesList[montages[i].InitialPosition()]);
        std::strncpy(updatedList[i].description, montages[i].Name().c_str(), TRC::MONTAGE_DESC_LENGTH);
    }

    m_montagesList = std::vector<montagesOfTrace>(updatedList);
}

void MicromedFile::AnonymizePatientData(std::string name, std::string surname, int d, int m, int y)
{
    m_surname = surname;
    m_name = name;
    m_day = d;
    m_month = m;
    m_year = y;
}

void MicromedFile::AnonymizeRecordData(int rd, int rm, int ry, int rth, int rtm, int rts)
{
    m_recordDay = rd;
    m_recordMonth = rm;
    m_recordYear = ry;
    // Only update time if explicitly set (not -1 means preserve original)
    if (rth != -1) m_recordTimeHour = rth;
    if (rtm != -1) m_recordTimeMin = rtm;
    if (rts != -1) m_recordTimeSec = rts;
}

void MicromedFile::SaveAnonymizedData(bool overwrite)
{
    std::string outputFile = overwrite ? m_filePath : m_anonFilePath;
    std::fstream writeStream;
    writeStream.open(outputFile.c_str(), std::ios::binary | std::ios::in | std::ios::out );
    if(writeStream.is_open())
    {
        //Correct Header Data
        writeStream.seekp(TRC::SURNAME_OFFSET, std::ios::beg);
        Utility::WriteCompleteString(writeStream, m_surname, TRC::SURNAME_LENGTH);
        writeStream.seekp(TRC::NAME_OFFSET, std::ios::beg);
        Utility::WriteCompleteString(writeStream, m_name, TRC::NAME_LENGTH);
        writeStream.seekp(TRC::BIRTH_MONTH_OFFSET, std::ios::beg);
        writeStream << static_cast<unsigned char>(m_month);
        writeStream.seekp(TRC::BIRTH_DAY_OFFSET, std::ios::beg);
        writeStream << static_cast<unsigned char>(m_day);
        writeStream.seekp(TRC::BIRTH_YEAR_OFFSET, std::ios::beg);
        writeStream << static_cast<unsigned char>(m_year - TRC::YEAR_BASE);
        writeStream.seekp(TRC::RECORD_DAY_OFFSET, std::ios::beg);
        writeStream << static_cast<unsigned char>(m_recordDay);
        writeStream.seekp(TRC::RECORD_MONTH_OFFSET, std::ios::beg);
        writeStream << static_cast<unsigned char>(m_recordMonth);
        writeStream.seekp(TRC::RECORD_YEAR_OFFSET, std::ios::beg);
        writeStream << static_cast<unsigned char>(m_recordYear - TRC::YEAR_BASE);
        writeStream.seekp(TRC::RECORD_TIME_HOUR_OFFSET, std::ios::beg);
        writeStream << static_cast<unsigned char>(m_recordTimeHour);
        writeStream.seekp(TRC::RECORD_TIME_MIN_OFFSET, std::ios::beg);
        writeStream << static_cast<unsigned char>(m_recordTimeMin);
        writeStream.seekp(TRC::RECORD_TIME_SEC_OFFSET, std::ios::beg);
        writeStream << static_cast<unsigned char>(m_recordTimeSec);

        //=== Update montage area length
        writeStream.seekp(TRC::MONTAGE_AREA_LENGTH_OFFSET, std::ios::beg);
        uint32_t sizeMontage = m_montagesList.size() * TRC::MONTAGE_SIZE;
        writeStream.write((char const *)&sizeMontage, sizeof(uint32_t));
        //===
        //Correct Montages Data
        //First overwrite the whole space
        writeStream.seekp(m_montageStartOffset, std::ios::beg);
        for (int i = 0; i < MAX_MONT; i++)
        {
            for(int j = 0; j < TRC::MONTAGE_SIZE; j++)
            {
                writeStream << (char)0;
            }
        }

        //Then write montages that were kept
        writeStream.seekp(m_montageStartOffset, std::ios::beg);
        for (int i = 0; i < m_montagesList.size(); i++)
        {
            writeStream.seekp(m_montageStartOffset + (TRC::MONTAGE_SIZE * i), std::ios::beg);
            writeStream.write((char const *)&m_montagesList[i].lines, sizeof(unsigned short));
            writeStream.write((char const *)&m_montagesList[i].sectors, sizeof(unsigned short));
            writeStream.write((char const *)&m_montagesList[i].baseTime, sizeof(unsigned short));
            writeStream.write((char const *)&m_montagesList[i].notch, sizeof(unsigned short));
            writeStream.write((char const *)&m_montagesList[i].colour, sizeof(unsigned char[128]));
            writeStream.write((char const *)&m_montagesList[i].selection, sizeof(unsigned char[128]));
            writeStream.write((char const *)&m_montagesList[i].description, sizeof(char[64]));
            for (int j = 0; j < MAX_CAN_VIEW; j++)
            {
                writeStream.write((char const *)&m_montagesList[i].inputs[j].nonInverting, sizeof(unsigned short));
                writeStream.write((char const *)&m_montagesList[i].inputs[j].inverting, sizeof(unsigned short));
            }
            for (int j = 0; j < MAX_CAN_VIEW; j++)
            {
                writeStream.write((char const *)&m_montagesList[i].highPassFilter[j], sizeof(uint32_t));
            }
            for (int j = 0; j < MAX_CAN_VIEW; j++)
            {
                writeStream.write((char const *)&m_montagesList[i].lowPassFilter[j], sizeof(uint32_t));
            }
            for (int j = 0; j < MAX_CAN_VIEW; j++)
            {
                writeStream.write((char const *)&m_montagesList[i].reference[j], sizeof(uint32_t));
            }
            writeStream.write((char const *)&m_montagesList[i].free, sizeof(unsigned char[TRC::MONTAGE_FREE_SIZE]));
        }

        //=== Write Notes Data
        //First overwrite the whole notes space
        writeStream.seekp(m_notesStartOffset, std::ios::beg);
        for (int i = 0; i < MAX_NOTE * TRC::NOTE_SIZE; i++)
        {
            writeStream << (char)0;
        }

        //Then write notes that were kept
        for (int i = 0; i < m_notesList.size(); i++)
        {
            writeStream.seekp(m_notesStartOffset + (i * TRC::NOTE_SIZE), std::ios::beg);
            int32_t sample = m_notesList[i].Sample();
            writeStream.write((char const *)&sample, sizeof(int32_t));

            std::string desc = m_notesList[i].Description();
            char descBuffer[TRC::NOTE_DESC_LENGTH] = {0};
            std::strncpy(descBuffer, desc.c_str(), TRC::NOTE_DESC_LENGTH);
            writeStream.write(descBuffer, TRC::NOTE_DESC_LENGTH);
        }

        //Close file
        writeStream.close();
    }
    else
    {
        std::cerr << "Error while opening file : " << m_anonFilePath.c_str();
    }
}

void MicromedFile::ReadHeader(std::ifstream &sr)
{
    m_surname = Utility::BinaryStringExtraction(sr, TRC::SURNAME_OFFSET, TRC::SURNAME_LENGTH);
    m_name = Utility::BinaryStringExtraction(sr, TRC::NAME_OFFSET, TRC::NAME_LENGTH);
    m_month = Utility::BinaryCharExtraction(sr, TRC::BIRTH_MONTH_OFFSET);
    m_day = Utility::BinaryCharExtraction(sr, TRC::BIRTH_DAY_OFFSET);
    m_year = TRC::YEAR_BASE + Utility::BinaryCharExtraction(sr, TRC::BIRTH_YEAR_OFFSET);
    m_recordDay = Utility::BinaryCharExtraction(sr, TRC::RECORD_DAY_OFFSET);
    m_recordMonth = Utility::BinaryCharExtraction(sr, TRC::RECORD_MONTH_OFFSET);
    m_recordYear = TRC::YEAR_BASE + Utility::BinaryCharExtraction(sr, TRC::RECORD_YEAR_OFFSET);
    m_recordTimeHour = Utility::BinaryCharExtraction(sr, TRC::RECORD_TIME_HOUR_OFFSET);
    m_recordTimeMin = Utility::BinaryCharExtraction(sr, TRC::RECORD_TIME_MIN_OFFSET);
    m_recordTimeSec = Utility::BinaryCharExtraction(sr, TRC::RECORD_TIME_SEC_OFFSET);
    m_samplingRate = (uint16_t)Utility::BinaryBytesExtraction(sr, TRC::SAMPLING_RATE_OFFSET, TRC::SAMPLING_RATE_SIZE);
}

void MicromedFile::GetNotes(std::ifstream &fileStream, int startOffset, int length, std::vector<operatorNote> &notesList)
{
    for (int i = 0; i < MAX_NOTE; i++)
    {
        int noteOffset = i * TRC::NOTE_SIZE;

        int32_t sample = (int32_t)Utility::BinaryBytesExtraction(fileStream, startOffset + noteOffset, TRC::NOTE_SAMPLE_SIZE);
        if (sample == 0) // A time of 0000 means that there are no more notes
            return;

        operatorNote currentNote;
        currentNote.Sample(sample);
        currentNote.Description(Utility::BinaryStringExtraction(fileStream, startOffset + noteOffset + TRC::NOTE_DESC_OFFSET, TRC::NOTE_DESC_LENGTH));
        notesList.push_back(currentNote);
    }
}

void MicromedFile::GetMontages(std::ifstream &fileStream, int startOffset, int length, std::vector<montagesOfTrace> &m_montagesList)
{
    int montageCount = length / TRC::MONTAGE_SIZE < MAX_MONT ? length / TRC::MONTAGE_SIZE : MAX_MONT;

    for (int i = 0; i < montageCount; i++)
    {
        int montageOffset = i * TRC::MONTAGE_SIZE;

        montagesOfTrace montage;
        montage.lines = (unsigned short)Utility::BinaryBytesExtraction(fileStream, startOffset + montageOffset, 2);
        montage.sectors = (unsigned short)Utility::BinaryBytesExtraction(fileStream, startOffset + montageOffset + 2, 2);
        montage.baseTime = (unsigned short)Utility::BinaryBytesExtraction(fileStream, startOffset + montageOffset + 4, 2);
        montage.notch = (unsigned short)Utility::BinaryBytesExtraction(fileStream, startOffset + montageOffset + 6, 2);

        fileStream.seekg(startOffset + montageOffset + TRC::MONTAGE_COLOUR_OFFSET);
        fileStream.read((char *)&montage.colour, sizeof(unsigned char[128]));
        fileStream.seekg(startOffset + montageOffset + TRC::MONTAGE_SELECTION_OFFSET);
        fileStream.read((char *)&montage.selection, sizeof(unsigned char[128]));
        fileStream.read((char *)&montage.description, sizeof(char[64]));
        for (int j = 0; j < MAX_CAN_VIEW; j++)
        {
            montage.inputs[j].nonInverting = (unsigned short)Utility::BinaryBytesExtraction(fileStream, startOffset + montageOffset + TRC::MONTAGE_INPUTS_OFFSET + (4 * j), 2);
            montage.inputs[j].inverting = (unsigned short)Utility::BinaryBytesExtraction(fileStream, startOffset + montageOffset + TRC::MONTAGE_INPUTS_OFFSET + 2 + (4 * j), 2);
        }
        fileStream.seekg(startOffset + montageOffset + TRC::MONTAGE_HIGHPASS_OFFSET);
        fileStream.read((char *)&montage.highPassFilter, sizeof(uint32_t[128]));
        fileStream.seekg(startOffset + montageOffset + TRC::MONTAGE_LOWPASS_OFFSET);
        fileStream.read((char *)&montage.lowPassFilter, sizeof(uint32_t[128]));
        fileStream.seekg(startOffset + montageOffset + TRC::MONTAGE_REFERENCE_OFFSET);
        fileStream.read((char *)&montage.reference, sizeof(uint32_t[128]));

        for (int j = 0; j < TRC::MONTAGE_FREE_SIZE; j++)
            montage.free[j] = Utility::BinaryCharExtraction(fileStream, startOffset + montageOffset + TRC::MONTAGE_FREE_OFFSET + j);

        //for the interface, and since montages are a micromed specific thing,
        //we send back a string vector instead of an inherited class
        m_montagesLabels.push_back(GenericMontage(montage.description, i));
        //
        m_montagesList.push_back(montage);

    }
}
