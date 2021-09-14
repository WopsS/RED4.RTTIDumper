#include "stdafx.hpp"
#include "Dumper.hpp"

void Dumper::Run(std::shared_ptr<IWriter> aWriter)
{
    if (m_types.empty())
    {
        CollectTypes();
        CollectStatics();

        OrderFunctions();
    }

    aWriter->Write(m_global);

    for (const auto [key, type] : m_types)
    {
        auto cls = std::dynamic_pointer_cast<Class>(type);
        if (cls)
        {
            aWriter->Write(cls);
            continue;
        }

        auto enm = std::dynamic_pointer_cast<Enum>(type);
        if (enm)
        {
            aWriter->Write(enm);
            continue;
        }

        auto bit = std::dynamic_pointer_cast<BitField>(type);
        if (bit)
        {
            aWriter->Write(bit);
            continue;
        }

        throw std::runtime_error("unhandled type");
    }

    aWriter->Flush();
}

void Dumper::CollectTypes()
{
    auto rtti = RED4ext::CRTTISystem::Get();
    rtti->types.for_each([this](RED4ext::CName aName, RED4ext::CBaseRTTIType* aType) {
        auto typeId = aType->GetType();
        switch (typeId)
        {
        case RED4ext::ERTTIType::Class:
        {
            auto cls = static_cast<RED4ext::CClass*>(aType);
            CollectType(cls);

            break;
        }
        case RED4ext::ERTTIType::Enum:
        {
            auto enm = static_cast<RED4ext::CEnum*>(aType);
            CollectType(enm);

            break;
        }
        case RED4ext::ERTTIType::BitField:
        {
            auto bit = static_cast<RED4ext::CBitfield*>(aType);
            CollectType(bit);

            break;
        }
        }
    });
}

void Dumper::CollectType(RED4ext::CBitfield* aBit)
{
    auto bit = std::make_shared<BitField>();
    bit->name = aBit->name;
    bit->actualSize = aBit->actualSize;
    bit->validBits = aBit->validBits;

    for (uint32_t i = 0; i < 64; i++)
    {
        bit->bitNames.emplace_back(aBit->bitNames[i]);
    }

    m_types.emplace(bit->name.ToString(), bit);
}

void Dumper::CollectType(RED4ext::CEnum* aEnum)
{
    auto enm = std::make_shared<Enum>();
    enm->name = aEnum->name;
    enm->actualSize = aEnum->actualSize;

    for (uint32_t i = 0; i < aEnum->hashList.size; i++)
    {
        Enum::Enumerator enumerator;
        enumerator.name = aEnum->hashList[i];
        enumerator.value = aEnum->valueList[i];

        enm->enumerators.emplace_back(std::move(enumerator));
    }

    for (uint32_t i = 0; i < aEnum->aliasList.size; i++)
    {
        Enum::Enumerator enumerator;
        enumerator.name = aEnum->aliasList[i];
        enumerator.value = aEnum->aliasValueList[i];

        enm->enumerators.emplace_back(std::move(enumerator));
    }

    //std::sort(enm->enumerators.begin(), enm->enumerators.end(),
    //          [](const Enum::Enumerator& aLhs, const Enum::Enumerator& aRhs) { return aLhs.value < aRhs.value; });

    m_types.emplace(enm->name.ToString(), enm);
}

void Dumper::CollectType(RED4ext::CClass* aClass)
{
    auto cls = std::make_shared<Class>();
    cls->raw = aClass;

    if (aClass->parent)
    {
        auto name = aClass->parent->name.ToString();
        if (m_types.count(name) == 0)
        {
            CollectType(aClass->parent);
        }

        auto type = m_types.at(name);
        auto parent = std::dynamic_pointer_cast<Class>(type);

        cls->parent = parent;
    }

    cls->name = aClass->name;
    cls->computedName = aClass->computedName;
    cls->size = aClass->size;
    cls->alignment = aClass->alignment;
    cls->holderSize = aClass->holderSize;
    cls->flags = aClass->flags;

    cls->props.reserve(aClass->props.size);
    for (auto prop : aClass->props)
    {
        cls->props.emplace_back(prop);
    }

    /*std::sort(cls->props.begin(), cls->props.end(), [](const RED4ext::CProperty* aLhs, const RED4ext::CProperty* aRhs) {
        return aLhs->valueOffset < aRhs->valueOffset;
    });*/

    for (auto func : aClass->funcs)
    {
        cls->funcs.emplace_back(func);
    }

    for (auto func : aClass->staticFuncs)
    {
        cls->funcs.emplace_back(func);
    }

    m_types.emplace(cls->name.ToString(), cls);
}

void Dumper::CollectStatics()
{
    auto rtti = RED4ext::CRTTISystem::Get();
    rtti->funcs.for_each([this](const RED4ext::CName& aName, RED4ext::CGlobalFunction* aFunction) {
        auto name = aName.ToString();

        auto pos = strstr(name, "::");
        if (pos)
        {
            std::string clsName(name, pos - name);
            auto cls = std::dynamic_pointer_cast<Class>(m_types.at(clsName.c_str()));

            cls->funcs.emplace_back(aFunction);
        }
        else
        {
            m_global.funcs.emplace_back(aFunction);
        }
    });
}

void Dumper::OrderFunctions()
{
    auto comp = [](const RED4ext::CBaseFunction* aLhs, const RED4ext::CBaseFunction* aRhs) {
        if (aLhs->flags.isStatic != aRhs->flags.isStatic)
        {
            return aLhs->flags.isStatic == true;
        }

        if (aLhs->flags.isNative != aRhs->flags.isNative)
        {
            return aLhs->flags.isNative == true;
        }

        if (aLhs->flags.isEvent != aRhs->flags.isEvent)
        {
            return aLhs->flags.isEvent == true;
        }

        return std::strcmp(aLhs->shortName.ToString(), aRhs->shortName.ToString()) < 0;
    };

    std::sort(m_global.funcs.begin(), m_global.funcs.end(), comp);

    for (auto [key, type] : m_types)
    {
        auto cls = std::dynamic_pointer_cast<Class>(type);
        if (!cls)
        {
            continue;
        }

        std::sort(cls->funcs.begin(), cls->funcs.end(), comp);
    }
}
