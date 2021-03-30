#pragma once

#include "Types.hpp"

class IWriter
{
public:
    virtual ~IWriter() = default;

    virtual void Write(Global& aGlobal) = 0;
    virtual void Write(std::shared_ptr<Class> aClass) = 0;
    virtual void Write(std::shared_ptr<Enum> aEnum) = 0;
    virtual void Flush() = 0;
};
