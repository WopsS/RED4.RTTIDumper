#include "stdafx.hpp"
#include "TextWriter.hpp"

TextWriter::TextWriter(const std::filesystem::path& aRootDir)
    : m_dir(aRootDir / L"text")
{
    if (!std::filesystem::exists(m_dir))
    {
        std::filesystem::create_directories(m_dir);
    }
}

void TextWriter::Write(Global& aGlobal)
{
    std::fstream file(m_dir / L"globals.txt", std::ios::out);

    for (size_t i = 0; i < aGlobal.funcs.size(); i++)
    {
        auto func = aGlobal.funcs[i];

        // Separate "import static", "static" and normal functions by an empty line.
        if (i > 0)
        {
            auto previous = aGlobal.funcs[i - 1];

            if (previous->flags.isStatic != func->flags.isStatic || previous->flags.isNative != func->flags.isNative)
            {
                file << std::endl;
            }
        }

        Write(file, func);

        if (i != aGlobal.funcs.size() - 1)
        {
            file << std::endl;
        }
    }
}

void TextWriter::Write(std::shared_ptr<Class> aClass)
{
    auto dir = m_dir / L"classes";
    if (!std::filesystem::exists(dir))
    {
        std::filesystem::create_directories(dir);
    }

    
    std::string name = aClass->name.ToString();
    std::fstream file(dir / (name + ".txt"), std::ios::out);

    Write(file, aClass);
}

void TextWriter::Flush()
{
}

void TextWriter::Write(std::fstream& aFile, std::shared_ptr<Class> aClass)
{
    aFile << "class " << aClass->name.ToString();
    if (aClass->parent)
    {
        aFile << " extends " << aClass->parent->name.ToString();
    }

    aFile << std::endl;
    aFile << "{" << std::endl;

    for (auto prop : aClass->props)
    {
        aFile << "\t";

        if (prop->flags.isPublic)
        {
            aFile << "public    ";
        }
        else if (prop->flags.isProtected)
        {
            aFile << "protected ";
        }
        else if (prop->flags.isPrivate)
        {
            aFile << "private   ";
        }

        aFile << "var ";
        Write(aFile, prop);
        aFile << "; // " << std::hex << std::showbase << prop->valueOffset << std::endl;
    }

    if (!aClass->props.empty())
    {
        aFile << std::endl;
    }

    for (size_t i = 0; i < aClass->funcs.size(); i++)
    {
        auto func = aClass->funcs[i];

        // Separate "import static", "static" and normal functions by an empty line.
        if (i > 0)
        {
            auto& previous = aClass->funcs[i - 1];

            if (previous->flags.isStatic != func->flags.isStatic || previous->flags.isNative != func->flags.isNative)
            {
                aFile << std::endl;
            }
        }

        aFile << "\t";
        Write(aFile, func);
        aFile << std::endl;
    }

    aFile << "}";

    if (aClass->parent)
    {
        aFile << std::endl;
        aFile << std::endl;

        Write(aFile, aClass->parent);
    }
}

void TextWriter::Write(std::fstream& aFile, const RED4ext::CProperty* aProperty)
{
    RED4ext::CName typeName;
    aProperty->type->GetName(typeName);

    aFile << aProperty->name.ToString() << " : " << typeName.ToString();
}

void TextWriter::Write(std::fstream& aFile, const RED4ext::CBaseFunction* aFunction)
{
    if (aFunction->flags.isNative)
    {
        aFile << "import ";
    }

    if (aFunction->flags.isStatic)
    {
        aFile << "static ";
    }

    aFile << "function " << aFunction->shortName.ToString();
    aFile << "(";

    for (size_t i = 0; i < aFunction->params.size; i++)
    {
        auto param = aFunction->params[i];
        if (param->flags.isOptional)
        {
            aFile << "optional ";
        }

        if (param->flags.isOut)
        {
            aFile << "out ";
        }

        Write(aFile, param);

        if (i != aFunction->params.size - 1)
        {
            aFile << ", ";
        }
    }

    aFile << ")";

    if (aFunction->returnType)
    {
        RED4ext::CName name;
        aFunction->returnType->type->GetName(name);

        aFile << " : " << name.ToString();
    }

    aFile << "; // " << aFunction->fullName.ToString();
}
