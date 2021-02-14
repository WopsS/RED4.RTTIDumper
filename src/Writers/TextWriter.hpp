#pragma once

#include "IWriter.hpp"

class TextWriter : public IWriter
{
public:
    TextWriter(const std::filesystem::path& aRootDir);
    ~TextWriter() = default;

    void Write(Global& aGlobal);
    void Write(std::shared_ptr<Class> aClass);
    void Flush();

private:
    void Write(std::fstream& aFile, std::shared_ptr<Class> aClass);
    void Write(std::fstream& aFile, const RED4ext::CProperty* aProperty);
    void Write(std::fstream& aFile, const RED4ext::CBaseFunction* aFunction);

    std::filesystem::path m_dir;
};
