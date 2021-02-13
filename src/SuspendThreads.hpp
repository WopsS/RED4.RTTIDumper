#pragma once

class SuspendThreads
{
public:
    SuspendThreads();
    SuspendThreads(SuspendThreads&) = delete;
    SuspendThreads(SuspendThreads&&) = delete;

    ~SuspendThreads();

    SuspendThreads& operator=(const SuspendThreads&) = delete;
    SuspendThreads& operator=(SuspendThreads&&) = delete;

private:
    void Suspend();
    void Resume();

    std::vector<HANDLE> m_handles;
};
