#ifndef VOIP_MODULES_AUDIO_CONFERENCE_MIXER_SOURCE_LEVEL_INDICATOR_H_
#define VOIP_MODULES_AUDIO_CONFERENCE_MIXER_SOURCE_LEVEL_INDICATOR_H_

#include "audio_engine/include/typedefs.h"

namespace VoIP {
class LevelIndicator
{
public:
    enum{TICKS_BEFORE_CALCULATION = 10};

    LevelIndicator();
    ~LevelIndicator();

    // Updates the level.
    void ComputeLevel(const int16_t* speech,
                      const uint16_t nrOfSamples);

    int32_t GetLevel();
private:
    int32_t  _max;
    uint32_t _count;
    uint32_t _currentLevel;
};
}  // namespace VoIP

#endif // VOIP_MODULES_AUDIO_CONFERENCE_MIXER_SOURCE_LEVEL_INDICATOR_H_
