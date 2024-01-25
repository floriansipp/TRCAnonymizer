#include "EdfFile.h"

#include <iostream>
#include <vector>
#include "Utility.h"

EdfFile::EdfFile()
{

}

EdfFile::EdfFile(std::string filepath)
{
    m_filePath = filepath;
    m_anonFilePath = filepath;
    m_anonFilePath.replace(m_anonFilePath.end() - 4, m_anonFilePath.end(), "_anon.edf");

    try
    {
        std::ifstream sr(m_filePath, std::ios::binary);
        //== Read header basic informations
        ReadHeader(sr);
        DisplayHeaderInfo();
        //==
        sr.close();
    }
    catch (std::system_error& e)
    {
        std::cerr << "Error opening .edf File" << std::endl;
        std::cerr << e.code().message().c_str() << std::endl;
    }
}

EdfFile::~EdfFile()
{

}

void EdfFile::AnonymizePatientData(std::string name, std::string surname, int d, int m, int y)
{
    m_surname = surname;
    m_name = name;
    m_day = d;
    m_month = m;
    m_year = y;
}

void EdfFile::SaveAnonymizedData(bool overwrite)
{
    std::string outputFile = overwrite ? m_filePath : m_anonFilePath;
    std::fstream writeStream;
    writeStream.open(outputFile.c_str(), std::ios::binary | std::ios::in | std::ios::out );
    if(writeStream.is_open())
    {
        //Correct Header Data
        std::string anonDate = Utility::FormatEdfDate(m_day, m_month, m_year);
        m_patientID = std::string("X X ") + anonDate + " " + m_name + "_" + m_surname;
        writeStream.seekp(8, std::ios::beg);
        Utility::WriteCompleteString(writeStream, m_patientID, 80);

        std::string anonRecordDate = Utility::FormatEdfDate(m_recordDay, m_recordMonth, m_recordYear);
        m_recordingID = "StartDate " + anonRecordDate + " " + "X X X";
        writeStream.seekp(88, std::ios::beg);
        Utility::WriteCompleteString(writeStream, m_recordingID, 80);

        //Close file
        writeStream.close();
    }
    else
    {
        std::cerr << "Error while opening file : " << m_anonFilePath.c_str();
    }
}

void EdfFile::ReadHeader(std::ifstream &sr)
{
    m_version = std::stoi(Utility::BinaryStringExtraction(sr, 0, 8));
    m_patientID = Utility::BinaryStringExtraction(sr, 8, 80);
    ParsePatientIDField(m_patientID);
    m_recordingID  = Utility::BinaryStringExtraction(sr, 88, 80);
    m_startDate = Utility::BinaryStringExtraction(sr, 168, 8);
    ParseStartDateField(m_startDate);
    m_startTime = Utility::BinaryStringExtraction(sr, 176, 8);
    ParseStartTimeField(m_startTime);
    m_bytesNumber = std::stoi(Utility::BinaryStringExtraction(sr, 184, 8));
    m_reserved = Utility::BinaryStringExtraction(sr, 192, 44);
    m_recordsNumber = std::stoi(Utility::BinaryStringExtraction(sr, 236, 8));
    m_recordDuration = std::stoi(Utility::BinaryStringExtraction(sr, 244, 8));
    m_signalNumber = std::stoi(Utility::BinaryStringExtraction(sr, 252, 4));
}

