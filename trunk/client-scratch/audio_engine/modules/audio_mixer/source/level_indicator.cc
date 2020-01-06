#include "audio_engine/modules/audio_mixer/source/level_indicator.h"

namespace VoIP {
// Array for adding smothing to level changes (ad-hoc).
const uint32_t perm[] =
    {0,1,2,3,4,4,5,5,5,5,6,6,6,6,6,7,7,7,7,8,8,8,9,9,9,9,9,9,9,9,9,9,9};

LevelIndicator::LevelIndicator()
    : _max(0),
      _count(0),
      _currentLevel(0)
{
}

LevelIndicator::~LevelIndicator()
{
}

// Level is based on the highest absolute value for all samples.
void LevelIndicator::ComputeLevel(const int16_t* speech,
                                  const uint16_t nrOfSamples)
{
    int32_t min = 0;
    for(uint32_t i = 0; i < nrOfSamples; i++)
    {
        if(_max < speech[i])
        {
            _max = speech[i];
        }
        if(min > speech[i])
        {
            min = speech[i];
        }
    }

    // Absolute max value.
    if(-min > _max)
    {
        _max = -min;
    }

    if(_count == TICKS_BEFORE_CALCULATION)
    {
        // Highest sample value maps directly to a level.
        int32_t position = _max / 1000;
        if ((position == 0) &&
            (_max > 250))
        {
            position = 1;
        }
        _currentLevel = perm[position];
        // The max value is decayed and stored so that it can be reused to slow
        // down decreases in level.
        _max = _max >> 1;
        _count = 0;
    } else {
        _count++;
    }
}

int32_t LevelIndicator::GetLevel()
{
    return _currentLevel;
}

}  // namespace VoIP
