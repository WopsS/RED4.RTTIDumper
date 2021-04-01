#pragma once

#include "Types.hpp"
#include "Writers/IWriter.hpp"

class Dumper
{
public:
    void Run(std::shared_ptr<IWriter> aWriter);

private:
    void CollectTypes();

    void CollectType(RED4ext::CBitfield* aEnum);
    void CollectType(RED4ext::CEnum* aEnum);

    void CollectType(RED4ext::CClass* aClass);
    void CollectStatics();
    void OrderFunctions();

    Global m_global;
    std::map<std::string, std::shared_ptr<BaseType>> m_types;
};
