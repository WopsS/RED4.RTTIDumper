#include "stdafx.hpp"
#include "WolvenKitWriter.hpp"
#include "Utils.hpp"

WolvenKitWriter::WolvenKitWriter(const std::filesystem::path& aRootDir)
    : m_dir(aRootDir / L"csharp")
{
    if (!std::filesystem::exists(m_dir))
    {
        std::filesystem::create_directories(m_dir);
    }

    // WolvenKit has custom names for some types.
    m_typeMappings.emplace("bool", "CBool");
    m_typeMappings.emplace("int", "CInt32");
    m_typeMappings.emplace("float", "CFloat");

    m_typeMappings.emplace("Uint8", "UInt8");
    m_typeMappings.emplace("Uint16", "UInt16");
    m_typeMappings.emplace("Uint32", "UInt32");
    m_typeMappings.emplace("Uint64", "UInt64");

    m_typeMappings.emplace("String", "CString");
    m_typeMappings.emplace("string", "CString");
    m_typeMappings.emplace("Color", "CColor");
    m_typeMappings.emplace("Matrix", "CMatrix");
    m_typeMappings.emplace("Variant", "CVariant");

    // This is a simple for some reason.
    m_typeMappings.emplace("multiChannelCurve:Float", "multiChannelCurve<CFloat>");

    // WolvenKit has custom classes (with more functions or fields) for some RTTI classes.
    m_customClasses.emplace("CColor", 0);
    m_customClasses.emplace("C2dArray", 0);
    m_customClasses.emplace("CMatrix", 0);
    m_customClasses.emplace("CMesh", 0);

    m_customClasses.emplace("AITrafficWorkspotCompiled", 0);
    m_customClasses.emplace("STextureGroupSetup", 1);
    m_customClasses.emplace("animAnimFeatureEntry", 0);
    m_customClasses.emplace("animAnimGraph", 0);
    m_customClasses.emplace("animAnimNode_AddSnapToTerrainIkRequest", 0);
    m_customClasses.emplace("animAnimNode_Base", 10);
    m_customClasses.emplace("animAnimNode_BlendFromPose", 0);
    m_customClasses.emplace("animAnimNode_BlendMultiple", 0);
    m_customClasses.emplace("animAnimNode_ConditionalSegmentBegin", 0);
    m_customClasses.emplace("animAnimNode_ConditionalSegmentEnd", 0);
    m_customClasses.emplace("animAnimNode_Drag", 0);
    m_customClasses.emplace("animAnimNode_FloatInput", 0);
    m_customClasses.emplace("animAnimNode_FloatTrackDirectConnConstraint", 1);
    m_customClasses.emplace("animAnimNode_ForegroundSegmentBegin", 0);
    m_customClasses.emplace("animAnimNode_ForegroundSegmentEnd", 0);
    m_customClasses.emplace("animAnimNode_IntInput", 0);
    m_customClasses.emplace("animAnimNode_LookAtController", 0);
    m_customClasses.emplace("animAnimNode_MathExpressionFloat", 0);
    m_customClasses.emplace("animAnimNode_MathExpressionPose", 0);
    m_customClasses.emplace("animAnimNode_MathExpressionVector", 0);
    m_customClasses.emplace("animAnimNode_ReferencePoseTerminator", 0);
    m_customClasses.emplace("animAnimNode_Sermo", 0);
    m_customClasses.emplace("animAnimNode_SkAnim", 2);
    m_customClasses.emplace("animAnimNode_StackTracksExtender", 0);
    m_customClasses.emplace("animAnimNode_StackTracksShrinker", 0);
    m_customClasses.emplace("animAnimNode_StackTransformsExtender", 0);
    m_customClasses.emplace("animAnimNode_StackTransformsShrinker", 0);
    m_customClasses.emplace("animAnimNode_StateMachine", 0);
    m_customClasses.emplace("animAnimStateTransitionCondition_IntFeature", 0);
    m_customClasses.emplace("animAnimStateTransitionInterpolator_Blend", 0);
    m_customClasses.emplace("animAnimVariable", 1);
    m_customClasses.emplace("animCollisionRoundedShape", 0);
    m_customClasses.emplace("animDangleConstraint_Simulation", 5);
    m_customClasses.emplace("animDangleConstraint_SimulationDyng", 0);
    m_customClasses.emplace("animDyngConstraintEllipsoid", 0);
    m_customClasses.emplace("animDyngParticle", 0);
    m_customClasses.emplace("animIDyngConstraint", 1);
    m_customClasses.emplace("animLipsyncMapping", 1);
    m_customClasses.emplace("animLookAtPartInfo", 0);
    m_customClasses.emplace("animLookAtPartsDependency", 0);
    m_customClasses.emplace("animLookAtRequest", 0);
    m_customClasses.emplace("animRig", 0);
    m_customClasses.emplace("AreaShapeOutline", 0);
    m_customClasses.emplace("CMaterialInstance", 0);
    m_customClasses.emplace("CMaterialTemplate", 0);
    m_customClasses.emplace("gameDeviceResourceData", 0);
    m_customClasses.emplace("gameJournalCodexDescription", 0);
    m_customClasses.emplace("gameJournalInternetPage", 1);
    m_customClasses.emplace("gameLocationResource", 0);
    m_customClasses.emplace("gameLootResourceData", 0);
    m_customClasses.emplace("gameSmartObjectAnimationDatabase", 0);
    m_customClasses.emplace("meshMeshParamSpeedTreeWind", 0);
    m_customClasses.emplace("MorphTargetMesh", 0);
    m_customClasses.emplace("physicsColliderMesh", 0);
    m_customClasses.emplace("physicsMaterialLibraryResource", 0);
    m_customClasses.emplace("scnAnimationRid", 0);
    m_customClasses.emplace("scnAnimName", 0);
    m_customClasses.emplace("scnAnimName", 0);
    m_customClasses.emplace("worldInstancedMeshNode", 0);
    m_customClasses.emplace("worldInstancedOccluderNode", 0);
    m_customClasses.emplace("worldTrafficLanesSpotsResource", 0);
    m_customClasses.emplace("worldNode", 0);
    m_customClasses.emplace("worldStreamingSector", 0);
    m_customClasses.emplace("worldTrafficCompiledNode", 0);

    // Some ordinals needs to be skipped.
    m_skippedOrdinals.emplace("CMesh", std::unordered_set<size_t>{1, 5, 21});
    m_skippedOrdinals.emplace("animAnimNode_AddSnapToTerrainIkRequest", std::unordered_set<size_t>{12});
    m_skippedOrdinals.emplace("animAnimNode_ConditionalSegmentBegin", std::unordered_set<size_t>{12});
    m_skippedOrdinals.emplace("animAnimNode_Drag", std::unordered_set<size_t>{16});
    m_skippedOrdinals.emplace("animDyngParticle", std::unordered_set<size_t>{0});
    m_skippedOrdinals.emplace("animLipsyncMapping", std::unordered_set<size_t>{3});
    m_skippedOrdinals.emplace("worldNode", std::unordered_set<size_t>{0, 1});
}

