#include "Utility.h"

#include <iostream>
#include <map>
#include <sstream>
#include <iomanip> // for std::setfill and std::setw


unsigned char Utility::BinaryCharExtraction(std::ifstream &sr, int positionInFile)
{
    unsigned char buf;
    sr.clear();
    sr.seekg(positionInFile, std::ios::beg);
    buf = (unsigned char)(sr.get());
    return buf;
}

uint32_t Utility::BinaryBytesExtraction(std::ifstream &sr, int positionInFile, int numberOfBytes)
{
    uint32_t valueOfBytes = 0;
    unsigned char ucharValue;
    int count = 0;

    /*According to count value, we shift ucharValue from 8,16,24,32 ...*/
    while (count < numberOfBytes)
    {
        ucharValue = BinaryCharExtraction(sr, positionInFile);
        valueOfBytes = valueOfBytes + (uint32_t)(ucharValue << (8 * count));

        count++;
        positionInFile++;
    }
    return valueOfBytes;
}

std::string Utility::BinaryStringExtraction(std::ifstream &sr, int positionInFile, int numberOfChar)
{
    char charValue = ' ';
    std::vector<char> myExtractedString;

    sr.clear();
    sr.seekg(positionInFile, std::ios::beg);
    for (int i = 0; i < numberOfChar; i++)
    {
        sr.get(charValue);
        if (charValue >= 0)
        {
            myExtractedString.push_back(charValue);
        }
    }
    return std::string(myExtractedString.begin(), myExtractedString.end());
}

void Utility::WriteCompleteString(std::fstream& outputFileStream, std::string strToWrite, int size, std::string endStringChars)
{
    outputFileStream << (char *)strToWrite.c_str();
    for (int i = strToWrite.size(); i < size - endStringChars.size(); i++)
    {
        outputFileStream << " ";
    }
    outputFileStream << endStringChars;
}

std::vector<std::string> Utility::ReadTxtFile(std::string pathFile)
{
    std::stringstream buffer;
    std::ifstream fileStream(pathFile);
    if (fileStream)
    {
        buffer << fileStream.rdbuf();
        fileStream.close();
        return(split<std::string>(buffer.str(), "\r\n"));
    }
    else
    {
        std::cout << " Error opening : " << pathFile << std::endl;
        return std::vector<std::string>();
    }
}

int Utility::MonthStrToNumber(std::string month)
{
    static const std::map<std::string, int> months
        {
            { "Jan", 1 }, { "JAN", 1 },
            { "Feb", 2 }, { "FEB", 2 },
            { "Mar", 3 }, { "MAR", 3 },
            { "Apr", 4 }, { "APR", 4 },
            { "May", 5 }, { "MAY", 5 },
            { "Jun", 6 }, { "JUN", 6 },
            { "Jul", 7 }, { "JUL", 7 },
            { "Aug", 8 }, { "AUG", 8 },
            { "Sep", 9 }, { "SEP", 9 },
            { "Oct", 10 }, { "OCT", 10 },
            { "Nov", 11 }, { "NOV", 11 },
            { "Dec", 12 }, { "DEC", 12 }
        };

    const auto iter(months.find(month));

    return (iter != std::cend(months)) ? iter->second : -1;
}

std::string Utility::FormatEdfDate(int d, int m, int y)
{
    std::ostringstream oss;
    const char* months[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun",
                            "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};

    if (m < 1 || m > 12) {
        return "X";
    }

    // y is now the full year (e.g., 1990, 2020)
    oss << std::setfill('0') << std::setw(2) << d << "-"
        << months[m - 1] << "-"
        << y;

    return oss.str();
}
