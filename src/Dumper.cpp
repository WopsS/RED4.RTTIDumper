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

        throw std::runtime_error("unhandled type");
    }

    aWriter->Flush();
}

void Dumper::CollectTypes()
{
    auto rtti = RED4ext::CRTTISystem::Get();
    rtti->types.for_each([this](RED4ext::CName aName, RED4ext::IRTTIType* aType) {
        auto typeId = aType->GetType();
        switch (typeId)
        {
        case RED4ext::ERTTIType::Class:
        {
            auto cls = static_cast<RED4ext::CClass*>(aType);
            CollectType(cls);

            break;
        }
        }
    });
}

void Dumper::CollectType(RED4ext::CClass* aClass)
{
    auto cls = std::make_shared<Class>();
    cls->raw = aClass;

    if (aClass->parent)
    {
        CollectType(aClass->parent);

        auto name = aClass->parent->name.ToString();
        auto parent = std::dynamic_pointer_cast<Class>(m_types.at(name));

        cls->parent = parent;
    }

    cls->name = aClass->name;
    cls->computedName = aClass->name2;
    cls->size = aClass->size;
    cls->alignment = aClass->alignment;
    cls->holderSize = aClass->holderSize;
    cls->flags = aClass->flags;

    cls->props.reserve(aClass->props.size);
    for (auto prop : aClass->props)
    {
        cls->props.emplace_back(prop);
    }

    std::sort(cls->props.begin(), cls->props.end(), [](const RED4ext::CProperty* aLhs, const RED4ext::CProperty* aRhs) {
        return aLhs->valueOffset < aRhs->valueOffset;
    });

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