void WolvenKitWriter::Write(Global& aGlobal)
{
}

void WolvenKitWriter::Write(std::shared_ptr<Class> aClass)
{
    auto dir = m_dir / L"classes";
    if (!std::filesystem::exists(dir))
    {
        std::filesystem::create_directories(dir);
    }

    auto orgName = GetWolvenType(aClass->name.ToString());
    auto name = orgName;

    auto isWritten = m_isWritten.find(name);
    if (isWritten != m_isWritten.end())
    {
        return;
    }

    if (aClass->parent)
    {
        Write(aClass->parent);
    }



    auto skippedOrdinals = m_skippedOrdinals.find(name);

    auto elem = m_customClasses.find(name);
    if (elem != m_customClasses.end())
    {
        name += "_";
    }

    std::fstream file(dir / (name + ".cs"), std::ios::out);

    file << "using WolvenKit.RED4.CR2W.Reflection;" << std::endl;

    if (!aClass->props.empty())
    {
        file << "using FastMember;" << std::endl;
        file << "using static WolvenKit.RED4.CR2W.Types.Enums;" << std::endl;
    }

    file << std::endl;
    file << "namespace WolvenKit.RED4.CR2W.Types" << std::endl;
    file << "{" << std::endl;
    file << "\t[REDMeta]" << std::endl;

    file << "\tpublic class " << name << " : ";
    if (aClass->parent)
    {
        auto parent = GetWolvenType(aClass->parent->name.ToString());
        file << parent;
    }
    else
    {
        file << "CVariable";
    }

    file << std::endl;
    file << "\t{" << std::endl;

    auto ordinal = GetOrdinalStart(aClass);
    if (name == "worldInstancedDestructibleMeshNode")
    {
        ordinal = 1000;
    }

    for (auto prop : aClass->raw->props)
    {
        auto csType = GetCSType(prop->type);
        auto name = SanitizeGeneral(prop->name.ToString());
        name[0] = std::tolower(name[0]);

        if (CheckForDuplicate(prop->parent, prop))
        {
            name += "_" + std::to_string(prop->valueOffset);
        }

        file << "\t\tprivate " << csType << " _" << name << ";" << std::endl;
    }

    file << std::endl;

    // We want to write them as they are in the RTTI type, not orderd by offset.
    for (auto prop : aClass->raw->props)
    {
        if (skippedOrdinals != m_skippedOrdinals.end())
        {
            const auto& set = skippedOrdinals->second;
            while (set.count(ordinal))
            {
                ordinal++;
            }
        }

        Write(file, prop, ordinal++);
    }

    if (elem != m_customClasses.end())
    {
        ordinal += elem->second;
    }

    m_nextOrdinals.emplace(orgName, ordinal);

    file << "\t\tpublic " << name << "(CR2WFile cr2w, CVariable parent, string name) : base(cr2w, parent, name) { }"
         << std::endl;

    file << "\t}" << std::endl;
    file << "}" << std::endl;

    m_isWritten.emplace(orgName);
}

