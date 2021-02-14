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

    if (aClass->parent)
    {
        CollectType(aClass->parent);

        auto name = aClass->parent->name.ToString();
        auto parent = std::dynamic_pointer_cast<Class>(m_types.at(name));

        cls->parent = parent;
    }

    cls->fullName = aClass->name;
    cls->computedName = aClass->name2;
    cls->size = aClass->size;
    cls->alignment = aClass->alignment;
    cls->holderSize = aClass->holderSize;
    cls->flags = aClass->flags;

    cls->props.reserve(aClass->props.size);
    for (auto nativeProp : aClass->props)
    {
        auto prop = ProcessType(nativeProp);
        cls->props.emplace_back(std::move(prop));
    }

    std::sort(cls->props.begin(), cls->props.end(),
              [](const Property& aLhs, const Property& aRhs) { return aLhs.valueOffset < aRhs.valueOffset; });

    for (auto nativeFunc : aClass->funcs)
    {
        auto func = ProcessType(nativeFunc);
        cls->funcs.emplace_back(std::move(func));
    }

    for (auto nativeFunc : aClass->staticFuncs)
    {
        auto func = ProcessType(nativeFunc);
        cls->funcs.emplace_back(std::move(func));
    }

    m_types.emplace(cls->fullName.ToString(), cls);
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

            auto func = ProcessType(aFunction);
            cls->funcs.emplace_back(std::move(func));
        }
        else
        {
            auto func = ProcessType(aFunction);
            m_global.funcs.emplace_back(std::move(func));
        }
    });
}

void Dumper::OrderFunctions()
{
    auto comp = [](const Function& aLhs, const Function& aRhs) {
        if (aLhs.flags.isStatic != aRhs.flags.isStatic)
        {
            return aLhs.flags.isStatic == true;
        }

        if (aLhs.flags.isNative != aRhs.flags.isNative)
        {
            return aLhs.flags.isNative == true;
        }

        return std::strcmp(aLhs.shortName.ToString(), aRhs.shortName.ToString()) < 0;
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

Property Dumper::ProcessType(RED4ext::CProperty* aProperty)
{
    Property prop{};
    aProperty->type->GetName(prop.type.fullName);

    prop.fullName = aProperty->name;
    prop.group = aProperty->group;
    prop.valueOffset = aProperty->valueOffset;
    prop.group = aProperty->group;
    prop.flags = aProperty->flags;

    return prop;
}

Function Dumper::ProcessType(RED4ext::CBaseFunction* aFunction)
{
    Function func{};
    func.fullName = aFunction->fullName;
    func.shortName = aFunction->shortName;
    func.flags = aFunction->flags;

    if (aFunction->returnType)
    {
        auto prop = ProcessType(aFunction->returnType);
        func.returnType = std::make_unique<Property>(std::move(prop));
    }

    func.params.reserve(aFunction->params.size);
    for (auto nativeParam : aFunction->params)
    {
        auto param = ProcessType(nativeParam);
        func.params.emplace_back(param);
    }

    return func;
}

Function Dumper::ProcessType(RED4ext::CGlobalFunction* aFunction)
{
    auto func = ProcessType(static_cast<RED4ext::CBaseFunction*>(aFunction));
    func.index = aFunction->index;

    return func;
}

Function Dumper::ProcessType(RED4ext::CClassFunction* aFunction)
{
    auto func = ProcessType(static_cast<RED4ext::CBaseFunction*>(aFunction));
    func.index = aFunction->index;

    return func;
}
