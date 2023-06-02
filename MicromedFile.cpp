#include "MicromedFile.h"
#include "Utility.h"

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

        //== Get Montages Area info
        int positionFile = 288;
        sr.seekg(positionFile + 8);
        sr.read((char *)&m_montageStartOffset, sizeof(uint32_t));
        sr.seekg(positionFile + 12);
        sr.read((char *)&m_montageLength, sizeof(uint32_t));

        //== Read all montages
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

void MicromedFile::AnonymizeHeaderData(std::string name, std::string surname, unsigned char d, unsigned char m, unsigned char y)
{
    m_surname = surname;
    m_name = name;
    m_day = d;
    m_month = m;
    m_year = y;
    m_recordDay = 1;
    m_recordMonth = 1;
    m_recordYear = 1;
}

void MicromedFile::SaveAnonymizedData(bool overwrite)
{
    std::string outputFile = overwrite ? m_filePath : m_anonFilePath;
    std::fstream writeStream;
    writeStream.open(outputFile.c_str(), std::ios::binary | std::ios::in | std::ios::out );
    if(writeStream.is_open())
    {
        //Correct Header Data
        writeStream.seekp(64, std::ios::beg);
        Utility::WriteCompleteString(writeStream, m_surname, 22);
        writeStream.seekp(86, std::ios::beg);
        Utility::WriteCompleteString(writeStream, m_name, 20);
        writeStream.seekp(106, std::ios::beg);
        writeStream << m_month;
        writeStream.seekp(107, std::ios::beg);
        writeStream << m_day;
        writeStream.seekp(108, std::ios::beg);
        writeStream << m_year;
        //=== Update montage area length
        writeStream.seekp(300, std::ios::beg);
        uint32_t sizeMontage = m_montagesList.size() * 4096;
        writeStream.write((char const *)&sizeMontage, sizeof(uint32_t));
        //===
        //Correct Montages Data
        //First overwrite the whole space
        writeStream.seekp(m_montageStartOffset, std::ios::beg);
        for (int i = 0; i < MAX_MONT; i++)
        {
            for(int j = 0; j < 4096; j++)
            {
                writeStream << (char)0;
            }
        }

        //Then write montages that were kept
        writeStream.seekp(m_montageStartOffset, std::ios::beg);
        for (int i = 0; i < m_montagesList.size(); i++)
        {
            writeStream.seekp(m_montageStartOffset + (4096 * i), std::ios::beg);
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
            writeStream.write((char const *)&m_montagesList[i].free, sizeof(unsigned char[1720]));
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
    m_surname = Utility::BinaryStringExtraction(sr, 64, 22);
    m_name = Utility::BinaryStringExtraction(sr, 86, 20);
    m_month = Utility::BinaryCharExtraction(sr, 106);
    m_day = Utility::BinaryCharExtraction(sr, 107);
    m_year = Utility::BinaryCharExtraction(sr, 108);
}

void MicromedFile::GetMontages(std::ifstream &fileStream, int startOffset, int length, std::vector<montagesOfTrace> &m_montagesList)
{
    int montageOffset = 0;
    int montageCount = length / 4096 < MAX_MONT ? length / 4096 : MAX_MONT;

    for (int i = 0; i < montageCount; i++)
    {
        montageOffset = i * 4096;

        montagesOfTrace montage;
        montage.lines = (unsigned short)Utility::BinaryBytesExtraction(fileStream, startOffset + montageOffset, 2);
        montage.sectors = (unsigned short)Utility::BinaryBytesExtraction(fileStream, startOffset + montageOffset + 2, 2);
        montage.baseTime = (unsigned short)Utility::BinaryBytesExtraction(fileStream, startOffset + montageOffset + 4, 2);
        montage.notch = (unsigned short)Utility::BinaryBytesExtraction(fileStream, startOffset + montageOffset + 6, 2);

        fileStream.seekg(startOffset + montageOffset + 8);
        fileStream.read((char *)&montage.colour, sizeof(unsigned char[128]));
        fileStream.seekg(startOffset + montageOffset + 136);
        fileStream.read((char *)&montage.selection, sizeof(unsigned char[128]));
        fileStream.read((char *)&montage.description, sizeof(char[64]));
        for (int j = 0; j < MAX_CAN_VIEW; j++)
        {
            montage.inputs[j].nonInverting = (unsigned short)Utility::BinaryBytesExtraction(fileStream, startOffset + montageOffset + 328 + (4 * j), 2);
            montage.inputs[j].inverting = (unsigned short)Utility::BinaryBytesExtraction(fileStream, startOffset + montageOffset + 330 + (4 * j), 2);
        }
        fileStream.seekg(startOffset + montageOffset + 840);
        fileStream.read((char *)&montage.highPassFilter, sizeof(uint32_t[128]));
        fileStream.seekg(startOffset + montageOffset + 1352);
        fileStream.read((char *)&montage.lowPassFilter, sizeof(uint32_t[128]));
        fileStream.seekg(startOffset + montageOffset + 1864);
        fileStream.read((char *)&montage.reference, sizeof(uint32_t[128]));

        for (int j = 0; j < 1720; j++)
            montage.free[j] = Utility::BinaryCharExtraction(fileStream, startOffset + montageOffset + 2376 + j);

        m_montagesList.push_back(montage);
    }
}