void WolvenKitWriter::Write(std::shared_ptr<Enum> aEnum)
{
}

void WolvenKitWriter::Flush()
{
}

void WolvenKitWriter::Write(std::fstream& aFile, RED4ext::IRTTIType* aType)
{
    RED4ext::CName cname;
    aType->GetName(cname);

    auto name = GetWolvenType(cname.ToString());

    using ERTTIType = RED4ext::ERTTIType;
    switch (aType->GetType())
    {
    case ERTTIType::Fundamental:
    {
        aFile << "C" << name;
        break;
    }
    case ERTTIType::Array:
    {
        aFile << "CArray<";

        auto arr = static_cast<RED4ext::CArray*>(aType);
        Write(aFile, arr->innerType);

        aFile << ">";

        break;
    }
    case ERTTIType::Enum:
    case ERTTIType::BitField:
    {
        aFile << "CEnum<" << name << ">";
        break;
    }
    case ERTTIType::StaticArray:
    {
        aFile << "CStatic<";

        auto arr = static_cast<RED4ext::CStaticArray*>(aType);
        Write(aFile, arr->innerType);

        aFile << ">";

        break;
    }
    case ERTTIType::NativeArray:
    {
        aFile << "CArrayFixedSize<";

        auto arr = static_cast<RED4ext::CNativeArray*>(aType);
        Write(aFile, arr->innerType);

        aFile << ">";

        break;
    }
    case ERTTIType::Handle:
    {
        aFile << "CHandle<";

        auto handle = static_cast<RED4ext::CHandle*>(aType);
        Write(aFile, handle->innerType);

        aFile << ">";
        break;
    }
    case ERTTIType::WeakHandle:
    {
        aFile << "wCHandle<";

        auto whandle = static_cast<RED4ext::CWeakHandle*>(aType);
        Write(aFile, whandle->innerType);

        aFile << ">";
        break;
    }
    case ERTTIType::ResourceReference:
    {
        aFile << "rRef<";

        auto rRef = static_cast<RED4ext::CResourceReference*>(aType);
        Write(aFile, rRef->innerType);

        aFile << ">";

        break;
    }
    case ERTTIType::ResourceAsyncReference:
    {
        aFile << "raRef<";

        auto raRef = static_cast<RED4ext::CResourceAsyncReference*>(aType);
        Write(aFile, raRef->innerType);

        aFile << ">";

        break;
    }
    case ERTTIType::LegacySingleChannelCurve:
    {
        aFile << "curveData<";

        auto curve = static_cast<RED4ext::CLegacySingleChannelCurve*>(aType);
        Write(aFile, curve->curveType);

        aFile << ">";

        break;
    }
    default:
    {
        aFile << name;
        break;
    }
    }
}

void WolvenKitWriter::Write(std::fstream& aFile, RED4ext::CProperty* aProperty, size_t aOrdinal)
{
    std::string orgName = aProperty->name.ToString();

    aFile << "\t\t[Ordinal(" << aOrdinal << ")] " << std::endl;
    aFile << "\t\t[RED(\"" << orgName << "\"" << GetFixedSize(aProperty->type) << ")] " << std::endl;


    auto csType = GetCSType(aProperty->type);

    aFile << "\t\tpublic " << csType << " ";

    auto name = SanitizeGeneral(orgName);
    name[0] = std::toupper(name[0]);
    name = Sanitize(name);

    auto privateName = SanitizeGeneral(orgName);
    privateName[0] = std::tolower(privateName[0]);
    privateName = "_" + privateName;

    auto parent = aProperty->parent;

    // Check if the variable has the same name as the class.
    if (_stricmp(name.c_str(), parent->name.ToString()) == 0)
    {
        name += "_";
    }

    aFile << name;

    if (CheckForDuplicate(parent, aProperty))
    {
        // Do not append "_" twice.
        if (name[name.size() - 1] != '_')
        {
            aFile << "_";
        }

        privateName += "_" + std::to_string(aProperty->valueOffset);
        aFile << aProperty->valueOffset;
    }

    RED4ext::CName typeName;
    aProperty->type->GetName(typeName);

    aFile << std::endl;
    aFile << "\t\t{" << std::endl;
    aFile << "\t\t\tget => GetProperty(ref " << privateName << ");" << std::endl;
    aFile << "\t\t\tset => SetProperty(ref " << privateName << ", value);" << std::endl;
    aFile << "\t\t}" << std::endl;
    aFile << std::endl;
}

