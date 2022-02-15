#include "stdafx.hpp"

#include <RED4ext/Dump/Reflection.hpp>

#include "Dumper.hpp"
#include "SuspendThreads.hpp"
#include "Writers/JsonWriter.hpp"
#include "Writers/TextWriter.hpp"
#include "Writers/WolvenKitWriter.hpp"

RED4EXT_C_EXPORT bool RED4EXT_CALL Main(RED4ext::PluginHandle aHandle, RED4ext::EMainReason aReason,
                                        const RED4ext::Sdk* aSdk)
{
    switch (aReason)
    {
    case RED4ext::EMainReason::Load:
    {
        RED4ext::GameState state;
        state.OnEnter = nullptr;
        state.OnUpdate = [](RED4ext::CGameApplication* aApp) {
            // Suspend other threads, if this process will take long (> 120 seconds) watchdog will bitch and close the
            // process. Since the watchdog run on a different thread, this will do the trick.
            SuspendThreads _;

            auto dumpsDir = std::filesystem::current_path() / L"dumps";

            std::vector<std::shared_ptr<IWriter>> writers;
            writers.emplace_back(new TextWriter(dumpsDir));
            writers.emplace_back(new JsonWriter(dumpsDir, true));
            writers.emplace_back(new WolvenKitWriter(dumpsDir));

            Dumper dumper;
            for (auto writer : writers)
            {
                dumper.Run(writer);
            }

            auto path = dumpsDir / L"cpp";
            RED4ext::GameReflection::Dump(path);

            return true;
        };
        state.OnExit = nullptr;

        aSdk->gameStates->Add(aHandle, RED4ext::EGameStateType::Running, &state);

        break;
    }
    case RED4ext::EMainReason::Unload:
    {
        break;
    }
    }

    return true;
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
