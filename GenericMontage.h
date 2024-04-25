#ifndef GENERICMONTAGE_H
#define GENERICMONTAGE_H

#include <string>

class GenericMontage
{
public:
    GenericMontage(std::string label, int initialPosition);
    ~GenericMontage();
    inline std::string Name() const { return m_label; }
    inline int InitialPosition() const { return m_initialPosition; }
    inline void Name(const std::string& name) { m_label = name; }

private:
    std::string m_label = "";
    int m_initialPosition = -1;
};

#endif // GENERICMONTAGE_H
