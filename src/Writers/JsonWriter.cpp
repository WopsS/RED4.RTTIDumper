#include "stdafx.hpp"
#include "JsonWriter.hpp"

JsonWriter::JsonWriter(const std::filesystem::path& aRootDir, bool aPrettyDump)
    : m_dir(aRootDir / L"json")
    , m_prettyDump(aPrettyDump)
{
    if (!std::filesystem::exists(m_dir))
    {
        std::filesystem::create_directories(m_dir);
    }
}

void JsonWriter::Write(Global& aGlobal)
{
    std::fstream file(m_dir / L"globals.json", std::ios::out);

    nlohmann::ordered_json obj;
    nlohmann::ordered_json funcs;

    for (auto& func : aGlobal.funcs)
    {
        auto obj = ProcessType(func);
        funcs.emplace_back(obj);
    }

    if (!funcs.is_null())
    {
        obj["funcs"] = funcs;
    }

    if (m_prettyDump)
    {
        file << std::setw(2);
    }

    file << obj << std::endl;
}

void JsonWriter::Write(std::shared_ptr<Class> aClass)
{
    auto dir = m_dir / L"classes";
    if (!std::filesystem::exists(dir))
    {
        std::filesystem::create_directories(dir);
    }

    std::string name = aClass->fullName.ToString();
    std::fstream file(dir / (name + ".json"), std::ios::out);

    nlohmann::ordered_json obj;

    auto parent = aClass->parent;
    if (parent)
    {
        obj["parent"] = parent->fullName.ToString();
    }

    obj["fullName"] = {{"hash", aClass->fullName.hash}, {"text", aClass->fullName.ToString()}};
    obj["computedName"] = {{"hash", aClass->computedName.hash}, {"text", aClass->computedName.ToString()}};
    obj["size"] = aClass->size;
    obj["alignment"] = aClass->alignment;
    obj["holderSize"] = aClass->holderSize;
    obj["flags"] = *reinterpret_cast<uint32_t*>(&aClass->flags);

    nlohmann::ordered_json props;

    for (auto& prop : aClass->props)
    {
        auto obj = ProcessType(prop);
        props.emplace_back(obj);
    }

    if (!props.is_null())
    {
        obj["props"] = props;
    }

    nlohmann::ordered_json funcs;

    for (auto& func : aClass->funcs)
    {
        auto obj = ProcessType(func);
        funcs.emplace_back(obj);
    }

    if (!funcs.is_null())
    {
        obj["funcs"] = funcs;
    }

    if (m_prettyDump)
    {
        file << std::setw(2);
    }

    file << obj << std::endl;
    m_classes.emplace_back(name);
}

void JsonWriter::Flush()
{
    std::fstream file;

    file.open(m_dir / L"classes.json", std::ios::out);
    file << m_classes << std::endl;
    file.close();
}

nlohmann::ordered_json JsonWriter::ProcessType(Property& aProperty) const
{
    nlohmann::ordered_json obj;

    obj["type"] = {{"hash", aProperty.type.fullName.hash}, {"text", aProperty.type.fullName.ToString()}};
    obj["fullName"] = {{"hash", aProperty.fullName.hash}, {"text", aProperty.fullName.ToString()}};

    if (aProperty.group.hash)
    {
        obj["group"] = {{"hash", aProperty.group.hash}, {"text", aProperty.group.ToString()}};
    }

    obj["valueOffset"] = aProperty.valueOffset;
    obj["flags"] = *reinterpret_cast<uint64_t*>(&aProperty.flags);

    return obj;
}

nlohmann::ordered_json JsonWriter::ProcessType(Function& aFunction) const
{
    nlohmann::ordered_json obj;
    obj["fullName"] = {{"hash", aFunction.fullName.hash}, {"text", aFunction.fullName.ToString()}};
    obj["shortName"] = {{"hash", aFunction.shortName.hash}, {"text", aFunction.shortName.ToString()}};

    if (aFunction.returnType)
    {
        obj["return"] = ProcessType(*aFunction.returnType.get());
    }

    if (aFunction.index)
    {
        obj["index"] = aFunction.index;
    }

    obj["flags"] = *reinterpret_cast<uint32_t*>(&aFunction.flags);

    nlohmann::ordered_json params;

    for (auto& param : aFunction.params)
    {
        auto obj = ProcessType(param);
        params.emplace_back(obj);
    }

    if (!params.is_null())
    {
        obj["params"] = params;
    }

    return obj;
}
