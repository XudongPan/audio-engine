#ifndef VOIP_AUDIO_DEVICE_AUDIO_DEVICE_UTILITY_WIN_H
#define VOIP_AUDIO_DEVICE_AUDIO_DEVICE_UTILITY_WIN_H

#include "audio_engine/modules/audio_device/audio_device_utility.h"
#include "audio_engine/modules/audio_device/include/audio_device.h"
#include <windows.h>

namespace VoIP
{
class CriticalSectionWrapper;

class AudioDeviceUtilityWindows : public AudioDeviceUtility
{
public:
    AudioDeviceUtilityWindows(const int32_t id);
    ~AudioDeviceUtilityWindows();

    virtual int32_t Init();

private:
    BOOL GetOSDisplayString(LPTSTR pszOS);

private:
    CriticalSectionWrapper&         _critSect;
    int32_t                         _id;
    AudioDeviceModule::ErrorCode    _lastError;
};

}  // namespace VoIP

#endif  // VOIP_AUDIO_DEVICE_AUDIO_DEVICE_UTILITY_WIN_H
