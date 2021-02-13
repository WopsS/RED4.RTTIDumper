#pragma once

#include "Types.hpp"
#include "Writers/IWriter.hpp"

class Dumper
{
public:
    void Run(IWriter& aWriter);

private:
    void CollectTypes();

    void CollectType(RED4ext::CClass* aClass);
    void CollectStatics();

    void OrderFunctions();

    Property ProcessType(RED4ext::CProperty* aProperty);

    Function ProcessType(RED4ext::CBaseFunction* aFunction);
    Function ProcessType(RED4ext::CGlobalFunction* aFunction);
    Function ProcessType(RED4ext::CClassFunction* aFunction);

    Global m_global;
    std::map<std::string, std::shared_ptr<BaseType>> m_types;
};