void EdfFile::ParsePatientIDField(std::string patientID)
{
    std::vector<std::string> patientIDSplit = Utility::split<std::string>(patientID, " ");
    if(patientIDSplit.size() == 3) //Micromed conversion
    {
        m_name = patientIDSplit[0];
        m_surname = patientIDSplit[1];
        std::string birthdate = patientIDSplit[2];
        std::vector<std::string> birthdateSplit = Utility::split<std::string>(birthdate, "-");
        if(birthdateSplit.size() == 3)
        {
            m_day = std::stoi(birthdateSplit[0]);
            m_month = Utility::MonthStrToNumber(birthdateSplit[1]);
            m_year = std::stoi(birthdateSplit[2]);
        }
    }
    else if(patientIDSplit.size() == 4) //Edf specification
    {
        std::string hospitalCode = patientIDSplit[0];
        std::string sex = patientIDSplit[1];
        //===
        std::string birthdate = patientIDSplit[2];
        std::vector<std::string> birthdateSplit = Utility::split<std::string>(birthdate, "-");
        if(birthdateSplit.size() == 3)
        {
            m_day = std::stoi(birthdateSplit[0]);
            m_month = Utility::MonthStrToNumber(birthdateSplit[1]);
            m_year = std::stoi(birthdateSplit[2]);
        }
        //===
        std::string patientName = patientIDSplit[3];
        std::vector<std::string> patientNameSplit = Utility::split<std::string>(patientName, "_");
        if(patientNameSplit.size() == 2)
        {
            m_name = patientNameSplit[0];
            m_surname = patientNameSplit[1];
        }
    }
    else
    {
        std::cout << "Warning : patientIDSplit.size is not 3 or 4" << std::endl;
    }
}

void EdfFile::ParseStartDateField(std::string startDate)
{
    std::vector<std::string> startDateSplit = Utility::split<std::string>(startDate, ".");
    if(startDateSplit.size() == 3)
    {
        m_recordDay = std::stoi(startDateSplit[0]);
        m_recordMonth =std::stoi(startDateSplit[1]);
        m_recordYear = std::stoi(startDateSplit[2]);
    }
}

void EdfFile::ParseStartTimeField(std::string startTime)
{
    std::vector<std::string> startTimeSplit = Utility::split<std::string>(startTime, ".");
    if(startTimeSplit.size() == 3)
    {
        m_recordTimeHour = std::stoi(startTimeSplit[0]);
        m_recordTimeMin = std::stoi(startTimeSplit[1]);
        m_recordTimeSec = std::stoi(startTimeSplit[2]);
    }
}

void EdfFile::DisplayHeaderInfo()
{
    std::cout << "Version : " << m_version << std::endl;
    std::cout << "PatientID : " << m_patientID << std::endl;
    std::cout << "-----------" << std::endl;
    std::cout << "  Name : " << m_name << std::endl;
    std::cout << "  Surname : " << m_surname << std::endl;
    std::cout << "  Day : " << m_day << std::endl;
    std::cout << "  Month : " << m_month << std::endl;
    std::cout << "  Year : " << m_year << std::endl;
    std::cout << "-----------" << std::endl;
    std::cout << "RecordingID : " << m_recordingID << std::endl;
    std::cout << "StartDate : " << m_startDate << std::endl;
    std::cout << "-----------" << std::endl;
    std::cout << "  StartDay : "<< m_recordDay << std::endl;
    std::cout << "  StartMonth : "<< m_recordMonth << std::endl;
    std::cout << "  StartYear : "<< m_recordYear << std::endl;
    std::cout << "-----------" << std::endl;
    std::cout << "StartTime : " << m_startTime << std::endl;
    std::cout << "-----------" << std::endl;
    std::cout << "  StartTimeHour : "<< m_recordTimeHour << std::endl;
    std::cout << "  StartTimeMinute : "<< m_recordTimeMin << std::endl;
    std::cout << "  StartTimeSecond : "<< m_recordTimeSec << std::endl;
    std::cout << "-----------" << std::endl;
    std::cout << "BytesNumber : " << m_bytesNumber << std::endl;
    std::cout << "Reserved : " << m_reserved << std::endl;
    std::cout << "RecordsNumber : " << m_recordsNumber << std::endl;
    std::cout << "RecordsDuration : " << m_recordDuration << std::endl;
    std::cout << "SignalNumber : " << m_signalNumber << std::endl;
    std::cout << "----" << std::endl;
}
