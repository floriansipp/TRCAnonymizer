#ifndef MICROMEDFILE_H
#define MICROMEDFILE_H

#include "IFile.h"
#include <vector>
#include <fstream>
#include "TRCParameters.h"

class MicromedFile : public IFile
{
public:
    MicromedFile();
    MicromedFile(std::string filePath);
    ~MicromedFile();

    void RemoveMontage(int position);
    void UpdateMontageLabel(int position, std::string label);
    void UpdateMontagesData(std::vector<GenericMontage> montages);
    void AnonymizePatientData(std::string name ="Ymous", std::string surname ="Anon", int d = 1, int m = 1, int y = 0);
    void AnonymizeRecordData(int rd = 1, int rm = 1, int ry = 0, int rth = 1, int rtm = 1, int rts = 1);
    void SaveAnonymizedData(bool overwrite);

private:
    void ReadHeader(std::ifstream &sr);
    void GetMontages(std::ifstream &fileStream, int startOffset, int length, std::vector<montagesOfTrace> &montageList);

private:
    uint32_t m_montageStartOffset;
    uint32_t m_montageLength;
    std::vector<montagesOfTrace> m_montagesList;
};

#endif // MICROMEDFILE_H
