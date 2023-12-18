#include "stdafx.hpp"
#include "NativeDBJsonWriter.hpp"

#include <iostream>

#define AST_FULL_NAME "a"
#define AST_SHORT_NAME "b"
#define AST_RETURN_TYPE "c"
#define AST_FLAGS "d"
#define AST_PARAMS "e"
#define AST_NAME "f"
#define AST_PRIMITIVE "g"
#define AST_INNER_TYPE "h"
#define AST_ARRAY_SIZE "i"
#define AST_TYPE "j"
#define AST_PARENT "k"
#define AST_PROPS "l"
#define AST_FUNCS "m"
#define AST_MEMBERS "n"

#define AST_FUNC_FULL_NAME "a"
#define AST_FUNC_SHORT_NAME "b"
#define AST_FUNC_RETURN_TYPE "c"
#define AST_FUNC_FLAGS "d"
#define AST_FUNC_PARAMS "e"

#define AST_CLASS_PARENT "a"
#define AST_CLASS_NAME "b"
#define AST_CLASS_ALIAS_NAME "c"
#define AST_CLASS_FLAGS "d"
#define AST_CLASS_PROPS "e"
#define AST_CLASS_FUNCS "f"
#define AST_CLASS_IS_STRUCT "g"

#define AST_ENUM_NAME "a"
#define AST_ENUM_ALIAS_NAME "b"
#define AST_ENUM_MEMBERS "c"

#define AST_BITFIELD_NAME "a"
#define AST_BITFIELD_ALIAS_NAME "b"
#define AST_BITFIELD_MEMBERS "c"

#define AST_PROP_TYPE "a"
#define AST_PROP_NAME "b"
#define AST_PROP_FLAGS "c"

#define AST_TYPE_FLAG "a"
#define AST_TYPE_NAME "b"
#define AST_TYPE_INNER_TYPE "c"
#define AST_TYPE_ARRAY_SIZE "d"

NativeDBJsonWriter::NativeDBJsonWriter(const std::filesystem::path& aRootDir)

    : m_dir(aRootDir / L"nativedb" / L"json")
    , m_globals(nlohmann::ordered_json::array())
    , m_classes(nlohmann::ordered_json::array())
    , m_enums(nlohmann::ordered_json::array())
    , m_bitfields(nlohmann::ordered_json::array())
    , m_rtti(RED4ext::CRTTISystem::Get())
{
    if (!std::filesystem::exists(m_dir))
    {
        std::filesystem::create_directories(m_dir);
    }

    m_primitives["Void"] = PrimitiveDef::Void;
    m_primitives["Bool"] = PrimitiveDef::Bool;
    m_primitives["Int8"] = PrimitiveDef::Int8;
    m_primitives["Uint8"] = PrimitiveDef::Uint8;
    m_primitives["Int16"] = PrimitiveDef::Int16;
    m_primitives["Uint16"] = PrimitiveDef::Uint16;
    m_primitives["Int32"] = PrimitiveDef::Int32;
    m_primitives["Uint32"] = PrimitiveDef::Uint32;
    m_primitives["Int64"] = PrimitiveDef::Int64;
    m_primitives["Uint64"] = PrimitiveDef::Uint64;
    m_primitives["Float"] = PrimitiveDef::Float;
    m_primitives["Double"] = PrimitiveDef::Double;
    m_primitives["String"] = PrimitiveDef::String;
    m_primitives["LocalizationString"] = PrimitiveDef::LocalizationString;
    m_primitives["CName"] = PrimitiveDef::CName;
    m_primitives["TweakDBID"] = PrimitiveDef::TweakDBID;
    m_primitives["NodeRef"] = PrimitiveDef::NodeRef;
    m_primitives["DataBuffer"] = PrimitiveDef::DataBuffer;
    m_primitives["serializationDeferredDataBuffer"] = PrimitiveDef::SerializationDeferredDataBuffer;
    m_primitives["SharedDataBuffer"] = PrimitiveDef::SharedDataBuffer;
    m_primitives["CDateTime"] = PrimitiveDef::CDateTime;
    m_primitives["CGUID"] = PrimitiveDef::CGUID;
    m_primitives["CRUID"] = PrimitiveDef::CRUID;
    //m_primitives["CRUIDRef"] = PrimitiveDef::CRUIDRef;
    m_primitives["EditorObjectID"] = PrimitiveDef::EditorObjectID;
    //m_primitives["GamedataLocKeyWrapper"] = PrimitiveDef::GamedataLocKeyWrapper;
    m_primitives["MessageResourcePath"] = PrimitiveDef::MessageResourcePath;
    //m_primitives["RuntimeEntityRef"] = PrimitiveDef::RuntimeEntityRef;
    m_primitives["Variant"] = PrimitiveDef::Variant;
}

