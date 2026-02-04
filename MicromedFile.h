#ifndef MICROMEDFILE_H
#define MICROMEDFILE_H

#include <cstdint>
#include "IFile.h"
#include <vector>
#include <fstream>
#include "TRCParameters.h"

class MicromedFile : public IFile
{
public:
    MicromedFile();
    MicromedFile(std::string filePath);
    ~MicromedFile() override;

    inline int NoteCount() const { return m_notesList.size(); }
    inline std::vector<INote*> Notes() const override
    {
        std::vector<INote*> notes;
        size_t notesSize = m_notesList.size();
        for (size_t i = 0; i < notesSize; i++)
        {
            notes.push_back(const_cast<operatorNote*>(&m_notesList[i]));
        }
        return notes;
    }
    inline INote* Note(const int& i) const
    {
        if (i >= 0 && i < static_cast<int>(m_notesList.size()))
        {
            return const_cast<operatorNote*>(&m_notesList[i]);
        }
        return nullptr;
    }
    inline void Notes(const std::vector<INote*> &notes) override
    {
        m_notesList.clear();
        for (size_t i = 0; i < notes.size(); i++)
        {
            m_notesList.push_back(operatorNote(*notes[i]));
        }
    }
    inline void AddNote(const INote& note)
    {
        for (size_t i = 0; i < m_notesList.size(); ++i)
        {
            if (note.Sample() < m_notesList[i].Sample())
            {
                m_notesList.insert(m_notesList.begin() + i, operatorNote(note));
                return;
            }
        }
        m_notesList.push_back(operatorNote(note));
    }
    inline void RemoveNote(int position) override
    {
        m_notesList.erase(m_notesList.begin() + position);
    }
    void UpdateNote(int position, int sample, std::string description) override;

    void RemoveMontage(int position) override;
    void UpdateMontageLabel(int position, std::string label) override;
    void UpdateMontagesData(std::vector<GenericMontage> montages) override;
    void AnonymizePatientData(std::string name ="Ymous", std::string surname ="Anon", int d = 1, int m = 1, int y = 1900) override;
    void AnonymizeRecordData(int rd = 1, int rm = 1, int ry = 1900, int rth = -1, int rtm = -1, int rts = -1) override;
    void SaveAnonymizedData(bool overwrite) override;

private:
    void ReadHeader(std::ifstream &sr);
    void GetNotes(std::ifstream &fileStream, int startOffset, int length, std::vector<operatorNote> &notesList);
    void GetMontages(std::ifstream &fileStream, int startOffset, int length, std::vector<montagesOfTrace> &montageList);

private:
    uint32_t m_notesStartOffset;
    uint32_t m_notesLength;
    uint32_t m_montageStartOffset;
    uint32_t m_montageLength;
    std::vector<operatorNote> m_notesList;
    std::vector<montagesOfTrace> m_montagesList;
};

#endif // MICROMEDFILE_H
