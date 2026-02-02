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
    ~MicromedFile();

    virtual inline int NoteCount() const { return m_notesList.size(); }
    virtual inline std::vector<INote*> Notes() const
    {
        std::vector<INote*> notes;
        size_t notesSize = m_notesList.size();
        for (size_t i = 0; i < notesSize; i++)
        {
            notes.push_back(m_notesList[i]);
        }
        return notes;
    }
    virtual inline INote* Note(const int& i) const
    {
        if (i >= 0 && i < m_notesList.size())
        {
            return m_notesList[i];
        }
        return nullptr;
    }
    virtual inline void Notes(const std::vector<INote*> &notes)
    {
        std::vector<operatorNote*> oldNotes = m_notesList;
        m_notesList.clear();
        for (int i = 0; i < notes.size(); i++)
        {
            m_notesList.push_back(new operatorNote(*notes[i]));
        }
        for (auto p : oldNotes)
        {
            Utility::DeleteAndNullify(p);
        }
    }
    virtual inline void AddNote(const INote& note)
    {
        for (int i = 0; i < m_notesList.size(); ++i)
        {
            if (note.Sample() < m_notesList[i]->Sample())
            {
                m_notesList.insert(m_notesList.begin() + i, new operatorNote(note));
                return;
            }
        }
        m_notesList.push_back(new operatorNote(note));
    }
    virtual inline void RemoveNote(int position)
    {
        m_notesList.erase(m_notesList.begin() + position);
    }
    virtual void UpdateNote(int position, int sample, std::string description);

    void RemoveMontage(int position);
    void UpdateMontageLabel(int position, std::string label);
    void UpdateMontagesData(std::vector<GenericMontage> montages);
    void AnonymizePatientData(std::string name ="Ymous", std::string surname ="Anon", int d = 1, int m = 1, int y = 1900);
    void AnonymizeRecordData(int rd = 1, int rm = 1, int ry = 1900, int rth = -1, int rtm = -1, int rts = -1) override;
    void SaveAnonymizedData(bool overwrite);

private:
    void ReadHeader(std::ifstream &sr);
    void GetNotes(std::ifstream &fileStream, int startOffset, int length, std::vector<operatorNote*> &notesList);
    void GetMontages(std::ifstream &fileStream, int startOffset, int length, std::vector<montagesOfTrace> &montageList);

private:
    uint32_t m_notesStartOffset;
    uint32_t m_notesLength;
    uint32_t m_montageStartOffset;
    uint32_t m_montageLength;
    std::vector<operatorNote*> m_notesList;
    std::vector<montagesOfTrace> m_montagesList;
};

#endif // MICROMEDFILE_H
