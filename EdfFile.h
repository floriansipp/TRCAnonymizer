#ifndef EDFFILE_H
#define EDFFILE_H

#include <fstream>
#include "IFile.h"

class EdfFile : public IFile
{
public:
    EdfFile();
    EdfFile(std::string filePath);
    ~EdfFile();

    virtual inline void RemoveMontage(int position) { }
    virtual inline void UpdateMontageLabel(int position, std::string label) { }
    virtual inline void UpdateMontagesData(std::vector<GenericMontage> montages) { }
    virtual void AnonymizePatientData(std::string name ="Ymous", std::string surname ="Anon", int d = 1, int m = 1, int y = 0);
    virtual void SaveAnonymizedData(bool overwrite);

private:
    void ReadHeader(std::ifstream &sr);
    void ParsePatientIDField(std::string patientID);
    void ParseStartDateField(std::string startDate);
    void ParseStartTimeField(std::string startTime);
    void DisplayHeaderInfo();

private:
    //== File info
    int m_version = -1;
    std::string m_patientID = "";
    //== PatientID subfields
    std::string m_hospitalCode = "";
    std::string m_sex = "";
    //
    std::string m_recordingID = "";
    std::string m_startDate = "";
    std::string m_startTime = "";
    int m_bytesNumber = -1;
    std::string m_reserved = "";
    int m_recordsNumber = -1;
    int m_recordDuration = -1;
    int m_signalNumber = -1;
};

#endif // EDFFILE_H
