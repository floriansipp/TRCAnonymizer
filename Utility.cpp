#include "Utility.h"

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
