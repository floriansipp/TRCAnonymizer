#ifndef MICROMEDFILE_H
#define MICROMEDFILE_H

#include <iostream>
#include <iostream>
#include <vector>
#include <fstream>
#include "TRCParameters.h"

class MicromedFile
{
public:
    MicromedFile();
    MicromedFile(std::string filePath);
    ~MicromedFile();

    inline std::string FilePath() const { return m_filePath; }
    inline std::string AnonFilePath() const { return m_anonFilePath; }
    //==
    inline std::string Name() const { return m_name; }
    inline void Name(const std::string& name) { m_name = name; }
    inline std::string Surname() const { return m_surname; }
    inline void Surname(const std::string& surname) { m_surname = surname; }
    inline unsigned char Day() const { return m_day; }
    inline void Day(const int &day) { m_day = static_cast<unsigned char>(day); }
    inline unsigned char Month() const { return m_month; }
    inline void Month(const int &month) { m_month = static_cast<unsigned char>(month); }
    inline unsigned char Year() const { return m_year; }
    inline void Year(const int &year) { m_year = static_cast<unsigned char>(year); }
    inline unsigned char RecordDay() const { return m_recordDay; }
    inline void RecordDay(const int &day) { m_recordDay = static_cast<unsigned char>(day); }
    inline unsigned char RecordMonth() const { return m_recordMonth; }
    inline void RecordMonth(const int &month) { m_recordMonth = static_cast<unsigned char>(month); }
    inline unsigned char RecordYear() const { return m_recordYear; }
    inline void RecordYear(const int &year) { m_recordYear = static_cast<unsigned char>(year); }
    inline unsigned char RecordTimeHour() const { return m_recordTimeHour; }
    inline void RecordTimeHour(const int &hour) { m_recordTimeHour = static_cast<unsigned char>(hour); }
    inline unsigned char RecordTimeMinute() const { return m_recordTimeMin; }
    inline void RecordTimeMinute(const int &minute) { m_recordTimeMin = static_cast<unsigned char>(minute); }
    inline unsigned char RecordTimeSecond() const { return m_recordTimeSec; }
    inline void RecordTimeSecond(const int &second) { m_recordTimeSec = static_cast<unsigned char>(second); }
    //==
    inline std::vector<montagesOfTrace>& Montages() { return m_montagesList; }
    //==
    void AnonymizePatientData(std::string name ="Ymous", std::string surname ="Anon", int d = 1, int m = 1, int y = 0);
    void AnonymizeRecordData(int rd = 1, int rm = 1, int ry = 0, int rth = 1, int rtm = 1, int rts = 1);
    void SaveAnonymizedData(bool overwrite = false);

private:
    void ReadHeader(std::ifstream &sr);
    void GetMontages(std::ifstream &fileStream, int startOffset, int length, std::vector<montagesOfTrace> &montageList);

private:
    std::string m_filePath = "";
    std::string m_anonFilePath = "";
    //==
    std::string m_name = "";
    std::string m_surname = "";
    unsigned char m_month;
    unsigned char m_day;
    unsigned char m_year;
    unsigned char m_recordMonth;
    unsigned char m_recordDay;
    unsigned char m_recordYear;
    unsigned char m_recordTimeHour;
    unsigned char m_recordTimeMin;
    unsigned char m_recordTimeSec;
    //==
    uint32_t m_montageStartOffset;
    uint32_t m_montageLength;
    std::vector<montagesOfTrace> m_montagesList;
};

#endif // MICROMEDFILE_H
