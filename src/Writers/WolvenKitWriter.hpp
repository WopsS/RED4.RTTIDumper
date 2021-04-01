#pragma once

#include "IWriter.hpp"

class WolvenKitWriter : public IWriter
{
public:
    WolvenKitWriter(const std::filesystem::path& aRootDir);
    ~WolvenKitWriter() = default;

    void Write(Global& aGlobal);
    void Write(std::shared_ptr<Class> aClass);
    void Write(std::shared_ptr<Enum> aEnum);
    void Write(std::shared_ptr<BitField> aBit);
    void Flush();

private:
    void Write(std::fstream& aFile, RED4ext::IRTTIType* aType);
    void Write(std::fstream& aFile, RED4ext::CProperty* aProperty, size_t aOrdinal);

    std::string GetWolvenType(const char* aName);
    std::string GetFixedSize(RED4ext::IRTTIType* aType);
    std::string GetCSType(RED4ext::IRTTIType* aType);
    size_t GetOrdinalStart(std::shared_ptr<Class> aClass);

    bool CheckForDuplicate(RED4ext::CClass* aClass, RED4ext::CProperty* aProperty);
    size_t CountOccurences(RED4ext::CClass* aClass, RED4ext::CProperty* aProperty);

    std::string Sanitize(const std::string& aInput);

    std::filesystem::path m_dir;

    std::unordered_map<std::string, std::string> m_typeMappings;

    std::unordered_map<std::string, size_t> m_customClasses;
    std::unordered_map<std::string, std::unordered_set<size_t>> m_skippedOrdinals;
    std::unordered_map<std::string, size_t> m_nextOrdinals;
    std::unordered_set<std::string> m_isWritten;
};
