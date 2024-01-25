#ifndef IFILE_H
#define IFILE_H

#include <string>
#include <vector>
#include "GenericMontage.h"

enum class FileType { Micromed, EuropeanDataFormat, Unknown };

class IFile
{
public:
    virtual ~IFile() = 0;
    //Getters/Setters
    inline std::string FilePath() const { return m_filePath; }
    inline std::string AnonFilePath() const { return m_anonFilePath; }
    //==
    inline std::string Name() const { return m_name; }
    inline std::string Surname() const { return m_surname; }
    inline int Day() const { return m_day; }
    inline int Month() const { return m_month; }
    inline int Year() const { return m_year; }
    inline int RecordDay() const { return m_recordDay; }
    inline int RecordMonth() const { return m_recordMonth; }
    inline int RecordYear() const { return m_recordYear; }
    inline int RecordTimeHour() const { return m_recordTimeHour; }
    inline int RecordTimeMinute() const { return m_recordTimeMin; }
    inline int RecordTimeSecond() const { return m_recordTimeSec; }
    inline std::vector<GenericMontage> Montages() { return m_montagesLabels; }

    //===
    inline void Name(const std::string& name) { m_name = name; }
    inline void Surname(const std::string& surname) { m_surname = surname; }
    inline void Day(const int &day) { m_day = day; }
    inline void Month(const int &month) { m_month = month; }
    inline void Year(const int &year) { m_year = year; }
    inline void RecordDay(const int &day) { m_recordDay = day; }
    inline void RecordMonth(const int &month) { m_recordMonth = month; }
    inline void RecordYear(const int &year) { m_recordYear = year; }
    inline void RecordTimeHour(const int &hour) { m_recordTimeHour = hour; }
    inline void RecordTimeMinute(const int &minute) { m_recordTimeMin = minute; }
    inline void RecordTimeSecond(const int &second) { m_recordTimeSec = second; }

    //Functions
    virtual void RemoveMontage(int position) = 0;
    virtual void UpdateMontageLabel(int position, std::string label) = 0;
    virtual void UpdateMontagesData(std::vector<GenericMontage> montages) = 0;
    virtual void AnonymizePatientData(std::string name, std::string surname, int d, int m, int y) = 0;
    //virtual void AnonymizeRecordData(int rd = 1, int rm = 1, int ry = 0, int rth = 1, int rtm = 1, int rts = 1);
    virtual void SaveAnonymizedData(bool overwrite = false) = 0;

protected:
    std::string m_filePath = "";
    std::string m_anonFilePath = "";
    //== Interface info to send back
    std::string m_name = "";
    std::string m_surname = "";
    int m_month = -1;
    int m_day = -1;
    int m_year = -1;
    int m_recordMonth = -1;
    int m_recordDay = -1;
    int m_recordYear = -1;
    int m_recordTimeHour = -1;
    int m_recordTimeMin = -1;
    int m_recordTimeSec = -1;
    std::vector<GenericMontage> m_montagesLabels;
};

#endif // IFILE_H