void NativeDBJsonWriter::Write(Global& aGlobal)
{

    for (auto& func : aGlobal.funcs)
    {
        auto obj = ProcessType(func);

        m_globals.emplace_back(obj);
    }
}

void NativeDBJsonWriter::Write(std::shared_ptr<Class> aClass)
{
    auto dir = m_dir / L"classes";
    if (!std::filesystem::exists(dir))
    {
        std::filesystem::create_directories(dir);
    }

    std::string name = aClass->name.ToString();
    std::fstream file(dir / (name + ".json"), std::ios::out);

    bool isStruct = aClass->flags.isScriptedStruct;
    bool isClass = aClass->flags.isScriptedClass;

    if (!isStruct && !isClass)
    {
        static auto serializableType = RED4ext::CRTTISystem::Get()->GetClass("ISerializable");
        if (!aClass->raw->IsA(serializableType))
        {
            isStruct = true;
        }
        else
        {
            isClass = true;
        }
    }
    nlohmann::ordered_json obj;

    auto parent = aClass->parent;
    if (parent)
    {
        obj[AST_CLASS_PARENT] = parent->name.ToString();
    }
    ClassFlags flags = ProcessClassFlags(aClass->flags);

    obj[AST_CLASS_NAME] = aClass->name.ToString();

    auto alias = m_rtti->nativeToScript.Get(aClass->name);

    if (alias && !m_rtti->types.Get(*alias))
    {
        obj[AST_CLASS_ALIAS_NAME] = alias->ToString();
    }

    obj[AST_CLASS_FLAGS] = *reinterpret_cast<uint8_t*>(&flags);
    //*reinterpret_cast<uint32_t*>(&aClass->flags);

    if (isStruct)
    {
        obj[AST_CLASS_IS_STRUCT] = true;
    }

    nlohmann::ordered_json props;

    for (auto& prop : aClass->props)
    {
        auto obj = ProcessType(prop);
        props.emplace_back(obj);
    }

    if (!props.is_null())
    {
        obj[AST_CLASS_PROPS] = props;
    }

    nlohmann::ordered_json funcs;

    for (auto& func : aClass->funcs)
    {
        auto obj = ProcessType(func);
        funcs.emplace_back(obj);
    }

    if (!funcs.is_null())
    {
        obj[AST_CLASS_FUNCS] = funcs;
    }

    m_classes.emplace_back(obj);

    file << obj << std::endl;
}

