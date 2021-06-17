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

    std::string name = aClass->name.ToString();
    std::fstream file(dir / (name + ".json"), std::ios::out);

    nlohmann::ordered_json obj;

    auto parent = aClass->parent;
    if (parent)
    {
        obj["parent"] = parent->name.ToString();
    }

    obj["name"] = aClass->name.ToString();
    //obj["computedName"] = aClass->computedName.ToString();
    //obj["size"] = aClass->size;
    //obj["alignment"] = aClass->alignment;
    //obj["holderSize"] = aClass->holderSize;
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

void JsonWriter::Write(std::shared_ptr<Enum> aEnum)
{
}

void JsonWriter::Write(std::shared_ptr<BitField> aBit)
{
}

void JsonWriter::Flush()
{
    std::fstream file;

    file.open(m_dir / L"classes.json", std::ios::out);
    file << m_classes << std::endl;
    file.close();
}

nlohmann::ordered_json JsonWriter::ProcessType(RED4ext::CProperty* aProperty) const
{
    nlohmann::ordered_json obj;

    RED4ext::CName typeName;
    aProperty->type->GetName(typeName);

    obj["type"] = typeName.ToString();

    // Skip "__return".
    if (aProperty->name != "__return")
    {
        obj["name"] = aProperty->name.ToString();
    }

    if (aProperty->group.hash)
    {
        obj["group"] = aProperty->group.ToString();
    }

    //obj["valueOffset"] = aProperty->valueOffset;
    obj["flags"] = *reinterpret_cast<uint64_t*>(&aProperty->flags);

    return obj;
}

nlohmann::ordered_json JsonWriter::ProcessType(RED4ext::CBaseFunction* aFunction) const
{
    nlohmann::ordered_json obj;
    obj["fullName"] = aFunction->fullName.ToString();
    obj["shortName"] = aFunction->shortName.ToString();

    if (aFunction->returnType)
    {
        obj["return"] = ProcessType(aFunction->returnType);
    }

    if (aFunction->GetRegIndex() != -1)
    {
        obj["index"] = aFunction->GetRegIndex();
    }

    obj["flags"] = *reinterpret_cast<uint32_t*>(&aFunction->flags);

    nlohmann::ordered_json params;

    for (auto& param : aFunction->params)
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
