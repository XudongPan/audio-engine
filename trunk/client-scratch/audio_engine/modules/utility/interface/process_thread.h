#ifndef VOIP_MODULES_UTILITY_INTERFACE_PROCESS_THREAD_H_
#define VOIP_MODULES_UTILITY_INTERFACE_PROCESS_THREAD_H_

#include "audio_engine/include/typedefs.h"

namespace VoIP {
class Module;

class ProcessThread
{
public:
    static ProcessThread* CreateProcessThread();
    static void DestroyProcessThread(ProcessThread* module);

    virtual int32_t Start() = 0;
    virtual int32_t Stop() = 0;

    virtual int32_t RegisterModule(Module* module) = 0;
    virtual int32_t DeRegisterModule(const Module* module) = 0;
protected:
    virtual ~ProcessThread();
};
}  // namespace VoIP
#endif // VOIP_MODULES_UTILITY_INTERFACE_PROCESS_THREAD_H_
