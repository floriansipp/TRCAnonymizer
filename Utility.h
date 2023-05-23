#ifndef UTILITY_H
#define UTILITY_H

#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>

class Utility
{
public:
    static std::string BinaryStringExtraction(std::ifstream &sr, int positionInFile, int numberOfChar);
    static uint32_t BinaryBytesExtraction(std::ifstream &sr, int positionInFile, int numberOfBytes);
    static unsigned char BinaryCharExtraction(std::ifstream &sr, int positionInFile);
    static void WriteCompleteString(std::fstream& outputFileStream, std::string strToWrite, int size, std::string endStringChars = "");

private:
    Utility() { }
};

#endif // UTILITY_EEGFORMAT_H