void NativeDBJsonWriter::Write(std::shared_ptr<Enum> aEnum)
{
    auto dir = m_dir / L"enums";
    if (!std::filesystem::exists(dir))
    {
        std::filesystem::create_directories(dir);
    }

    std::string name = aEnum->name.ToString();
    std::fstream file(dir / (name + ".json"), std::ios::out);

    nlohmann::ordered_json obj;
    obj[AST_ENUM_NAME] = aEnum->name.ToString();

    auto alias = m_rtti->nativeToScript.Get(aEnum->name);

    if (alias && !m_rtti->types.Get(*alias))
    {
        obj[AST_ENUM_ALIAS_NAME] = alias->ToString();
    }

    nlohmann::ordered_json members = nlohmann::json::array();

    for (size_t i = 0; i < aEnum->enumerators.size(); i++)
    {
        const auto& member = aEnum->enumerators[i];

        nlohmann::ordered_json mbrObj;
        nlohmann::json::number_integer_t mbrValue;

        switch (aEnum->actualSize)
        {
        case sizeof(int8_t):
        case sizeof(int16_t):
        {
            mbrValue = nlohmann::json::number_integer_t(member.value);
            break;
        }
        case sizeof(int32_t):
        {
            mbrValue = nlohmann::json::number_integer_t(member.value);
            break;
        }
        case sizeof(int64_t):
        {
            mbrValue = nlohmann::json::number_integer_t(member.value);
            break;
        }
        default:
        {
            if (member.value == -1)
            {
                std::cout << aEnum->name.ToString() << std::endl;
            }
            mbrValue = member.value;
            break;
        }
        }

        mbrObj[member.name.ToString()] = mbrValue;

        members.emplace_back(mbrObj);
    }

    obj[AST_ENUM_MEMBERS] = members;

    m_enums.emplace_back(obj);

    file << obj << std::endl;
}

void NativeDBJsonWriter::Write(std::shared_ptr<BitField> aBit)
{
    auto dir = m_dir / L"bitfields";
    if (!std::filesystem::exists(dir))
    {
        std::filesystem::create_directories(dir);
    }

    std::string name = aBit->name.ToString();
    std::fstream file(dir / (name + ".json"), std::ios::out);

    nlohmann::ordered_json obj;
    obj[AST_BITFIELD_NAME] = aBit->name.ToString();

    auto alias = m_rtti->nativeToScript.Get(aBit->name);

    if (alias && !m_rtti->types.Get(*alias))
    {
        obj[AST_BITFIELD_ALIAS_NAME] = alias->ToString();
    }

    nlohmann::ordered_json members = nlohmann::json::array();

    auto validBits = aBit->validBits;
    auto counter = 0;
    while (validBits != 0)
    {
        auto bit = validBits & 1;
        validBits >>= 1;

        if (bit == 1)
        {
            nlohmann::ordered_json member;
            const auto& name = aBit->bitNames[counter].ToString();
            const auto& bit = nlohmann::json::number_integer_t(counter);

            member[name] = bit;

            members.emplace_back(member);
        }

        counter++;
    }

    obj[AST_BITFIELD_MEMBERS] = members;

    m_bitfields.emplace_back(obj);

    file << obj << std::endl;
}

void NativeDBJsonWriter::Flush()
{
    std::fstream globals_file(m_dir / L"globals.json", std::ios::out);
    std::fstream classes_file(m_dir / L"classes.json", std::ios::out);
    std::fstream enums_file(m_dir / L"enums.json", std::ios::out);
    std::fstream bitfields_file(m_dir / L"bitfields.json", std::ios::out);

    globals_file << m_globals << std::endl;
    classes_file << m_classes << std::endl;
    enums_file << m_enums << std::endl;
    bitfields_file << m_bitfields << std::endl;
}

nlohmann::ordered_json NativeDBJsonWriter::ProcessType(RED4ext::CProperty* aProperty, bool isArgument) const
{
    nlohmann::ordered_json obj;

    obj[AST_PROP_TYPE] = ProcessType(aProperty->type);

    // Skip "__return".
    if (aProperty->name != "__return")
    {
        obj[AST_PROP_NAME] = aProperty->name.ToString();
    }

    if (isArgument)
    {
        ArgumentFlags flags = ProcessArgumentFlags(aProperty->flags);

        obj[AST_PROP_FLAGS] = *reinterpret_cast<uint8_t*>(&flags);
    }
    else
    {
        PropertyFlags flags = ProcessPropertyFlags(aProperty->flags);

        obj[AST_PROP_FLAGS] = *reinterpret_cast<uint8_t*>(&flags);
    }

    return obj;
}

