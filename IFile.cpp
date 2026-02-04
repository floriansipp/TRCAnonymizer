#include "IFile.h"
#include "MicromedFile.h"
#include "EdfFile.h"
#include <algorithm>

IFile::~IFile()
{

}

std::unique_ptr<IFile> IFile::Create(const std::string& path)
{
    // Extract extension and compare case-insensitively
    auto dotPos = path.rfind('.');
    if (dotPos == std::string::npos)
        return nullptr;

    std::string ext = path.substr(dotPos + 1);
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

    if (ext == "trc")
        return std::make_unique<MicromedFile>(path);
    else if (ext == "edf")
        return std::make_unique<EdfFile>(path);

    return nullptr;
}
