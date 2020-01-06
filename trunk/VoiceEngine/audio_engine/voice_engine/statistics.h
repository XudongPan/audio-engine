
#ifndef VOIP_AUDIO_ENGINE_STATISTICS_H
#define VOIP_AUDIO_ENGINE_STATISTICS_H

#include "audio_engine/include/common_types.h"
#include "audio_engine/include/typedefs.h"
#include "audio_engine/voice_engine/include/aoe_errors.h"
#include "audio_engine/voice_engine/audio_engine_defines.h"

namespace VoIP {
class CriticalSectionWrapper;

namespace aoe {

class Statistics
{
 public:
    enum {KTraceMaxMessageSize = 256};
 public:
    Statistics(uint32_t instanceId);
    ~Statistics();

    int32_t SetInitialized();
    int32_t SetUnInitialized();
    bool Initialized() const;
    int32_t SetLastError(int32_t error) const;
    int32_t SetLastError(int32_t error, TraceLevel level) const;
    int32_t SetLastError(int32_t error,
                         TraceLevel level,
                         const char* msg) const;
    int32_t LastError() const;

 private:
    CriticalSectionWrapper* _critPtr;
    const uint32_t _instanceId;
    mutable int32_t _lastError;
    bool _isInitialized;
};

}  // namespace aoe

}  // namespace VoIP

#endif // VOIP_AUDIO_ENGINE_STATISTICS_H