nlohmann::ordered_json NativeDBJsonWriter::ProcessType(RED4ext::CBaseFunction* aFunction) const
{
    nlohmann::ordered_json obj;
    const auto& fullName = aFunction->fullName.ToString();
    const auto& shortName = aFunction->shortName.ToString();

    obj[AST_FUNC_FULL_NAME] = fullName;
    if (fullName != shortName)
    {
        obj[AST_FUNC_SHORT_NAME] = shortName;
    }

    if (aFunction->returnType)
    {
        obj[AST_FUNC_RETURN_TYPE] = ProcessType(aFunction->returnType->type);
    }

    FunctionFlags flags = ProcessFunctionFlags(aFunction->flags);

    obj[AST_FUNC_FLAGS] = *reinterpret_cast<uint16_t*>(&flags);

    nlohmann::ordered_json params;

    for (auto& param : aFunction->params)
    {
        auto obj = ProcessType(param, true);
        params.emplace_back(obj);
    }

    if (!params.is_null())
    {
        obj[AST_FUNC_PARAMS] = params;
    }

    return obj;
}

nlohmann::ordered_json NativeDBJsonWriter::ProcessType(RED4ext::CBaseRTTIType* aType) const
{
    using ERTTIType = RED4ext::ERTTIType;

    nlohmann::ordered_json obj;
    auto name = aType->GetName().ToString();

    switch (aType->GetType())
    {
    case ERTTIType::Fundamental:
    {
        if (m_primitives.contains(name))
        {
            obj[AST_TYPE_FLAG] = m_primitives.at(name);
        }
        else
        {
            obj[AST_TYPE_NAME] = name;
        }
        break;
    }
    case ERTTIType::Enum:
    case ERTTIType::BitField:
    {
        obj[AST_TYPE_NAME] = name;
        break;
    }
    case ERTTIType::Array:
    {
        obj[AST_TYPE_FLAG] = TemplateDef::array;

        auto arr = static_cast<RED4ext::CRTTIArrayType*>(aType);

        obj[AST_TYPE_INNER_TYPE] = ProcessType(arr->innerType);
        break;
    }
    case ERTTIType::StaticArray:
    {
        obj[AST_TYPE_FLAG] = TemplateDef::array;

        auto arr = static_cast<RED4ext::CRTTIStaticArrayType*>(aType);

        obj[AST_TYPE_INNER_TYPE] = ProcessType(arr->innerType);
        break;
    }
    case ERTTIType::NativeArray:
    {
        obj[AST_TYPE_FLAG] = TemplateDef::array;

        auto arr = static_cast<RED4ext::CRTTINativeArrayType*>(aType);

        obj[AST_TYPE_INNER_TYPE] = ProcessType(arr->innerType);
        obj[AST_TYPE_ARRAY_SIZE] = arr->size;
        break;
    }
    case ERTTIType::Handle:
    {
        obj[AST_TYPE_FLAG] = TemplateDef::ref;

        auto handle = static_cast<RED4ext::CRTTIHandleType*>(aType);

        obj[AST_TYPE_INNER_TYPE] = ProcessType(handle->innerType);
        break;
    }
    case ERTTIType::WeakHandle:
    {
        obj[AST_TYPE_FLAG] = TemplateDef::wref;

        auto whandle = static_cast<RED4ext::CRTTIWeakHandleType*>(aType);

        obj[AST_TYPE_INNER_TYPE] = ProcessType(whandle->innerType);
        break;
    }
    case ERTTIType::ResourceReference:
    {
        obj[AST_TYPE_FLAG] = TemplateDef::res_ref;

        auto rRef = static_cast<RED4ext::CRTTIResourceReferenceType*>(aType);

        obj[AST_TYPE_INNER_TYPE] = ProcessType(rRef->innerType);
        break;
    }
    case ERTTIType::ResourceAsyncReference:
    {
        obj[AST_TYPE_FLAG] = TemplateDef::res_async_ref;

        auto raRef = static_cast<RED4ext::CRTTIResourceAsyncReferenceType*>(aType);

        obj[AST_TYPE_INNER_TYPE] = ProcessType(raRef->innerType);
        break;
    }
    case ERTTIType::ScriptReference:
    {
        obj[AST_TYPE_FLAG] = TemplateDef::script_ref;

        auto whandle = static_cast<RED4ext::CRTTIScriptReferenceType*>(aType);

        obj[AST_TYPE_INNER_TYPE] = ProcessType(whandle->innerType);
        break;
    }
    case ERTTIType::LegacySingleChannelCurve:
    {
        obj[AST_TYPE_FLAG] = TemplateDef::curve_data;

        auto curve = static_cast<RED4ext::CRTTILegacySingleChannelCurveType*>(aType);

        obj[AST_TYPE_INNER_TYPE] = ProcessType(curve->curveType);
        break;
    }
    default:
    {
        if (m_primitives.contains(name))
        {
            obj[AST_TYPE_FLAG] = m_primitives.at(name);
        }
        else
        {
            std::string test = name;

            if (test.find("multiChannelCurve:") != std::string::npos)
            {
                obj[AST_TYPE_FLAG] = TemplateDef::multi_channel_curve;

                auto curve = static_cast<RED4ext::CRTTIMultiChannelCurveType*>(aType);

                obj[AST_TYPE_INNER_TYPE] = ProcessType(curve->curveType);
            }
            else
            {
                obj[AST_TYPE_NAME] = name;
            }
        }
        break;
    }
    }
    return obj;
}