std::string WolvenKitWriter::GetWolvenType(const char* aName)
{
    auto elem = m_typeMappings.find(aName);
    if (elem != m_typeMappings.end())
    {
        return elem->second;
    }

    return aName;
}

std::string WolvenKitWriter::GetFixedSize(RED4ext::IRTTIType* aType)
{
    using ERTTIType = RED4ext::ERTTIType;
    switch (aType->GetType())
    {
    case ERTTIType::StaticArray:
    {
        auto arr = static_cast<RED4ext::CStaticArray*>(aType);
        return ", " + std::to_string(arr->size) + GetFixedSize(arr->innerType);
    }
    case ERTTIType::NativeArray:
    {
        auto arr = static_cast<RED4ext::CNativeArray*>(aType);
        return ", " + std::to_string(arr->size) + GetFixedSize(arr->innerType);
    }
    default:
    {
        return "";
    }
    }
}

std::string WolvenKitWriter::GetCSType(RED4ext::IRTTIType* aType)
{
    RED4ext::CName cname;
    aType->GetName(cname);

    auto name = GetWolvenType(cname.ToString());

    using ERTTIType = RED4ext::ERTTIType;
    switch (aType->GetType())
    {
    case ERTTIType::Fundamental:
    {
        return "C" + name;
    }
    case ERTTIType::Array:
    {
        auto arr = static_cast<RED4ext::CArray*>(aType);
        return "CArray<" + GetCSType(arr->innerType) + ">";
    }
    case ERTTIType::Enum:
    case ERTTIType::BitField:
    {
        return "CEnum<" + name + ">";
    }
    case ERTTIType::StaticArray:
    {
        auto arr = static_cast<RED4ext::CStaticArray*>(aType);
        return "CStatic<" + GetCSType(arr->innerType) + ">";
    }
    case ERTTIType::NativeArray:
    {
        auto arr = static_cast<RED4ext::CNativeArray*>(aType);
        return "CArrayFixedSize<" + GetCSType(arr->innerType) + ">";
    }
    case ERTTIType::Handle:
    {
        auto handle = static_cast<RED4ext::CHandle*>(aType);
        return "CHandle<" + GetCSType(handle->innerType) + ">";
    }
    case ERTTIType::WeakHandle:
    {
        auto whandle = static_cast<RED4ext::CWeakHandle*>(aType);
        return "wCHandle<" + GetCSType(whandle->innerType) + ">";
    }
    case ERTTIType::ResourceReference:
    {
        auto rRef = static_cast<RED4ext::CResourceReference*>(aType);
        return "rRef<" + GetCSType(rRef->innerType) + ">";
    }
    case ERTTIType::ResourceAsyncReference:
    {
        auto raRef = static_cast<RED4ext::CResourceAsyncReference*>(aType);
        return "raRef<" + GetCSType(raRef->innerType) + ">";
    }
    case ERTTIType::LegacySingleChannelCurve:
    {
        auto curve = static_cast<RED4ext::CLegacySingleChannelCurve*>(aType);
        return "curveData<" + GetCSType(curve->curveType) + ">";
    }
    default:
    {
        return name;
    }
    }
}

size_t WolvenKitWriter::GetOrdinalStart(std::shared_ptr<Class> aClass)
{
    if (aClass->parent)
    {
        auto parentName = GetWolvenType(aClass->parent->name.ToString());
        auto elem = m_nextOrdinals.find(parentName);
        if (elem != m_nextOrdinals.end())
        {
            return elem->second;
        }

        throw std::runtime_error("unknown parent");
    }

    return 0;
}

bool WolvenKitWriter::CheckForDuplicate(RED4ext::CClass* aClass, RED4ext::CProperty* aProperty)
{
    // Exclude self.
    return CountOccurences(aClass, aProperty) > 1;
}

size_t WolvenKitWriter::CountOccurences(RED4ext::CClass* aClass, RED4ext::CProperty* aProperty)
{
    size_t occurences = 0;
    if (aClass->parent)
    {
        occurences += CountOccurences(aClass->parent, aProperty);
    }

    for (auto prop : aClass->props)
    {
        // Properties can have almost the same name, only the first letter is lowercase or uppercase.
        if (_stricmp(prop->name.ToString(), aProperty->name.ToString()) == 0)
        {
            occurences++;
        }
    }

    return occurences;
}

std::string WolvenKitWriter::Sanitize(const std::string& aInput)
{
    static std::regex reservedKeywords(R"(\bEquals\b|\bPropertyChanged\b|\bRead\b)");
    return std::regex_replace(aInput, reservedKeywords, "$&_");
}
