#pragma once

#include <RED4ext/RTTITypes.hpp>

struct BaseType
{
    virtual ~BaseType() = default;

    RED4ext::CName name;
};

struct BitField : BaseType
{
    uint64_t validBits;
    std::vector<RED4ext::CName> bitNames;
    uint8_t actualSize;
};

struct Enum : BaseType
{
    struct Enumerator
    {
        RED4ext::CName name;
        uint64_t value;
    };

    std::vector<Enumerator> enumerators;
    uint8_t actualSize;
};

struct Class : BaseType
{
    RED4ext::CClass* raw;

    std::shared_ptr<Class> parent;
    RED4ext::CName computedName;

    uint32_t size;
    uint32_t alignment;
    uint32_t holderSize;
    RED4ext::CClass::Flags flags;

    std::vector<RED4ext::CProperty*> props;
    std::vector<RED4ext::CBaseFunction*> funcs;
};

struct Global
{
    std::vector<RED4ext::CGlobalFunction*> funcs;
};
