#ifndef UTILITY_H
#define UTILITY_H

#include <vector>
#include <fstream>

class Utility
{
public:
    static std::string BinaryStringExtraction(std::ifstream &sr, int positionInFile, int numberOfChar);
    static uint32_t BinaryBytesExtraction(std::ifstream &sr, int positionInFile, int numberOfBytes);
    static unsigned char BinaryCharExtraction(std::ifstream &sr, int positionInFile);
    static void WriteCompleteString(std::fstream& outputFileStream, std::string strToWrite, int size, std::string endStringChars = "");
    static std::vector<std::string> ReadTxtFile(std::string pathFile);
    static int MonthStrToNumber(std::string month);
    static std::string FormatEdfDate(int d, int m, int y);
    /********************************************************************/
    /*	vector<string> v = split<string>("Hello, there; World", ";,");	*/
    /********************************************************************/
    template<typename T>
    static std::vector<T> split(const T & str, const T & delimiters)
    {
        std::vector<T> v;
        typename T::size_type start = 0;
        auto pos = str.find_first_of(delimiters, start);
        while (pos != T::npos) {
            if (pos != start) // ignore empty tokens
                v.emplace_back(str, start, pos - start);
            start = pos + 1;
            pos = str.find_first_of(delimiters, start);
        }
        if (start < str.length()) // ignore trailing delimiter
            v.emplace_back(str, start, str.length() - start); // add what's left of the string
        return v;
    }

private:
    Utility() { }
};

#endif // UTILITY_EEGFORMAT_H
