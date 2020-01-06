
#include <assert.h>
#include <stdio.h>

#include "audio_engine/voice_engine/statistics.h"

#include "audio_engine/system_wrappers/interface/critical_section_wrapper.h"
#include "audio_engine/system_wrappers/interface/trace.h"

namespace VoIP {

namespace aoe {

Statistics::Statistics(uint32_t instanceId) :
    _critPtr(CriticalSectionWrapper::CreateCriticalSection()),
    _instanceId(instanceId),
    _lastError(0),
    _isInitialized(false)
{
}
	
Statistics::~Statistics()
{
    if (_critPtr)
    {
        delete _critPtr;
        _critPtr = NULL;
    }
}

int32_t Statistics::SetInitialized()
{
    _isInitialized = true;
    return 0;
}

int32_t Statistics::SetUnInitialized()
{
    _isInitialized = false;
    return 0;
}

bool Statistics::Initialized() const
{
    return _isInitialized;
}

int32_t Statistics::SetLastError(int32_t error) const
{
    CriticalSectionScoped cs(_critPtr);
    _lastError = error;
    return 0;
}

int32_t Statistics::SetLastError(int32_t error,
                                 TraceLevel level) const
{
    CriticalSectionScoped cs(_critPtr);
    _lastError = error;
    VOIP_TRACE(level, kTraceVoice, AoEId(_instanceId,-1),
                 "error code is set to %d",
                 _lastError);
    return 0;
}

int32_t Statistics::SetLastError(
    int32_t error,
    TraceLevel level, const char* msg) const
{
    CriticalSectionScoped cs(_critPtr);
    char traceMessage[KTraceMaxMessageSize];
    assert(strlen(msg) < KTraceMaxMessageSize);
    _lastError = error;
    sprintf(traceMessage, "%s (error=%d)", msg, error);
    VOIP_TRACE(level, kTraceVoice, AoEId(_instanceId,-1), "%s",
                 traceMessage);
    return 0;
}

int32_t Statistics::LastError() const
{
    CriticalSectionScoped cs(_critPtr);
    VOIP_TRACE(kTraceStateInfo, kTraceVoice, AoEId(_instanceId,-1),
               "LastError() => %d", _lastError);
    return _lastError;
}

}  // namespace aoe

}  // namespace VoIP
