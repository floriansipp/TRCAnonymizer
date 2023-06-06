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
    inline std::string Surname() const { return m_surname; }
    inline unsigned char Month() const { return m_month; }
    inline unsigned char Day() const { return m_day; }
    inline unsigned char Year() const { return m_year; }
    //==
    inline std::vector<montagesOfTrace>& Montages() { return m_montagesList; }
    //==
    void AnonymizeHeaderData(std::string name ="Ymous", std::string surname ="Anon", int d = 1, int m = 1, int y = 0);
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
    //==
    uint32_t m_montageStartOffset;
    uint32_t m_montageLength;
    std::vector<montagesOfTrace> m_montagesList;
};

#endif // MICROMEDFILE_H
