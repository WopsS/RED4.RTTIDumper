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

    // WolvenKit has custom classes (with more functions or fields) for some RTTI classes.
    m_customClasses.emplace("Color");
    m_customClasses.emplace("C2dArray");
    m_customClasses.emplace("CMesh");

    m_customClasses.emplace("AITrafficWorkspotCompiled");
    m_customClasses.emplace("animAnimFeatureEntry");
    m_customClasses.emplace("animAnimGraph");
    m_customClasses.emplace("animAnimNode_Base");
    m_customClasses.emplace("animAnimNode_BlendFromPose");
    m_customClasses.emplace("animAnimNode_BlendMultiple");
    m_customClasses.emplace("animAnimNode_ConditionalSegmentBegin");
    m_customClasses.emplace("animAnimNode_ConditionalSegmentEnd");
    m_customClasses.emplace("animAnimNode_FloatInput");
    m_customClasses.emplace("animAnimNode_ForegroundSegmentBegin");
    m_customClasses.emplace("animAnimNode_ForegroundSegmentEnd");
    m_customClasses.emplace("animAnimNode_IntInput");
    m_customClasses.emplace("animAnimNode_LookAtController");
    m_customClasses.emplace("animAnimNode_MathExpressionFloat");
    m_customClasses.emplace("animAnimNode_MathExpressionPose");
    m_customClasses.emplace("animAnimNode_MathExpressionVector");
    m_customClasses.emplace("animAnimNode_ReferencePoseTerminator");
    m_customClasses.emplace("animAnimNode_Sermo");
    m_customClasses.emplace("animAnimNode_SkAnim");
    m_customClasses.emplace("animAnimNode_StackTracksExtender");
    m_customClasses.emplace("animAnimNode_StackTracksShrinker");
    m_customClasses.emplace("animAnimNode_StackTransformsExtender");
    m_customClasses.emplace("animAnimNode_StackTransformsShrinker");
    m_customClasses.emplace("animAnimNode_StateMachine");
    m_customClasses.emplace("animAnimStateTransitionCondition_IntFeature");
    m_customClasses.emplace("animAnimStateTransitionInterpolator_Blend");
    m_customClasses.emplace("animAnimVariable");
    m_customClasses.emplace("animCollisionRoundedShape");
    m_customClasses.emplace("animDangleConstraint_Simulation");
    m_customClasses.emplace("animDangleConstraint_SimulationDyng");
    m_customClasses.emplace("animDyngConstraintEllipsoid");
    m_customClasses.emplace("animDyngParticle");
    m_customClasses.emplace("animIDyngConstraint");
    m_customClasses.emplace("animLookAtPartInfo");
    m_customClasses.emplace("animLookAtPartsDependency");
    m_customClasses.emplace("animLookAtRequest");
    m_customClasses.emplace("animRig");
    m_customClasses.emplace("CMaterialInstance");
    m_customClasses.emplace("CMaterialTemplate");
    m_customClasses.emplace("gameDeviceResourceData");
    m_customClasses.emplace("gameJournalCodexDescription");
    m_customClasses.emplace("gameLocationResource");
    m_customClasses.emplace("gameLootResourceData");
    m_customClasses.emplace("meshMeshParamSpeedTreeWind");
    m_customClasses.emplace("MorphTargetMesh");
    m_customClasses.emplace("physicsColliderMesh");
    m_customClasses.emplace("physicsMaterialLibraryResource");
    m_customClasses.emplace("scnAnimationRid");
    m_customClasses.emplace("scnAnimName");
    m_customClasses.emplace("scnAnimName");
    m_customClasses.emplace("worldTrafficLanesSpotsResource");
    m_customClasses.emplace("worldNode");

    // Some ordinals needs to be skipped.
    m_skippedOrdinals.emplace("CMesh", std::unordered_set<size_t>{4, 20});

    // WolvenKit has custom names for some types.
    m_typeMappings.emplace("Uint8", "UInt8");
    m_typeMappings.emplace("Uint16", "UInt16");
    m_typeMappings.emplace("Uint32", "UInt32");
    m_typeMappings.emplace("Uint64", "UInt64");

    m_typeMappings.emplace("Color", "CColor");
    m_typeMappings.emplace("String", "CString");
    m_typeMappings.emplace("Variant", "CVariant");

    // This is a simple for some reason.
    m_typeMappings.emplace("multiChannelCurve:Float", "multiChannelCurve<CFloat>");
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

    std::string name = aClass->name.ToString();
    auto skippedOrdinals = m_skippedOrdinals.find(name);

    auto elem = m_customClasses.find(name);
    if (elem != m_customClasses.end())
    {
        name += "_";
    }

    std::fstream file(dir / (name + ".cs"), std::ios::out);

    file << "using CP77.CR2W.Reflection;" << std::endl;

    if (!aClass->props.empty())
    {
        file << "using FastMember;" << std::endl;
        file << "using static CP77.CR2W.Types.Enums;" << std::endl;
    }

    file << std::endl;
    file << "namespace CP77.CR2W.Types" << std::endl;
    file << "{" << std::endl;
    file << "\t[REDMeta]" << std::endl;

    file << "\tpublic class " << name << " : ";
    if (aClass->parent)
    {
        file << aClass->parent->name.ToString();
    }
    else
    {
        file << "CVariable";
    }

    file << std::endl;
    file << "\t{" << std::endl;

    auto ordinal = GetOrdinalStart(aClass);

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

    if (aClass->props.size())
    {
        file << std::endl;
    }

    file << "\t\tpublic " << name << "(CR2WFile cr2w, CVariable parent, string name) : base(cr2w, parent, name) { }"
         << std::endl;

    file << "\t}" << std::endl;
    file << "}" << std::endl;
}

void WolvenKitWriter::Flush()
{
}

void WolvenKitWriter::Write(std::fstream& aFile, RED4ext::IRTTIType* aType)
{
    RED4ext::CName cname;
    aType->GetName(cname);

    std::string_view name = cname.ToString();
    auto elem = m_typeMappings.find(name.data());
    if (elem != m_typeMappings.end())
    {
        name = elem->second;
    }

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
    std::string name = aProperty->name.ToString();

    aFile << "\t\t";
    aFile << "[Ordinal(" << aOrdinal << ")] ";
    aFile << "[RED(\"" << name << "\"";

    auto type = aProperty->type;

    using ERTTIType = RED4ext::ERTTIType;
    switch (type->GetType())
    {
    case ERTTIType::StaticArray:
    {
        auto arr = static_cast<RED4ext::CStaticArray*>(type);
        aFile << ", " << arr->size;
        break;
    }
    case ERTTIType::NativeArray:
    {
        auto arr = static_cast<RED4ext::CNativeArray*>(type);
        aFile << ", " << arr->size;
        break;
    }
    }

    aFile << ")] ";
    aFile << "public ";

    Write(aFile, type);
    aFile << " ";

    name = SanitizeGeneral(name);
    name[0] = std::toupper(name[0]);
    name = Sanitize(name);

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

        aFile << aProperty->valueOffset;
    }

    aFile << " ";
    aFile << "{ get; set; }" << std::endl;
}

size_t WolvenKitWriter::GetOrdinalStart(std::shared_ptr<Class> aClass)
{
    if (aClass->parent)
    {
        auto result = aClass->parent->props.size();
        result += GetOrdinalStart(aClass->parent);

        return result;
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
