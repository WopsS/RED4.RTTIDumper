#pragma once

#include "IWriter.hpp"

class JsonWriter : public IWriter
{
public:
    JsonWriter(const std::filesystem::path& aRootDir, bool aPrettyDump = false);
    ~JsonWriter() = default;

    void Write(Global& aGlobal);
    void Write(std::shared_ptr<Class> aClass);
    void Flush();

private:
    nlohmann::ordered_json ProcessType(Property& aProperty) const;
    nlohmann::ordered_json ProcessType(Function& aFunction) const;

    std::filesystem::path m_dir;
    bool m_prettyDump;

    nlohmann::json m_classes;
};
