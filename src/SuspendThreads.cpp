#include "stdafx.hpp"
#include "SuspendThreads.hpp"

SuspendThreads::SuspendThreads()
{
    Suspend();
}

SuspendThreads::~SuspendThreads()
{
    Resume();
}

void SuspendThreads::Suspend()
{
    auto snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
    if (snapshot == INVALID_HANDLE_VALUE)
    {
        return;
    }

    THREADENTRY32 entry;
    entry.dwSize = sizeof(THREADENTRY32);

    if (!Thread32First(snapshot, &entry))
    {
        return;
    }

    auto processId = GetCurrentProcessId();
    auto threadId = GetCurrentThreadId();

    do
    {
        if (entry.th32OwnerProcessID == processId && entry.th32ThreadID != threadId)
        {
            auto handle = OpenThread(THREAD_GET_CONTEXT | THREAD_SUSPEND_RESUME, false, entry.th32ThreadID);
            if (!handle)
            {
                continue;
            }

            auto count = SuspendThread(handle);
            if (count == -1)
            {
                CloseHandle(handle);
                continue;
            }

            m_handles.emplace_back(handle);
        }
    } while (Thread32Next(snapshot, &entry));
}

void SuspendThreads::Resume()
{
    for (auto handle : m_handles)
    {
        ResumeThread(handle);
        CloseHandle(handle);
    }

    m_handles.clear();
}
