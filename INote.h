#pragma once

#include "Utility.h"

class INote
{
public:
    virtual ~INote() = default;
    // Getters
    virtual inline std::string Description() const { return m_Description; }
    virtual inline long Sample() const { return m_Sample; }
    // Setters
    virtual inline void Description(const std::string& description) { m_Description = description; }
    virtual inline void Sample(const long& sample) { m_Sample = sample; }

protected:
    std::string m_Description;
    long m_Sample;
};
