#ifndef VOIP_SYSTEM_WRAPPERS_SOURCE_THREAD_WINDOWS_SET_NAME_H_
#define VOIP_SYSTEM_WRAPPERS_SOURCE_THREAD_WINDOWS_SET_NAME_H_

namespace VoIP {

struct THREADNAME_INFO
{
   DWORD dwType;     // must be 0x1000
   LPCSTR szName;    // pointer to name (in user addr space)
   DWORD dwThreadID; // thread ID (-1 = caller thread)
   DWORD dwFlags;    // reserved for future use, must be zero
};

void SetThreadName(DWORD dwThreadID, LPCSTR szThreadName)
{
    THREADNAME_INFO info;
    info.dwType = 0x1000;
    info.szName = szThreadName;
    info.dwThreadID = dwThreadID;
    info.dwFlags = 0;

    __try
    {
        RaiseException(0x406D1388, 0, sizeof(info) / sizeof(DWORD),
                       (ULONG_PTR*)&info);
    }
    __except (EXCEPTION_CONTINUE_EXECUTION)
    {
    }
}
}  // namespace VoIP
#endif // VOIP_SYSTEM_WRAPPERS_SOURCE_THREAD_WINDOWS_SET_NAME_H_
