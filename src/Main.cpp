#include "stdafx.hpp"

#include "Dumper.hpp"
#include "SuspendThreads.hpp"
#include "Writers/JsonWriter.hpp"
#include "Writers/TextWriter.hpp"
#include "Writers/WolvenKitWriter.hpp"

RED4EXT_C_EXPORT bool RED4EXT_CALL Load(RED4ext::PluginHandle aHandle, const RED4ext::IRED4ext* aInterface)
{
    // Suspend other threads, if this process will take long (> 120 seconds) watchdog will bitch and close the
    // process. Since the watchdog run on a different thread, this will do the trick.
    SuspendThreads _;

    auto dumpsDir = std::filesystem::current_path() / L"dumps";

    std::vector<std::shared_ptr<IWriter>> writers;
    writers.emplace_back(new TextWriter(dumpsDir));
    writers.emplace_back(new JsonWriter(dumpsDir));
    writers.emplace_back(new WolvenKitWriter(dumpsDir));
    
    Dumper dumper;
    for (auto writer : writers)
    {
        dumper.Run(writer);
    }

    return true;
}

RED4EXT_C_EXPORT void RED4EXT_CALL Unload()
{
}

RED4EXT_C_EXPORT void RED4EXT_CALL Query(RED4ext::PluginInfo* aInfo)
{
    aInfo->name = L"RED4.RTTIDumper";
    aInfo->author = L"WopsS";
    aInfo->version = RED4EXT_SEMVER(1, 0, 0);
    aInfo->runtime = RED4EXT_RUNTIME_LATEST;
    aInfo->sdk = RED4EXT_SDK_LATEST;
}

RED4EXT_C_EXPORT uint32_t RED4EXT_CALL Supports()
{
    return RED4EXT_API_VERSION_LATEST;
}
