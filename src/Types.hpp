#pragma once

#include <RED4ext/RTTITypes.hpp>

struct BaseType
{
    virtual ~BaseType() = default;

    RED4ext::CName name;
};

struct Class : BaseType
{
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
