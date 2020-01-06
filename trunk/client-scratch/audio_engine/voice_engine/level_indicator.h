
#ifndef VOIP_AUDIO_ENGINE_LEVEL_INDICATOR_H
#define VOIP_AUDIO_ENGINE_LEVEL_INDICATOR_H

#include "audio_engine/include/typedefs.h"
#include "audio_engine/voice_engine/audio_engine_defines.h"

namespace VoIP {

class AudioFrame;
class CriticalSectionWrapper;
namespace aoe {

class AudioLevel
{
public:
    AudioLevel();
    virtual ~AudioLevel();

    // Called on "API thread(s)" from APIs like AoEBase::CreateChannel(),
    // AoEBase::StopSend(), AoEVolumeControl::GetSpeechOutputLevel().
    int8_t Level() const;
    int16_t LevelFullRange() const;
    void Clear();

    // Called on a native capture audio thread (platform dependent) from the
    // AudioTransport::RecordedDataIsAvailable() callback.
    // In Chrome, this method is called on the AudioInputDevice thread.
    void ComputeLevel(const AudioFrame& audioFrame);

private:
    enum { kUpdateFrequency = 10};

    CriticalSectionWrapper& _critSect;

    int16_t _absMax;
    int16_t _count;
    int8_t _currentLevel;
    int16_t _currentLevelFullRange;
};

}  // namespace aoe

}  // namespace VoIP

#endif // VOIP_AUDIO_ENGINE_LEVEL_INDICATOR_H