NativeDBJsonWriter::ClassFlags NativeDBJsonWriter::ProcessClassFlags(RED4ext::CClass::Flags aFlags) const
{
    NativeDBJsonWriter::ClassFlags flags;

    flags.isPrivate = aFlags.isPrivate;
    flags.isProtected = aFlags.isProtected;
    flags.isAbstract = aFlags.isAbstract;
    flags.isNative = aFlags.isNative;
    flags.isImportOnly = aFlags.isImportOnly;
    flags.b5 = 0;
    return flags;
}

NativeDBJsonWriter::PropertyFlags NativeDBJsonWriter::ProcessPropertyFlags(RED4ext::CProperty::Flags aFlags) const
{
    NativeDBJsonWriter::PropertyFlags flags;

    flags.isPrivate = aFlags.isPrivate;
    flags.isProtected = aFlags.isProtected;
    flags.isPersistent = aFlags.isPersistent;
    flags.isReplicated = 0; // aFlags.isReplicated;
    flags.isInline = 0; // aFlags.isInline;
    flags.isEdit = 0; // aFlags.isEdit;
    flags.isConst = 0; // aFlags.isConst;
    flags.b7 = 0;
    return flags;
}

NativeDBJsonWriter::ArgumentFlags NativeDBJsonWriter::ProcessArgumentFlags(RED4ext::CProperty::Flags aFlags) const
{
    NativeDBJsonWriter::ArgumentFlags flags;

    flags.isConst = 0; //aFlags.isConst;
    flags.isOut = aFlags.isOut;
    flags.isOptional = aFlags.isOptional;
    flags.b3 = 0;
    return flags;
}

NativeDBJsonWriter::FunctionFlags NativeDBJsonWriter::ProcessFunctionFlags(RED4ext::CBaseFunction::Flags aFlags) const
{
    NativeDBJsonWriter::FunctionFlags flags;

    flags.isPrivate = aFlags.isPrivate;
    flags.isProtected = aFlags.isProtected;
    flags.isNative = aFlags.isNative;
    flags.isStatic = aFlags.isStatic;
    flags.isFinal = aFlags.isFinal;
    flags.isThreadSafe = aFlags.isThreadsafe;
    flags.isEvent = aFlags.isEvent;
    flags.isConst = aFlags.isConst;
    flags.isQuest = aFlags.isQuest;
    flags.isTimer = aFlags.isTimer;
    flags.b10 = 0;
    return flags;
}
