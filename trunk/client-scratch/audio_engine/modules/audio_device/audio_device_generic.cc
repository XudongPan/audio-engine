
#include "audio_engine/modules/audio_device/audio_device_generic.h"
#include "audio_engine/system_wrappers/interface/trace.h"

namespace VoIP {

int32_t AudioDeviceGeneric::SetRecordingSampleRate(
    const uint32_t samplesPerSec)
{
    VOIP_TRACE(kTraceError, kTraceAudioDevice, -1,
        "Set recording sample rate not supported on this platform");
    return -1;
}

int32_t AudioDeviceGeneric::SetPlayoutSampleRate(
    const uint32_t samplesPerSec)
{
    VOIP_TRACE(kTraceError, kTraceAudioDevice, -1,
        "Set playout sample rate not supported on this platform");
    return -1;
}

int32_t AudioDeviceGeneric::SetLoudspeakerStatus(bool enable)
{
    VOIP_TRACE(kTraceError, kTraceAudioDevice, -1,
        "Set loudspeaker status not supported on this platform");
    return -1;
}

int32_t AudioDeviceGeneric::GetLoudspeakerStatus(bool& enable) const
{
    VOIP_TRACE(kTraceError, kTraceAudioDevice, -1,
        "Get loudspeaker status not supported on this platform");
    return -1;
}

int32_t AudioDeviceGeneric::ResetAudioDevice()
{
    VOIP_TRACE(kTraceError, kTraceAudioDevice, -1,
        "Reset audio device not supported on this platform");
    return -1;
}

int32_t AudioDeviceGeneric::SoundDeviceControl(unsigned int par1,
    unsigned int par2, unsigned int par3, unsigned int par4)
{
    VOIP_TRACE(kTraceError, kTraceAudioDevice, -1,
        "Sound device control not supported on this platform");
    return -1;
}

int32_t AudioDeviceGeneric::EnableBuiltInAEC(bool enable)
{
    VOIP_TRACE(kTraceError, kTraceAudioDevice, -1,
        "Windows AEC not supported on this platform");
    return -1;
}

bool AudioDeviceGeneric::BuiltInAECIsEnabled() const
{
    VOIP_TRACE(kTraceError, kTraceAudioDevice, -1,
        "Windows AEC not supported on this platform");
    return false;
}

}  // namespace VoIP
