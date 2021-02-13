#pragma once

#include <RED4ext/RTTITypes.hpp>

struct BaseType
{
    virtual ~BaseType() = default;

    RED4ext::CName fullName;
};

struct Property : BaseType
{
    BaseType type;
    RED4ext::CName group;
    uint32_t valueOffset;
    RED4ext::CProperty::Flags flags;
};

struct Function : BaseType
{
    RED4ext::CName shortName;
    std::unique_ptr<Property> returnType;
    std::vector<Property> params;
    uint32_t index;
    RED4ext::CBaseFunction::Flags flags;
};

struct Class : BaseType
{
    std::shared_ptr<Class> parent;
    RED4ext::CName computedName;

    uint32_t size;
    uint32_t alignment;
    uint32_t holderSize;
    RED4ext::CClass::Flags flags;

    std::vector<Property> props;
    std::vector<Function> funcs;
};

struct Global
{
    std::vector<Function> funcs;
};
