#ifndef VOIP_AUDIO_DEVICE_AUDIO_DEVICE_UTILITY_H
#define VOIP_AUDIO_DEVICE_AUDIO_DEVICE_UTILITY_H

#include "audio_engine/include/typedefs.h"

namespace VoIP
{

class AudioDeviceUtility
{
public:
    static uint32_t GetTimeInMS();
	static void WaitForKey();
    static bool StringCompare(const char* str1,
                              const char* str2,
                              const uint32_t length);
	virtual int32_t Init() = 0;

	virtual ~AudioDeviceUtility() {}
};

}  // namespace VoIP

#endif  // VOIP_AUDIO_DEVICE_AUDIO_DEVICE_UTILITY_H
