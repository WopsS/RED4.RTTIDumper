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

void TextWriter::Write(std::shared_ptr<Enum> aEnum)
{
    auto dir = m_dir / L"enums";
    if (!std::filesystem::exists(dir))
    {
        std::filesystem::create_directories(dir);
    }

    std::string name = aEnum->name.ToString();
    std::fstream file(dir / (name + ".txt"), std::ios::out);

    file << "enum " << name << std::endl;
    file << "{" << std::endl;

    for (size_t i = 0; i < aEnum->enumerators.size(); i++)
    {
        const auto& enumerator = aEnum->enumerators[i];
        file << "\t" << enumerator.name.ToString() << " = ";

        switch (aEnum->actualSize)
        {
        case sizeof(int8_t):
        case sizeof(int16_t):
        {
            file << static_cast<int16_t>(enumerator.value);
            break;
        }
        case sizeof(int32_t):
        {
            file << static_cast<int32_t>(enumerator.value);
            break;
        }
        case sizeof(int64_t):
        {
            file << static_cast<int64_t>(enumerator.value);
            break;
        }
        default:
        {
            file << enumerator.value;
            break;
        }
        }

        if (i < aEnum->enumerators.size() - 1)
        {
            file << ",";
        }

        file << std::endl;
    }

    file << "}";
}

void TextWriter::Write(std::shared_ptr<BitField> aBit)
{
    auto dir = m_dir / L"bitfields";
    if (!std::filesystem::exists(dir))
    {
        std::filesystem::create_directories(dir);
    }

    std::string name = aBit->name.ToString();
    std::fstream file(dir / (name + ".txt"), std::ios::out);

    file << "bitfield " << name << std::endl;
    file << "{" << std::endl;

    auto validBits = aBit->validBits;
    auto counter = 0;
    while (validBits != 0)
    {
        auto bit = validBits & 1;
        validBits >>= 1;

        if (bit == 1)
        {
            file << "\t" << aBit->bitNames[counter].ToString() << " = 1 << " << counter;

            if (validBits != 0)
            {
                file << ",";
            }

            file << std::endl;
        }

        counter++;
    }

    file << "}";
}

void TextWriter::Flush()
{
}

void TextWriter::Write(std::fstream& aFile, std::shared_ptr<Class> aClass)
{
    if (aClass->flags.isNative)
    {
        aFile << "import ";
    }

    if (aClass->flags.isScriptedStruct)
    {
        aFile << "struct";
    }
    else if (aClass->flags.isScriptedClass)
    {
        aFile << "class";
    }
    else
    {
        static auto serializableType = RED4ext::CRTTISystem::Get()->GetClass("ISerializable");
        if (!aClass->raw->IsA(serializableType))
        {
            aFile << "struct";
        }
        else
        {
            aFile << "class";
        }
    }

    if (aClass->flags.isAbstract)
    {
        aFile << " abstract";
    }

    aFile << " " << aClass->name.ToString();

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
        aFile << ";" << std::endl;
        // aFile << "; // " << std::hex << std::showbase << prop->valueOffset << std::endl;
    }

    if (!aClass->props.empty() && !aClass->funcs.empty())
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

            if (previous->flags.isStatic != func->flags.isStatic || previous->flags.isEvent != func->flags.isEvent ||
                previous->flags.isNative != func->flags.isNative)
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
    auto typeName = aProperty->type->GetName();
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

    if (aFunction->flags.isEvent)
    {
        aFile << "event";
    }
    else if (aFunction->flags.isExec)
    {
        aFile << "exec";
    }
    else
    {
        aFile << "function";
    }

    aFile << " ";
    aFile << aFunction->shortName.ToString();
    aFile << "(";

    for (auto param : aFunction->params)
    {
        if (param->flags.isOptional)
        {
            aFile << "optional ";
        }

        if (param->flags.isOut)
        {
            aFile << "out ";
        }

        Write(aFile, param);

        if (param != aFunction->params[aFunction->params.size - 1])
        {
            aFile << ", ";
        }
    }

    aFile << ")";

    if (aFunction->returnType)
    {
        auto name = aFunction->returnType->type->GetName();
        aFile << " : " << name.ToString();
    }

    aFile << "; // " << aFunction->fullName.ToString();
}
