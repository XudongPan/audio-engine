
#include "audio_engine/common_audio/signal_processing/include/signal_processing_library.h"
#include "audio_engine/modules/interface/module_common_types.h"
#include "audio_engine/system_wrappers/interface/critical_section_wrapper.h"
#include "audio_engine/voice_engine/level_indicator.h"

namespace VoIP {

namespace aoe {

// Number of bars on the indicator.
// Note that the number of elements is specified because we are indexing it
// in the range of 0-32
const int8_t permutation[33] =
    {0,1,2,3,4,4,5,5,5,5,6,6,6,6,6,7,7,7,7,8,8,8,9,9,9,9,9,9,9,9,9,9,9};


AudioLevel::AudioLevel() :
    _critSect(*CriticalSectionWrapper::CreateCriticalSection()),
    _absMax(0),
    _count(0),
    _currentLevel(0),
    _currentLevelFullRange(0) {
}

AudioLevel::~AudioLevel() {
    delete &_critSect;
}

void AudioLevel::Clear()
{
    CriticalSectionScoped cs(&_critSect);
    _absMax = 0;
    _count = 0;
    _currentLevel = 0;
    _currentLevelFullRange = 0;
}

void AudioLevel::ComputeLevel(const AudioFrame& audioFrame)
{
    int16_t absValue(0);

    // Check speech level (works for 2 channels as well)
    absValue = VoipSpl_MaxAbsValueW16(
        audioFrame.data_,
        audioFrame.samples_per_channel_*audioFrame.num_channels_);

    // Protect member access using a lock since this method is called on a
    // dedicated audio thread in the RecordedDataIsAvailable() callback.
    CriticalSectionScoped cs(&_critSect);

    if (absValue > _absMax)
    _absMax = absValue;

    // Update level approximately 10 times per second
    if (_count++ == kUpdateFrequency)
    {
        _currentLevelFullRange = _absMax;

        _count = 0;

        // Highest value for a int16_t is 0x7fff = 32767
        // Divide with 1000 to get in the range of 0-32 which is the range of
        // the permutation vector
        int32_t position = _absMax/1000;

        // Make it less likely that the bar stays at position 0. I.e. only if
        // its in the range 0-250 (instead of 0-1000)
        if ((position == 0) && (_absMax > 250))
        {
            position = 1;
        }
        _currentLevel = permutation[position];

        // Decay the absolute maximum (divide by 4)
        _absMax >>= 2;
    }
}

int8_t AudioLevel::Level() const
{
    CriticalSectionScoped cs(&_critSect);
    return _currentLevel;
}

int16_t AudioLevel::LevelFullRange() const
{
    CriticalSectionScoped cs(&_critSect);
    return _currentLevelFullRange;
}

}  // namespace aoe

}  // namespace VoIP
