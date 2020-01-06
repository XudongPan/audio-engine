#ifndef VOIP_AUDIO_ENGINE_MONITOR_MODULE_H
#define VOIP_AUDIO_ENGINE_MONITOR_MODULE_H

#include "audio_engine/modules/interface/module.h"
#include "audio_engine/include/typedefs.h"
#include "audio_engine/voice_engine/audio_engine_defines.h"

class MonitorObserver
{
public:
    virtual void OnPeriodicProcess() = 0;
protected:
    virtual ~MonitorObserver() {}
};


namespace VoIP {
class CriticalSectionWrapper;

namespace aoe {

class MonitorModule : public Module
{
public:
    int32_t RegisterObserver(MonitorObserver& observer);

    int32_t DeRegisterObserver();

    MonitorModule();

    virtual ~MonitorModule();
public:	// module
    int32_t Version(char* version,
                    uint32_t& remainingBufferInBytes,
                    uint32_t& position) const;

    int32_t ChangeUniqueId(int32_t id);

    int32_t TimeUntilNextProcess();

    int32_t Process();
private:
    enum { kAverageProcessUpdateTimeMs = 1000 };
    MonitorObserver* _observerPtr;
    CriticalSectionWrapper&	_callbackCritSect;
    int32_t _lastProcessTime;
};

}  // namespace aoe

}  // namespace VoIP

#endif // AUDIO_ENGINE_MONITOR_MODULE
