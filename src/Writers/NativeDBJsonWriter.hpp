#pragma once

#include "IWriter.hpp"

class NativeDBJsonWriter : public IWriter
{
private:
    enum PrimitiveDef : uint8_t
    {
        Void,
        Bool,
        Int8, Uint8,
        Int16, Uint16,
        Int32, Uint32,
        Int64, Uint64,
        Float, Double,
        String, LocalizationString, CName,
        TweakDBID,
        NodeRef,
        DataBuffer,
        SerializationDeferredDataBuffer,
        SharedDataBuffer,
        CDateTime,
        CGUID,
        CRUID,
        //CRUIDRef,
        EditorObjectID,
        //GamedataLocKeyWrapper,
        MessageResourcePath,
        //RuntimeEntityRef,
        Variant
    };

    enum TemplateDef : uint8_t
    {
        ref = PrimitiveDef::Variant + 1,
        wref,
        script_ref,
        res_ref,
        res_async_ref,
        array,
        curve_data,
        multi_channel_curve
    };

    struct ClassFlags
    {
        uint8_t isPrivate : 1;
        uint8_t isProtected : 1;
        uint8_t isAbstract : 1;
        uint8_t isNative : 1;
        uint8_t isImportOnly : 1;
        uint8_t b5 : 3;
    };

    struct PropertyFlags
    {
        uint8_t isPrivate : 1;
        uint8_t isProtected : 1;
        uint8_t isPersistent : 1;
        uint8_t isReplicated : 1;
        uint8_t isInline : 1;
        uint8_t isEdit : 1;
        uint8_t isConst : 1;
        uint8_t b7 : 2;
    };

    struct ArgumentFlags
    {
        uint8_t isConst : 1;
        uint8_t isOut : 1;
        uint8_t isOptional : 1;
        uint8_t b3 : 5;
    };

    struct FunctionFlags
    {
        uint16_t isPrivate : 1;
        uint16_t isProtected : 1;
        uint16_t isNative : 1;
        uint16_t isStatic : 1;
        uint16_t isFinal : 1;
        uint16_t isThreadSafe : 1;
        uint16_t isEvent : 1;
        uint16_t isConst : 1;
        uint16_t isQuest : 1;
        uint16_t isTimer : 1;
        uint16_t b10 : 6;
    };

public:
    NativeDBJsonWriter(const std::filesystem::path& aRootDir);
    ~NativeDBJsonWriter() = default;

    void Write(Global& aGlobal);
    void Write(std::shared_ptr<Class> aClass);
    void Write(std::shared_ptr<Enum> aEnum);
    void Write(std::shared_ptr<BitField> aBit);
    void Flush();

private:
    nlohmann::ordered_json ProcessType(RED4ext::CProperty* aProperty, bool isArgument = false) const;
    nlohmann::ordered_json ProcessType(RED4ext::CBaseFunction* aFunction) const;
    nlohmann::ordered_json ProcessType(RED4ext::CBaseRTTIType* aType) const;

    ClassFlags ProcessClassFlags(RED4ext::CClass::Flags aFlags) const;
    PropertyFlags ProcessPropertyFlags(RED4ext::CProperty::Flags aFlags) const;
    ArgumentFlags ProcessArgumentFlags(RED4ext::CProperty::Flags aFlags) const;
    FunctionFlags ProcessFunctionFlags(RED4ext::CBaseFunction::Flags aFlags) const;

    std::filesystem::path m_dir;

    nlohmann::ordered_json m_globals;
    nlohmann::ordered_json m_classes;
    nlohmann::ordered_json m_enums;
    nlohmann::ordered_json m_bitfields;

    RED4ext::CRTTISystem* m_rtti;

    std::map<std::string, uint8_t> m_primitives;
};
