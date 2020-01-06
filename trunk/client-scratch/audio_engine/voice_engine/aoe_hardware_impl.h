
#ifndef VOIP_AUDIO_ENGINE_AOE_HARDWARE_IMPL_H
#define VOIP_AUDIO_ENGINE_AOE_HARDWARE_IMPL_H

#include "audio_engine/voice_engine/include/aoe_hardware.h"

#include "audio_engine/voice_engine/shared_data.h"

namespace VoIP
{

class AoEHardwareImpl: public AoEHardware
{
public:
    virtual int GetNumOfRecordingDevices(int& devices);

    virtual int GetNumOfPlayoutDevices(int& devices);

    virtual int GetRecordingDeviceName(int index,
                                       char strNameUTF8[128],
                                       char strGuidUTF8[128]);

    virtual int GetPlayoutDeviceName(int index,
                                     char strNameUTF8[128],
                                     char strGuidUTF8[128]);

    virtual int GetRecordingDeviceStatus(bool& isAvailable);

    virtual int GetPlayoutDeviceStatus(bool& isAvailable);

    virtual int SetRecordingDevice(
        int index,
        StereoChannel recordingChannel = kStereoBoth);

    virtual int SetPlayoutDevice(int index);

    virtual int SetAudioDeviceLayer(AudioLayers audioLayer);

    virtual int GetAudioDeviceLayer(AudioLayers& audioLayer);

    virtual int GetCPULoad(int& loadPercent);

    virtual int ResetAudioDevice();

    virtual int AudioDeviceControl(unsigned int par1,
                                   unsigned int par2,
                                   unsigned int par3);

    virtual int SetLoudspeakerStatus(bool enable);

    virtual int GetLoudspeakerStatus(bool& enabled);

    virtual int EnableBuiltInAEC(bool enable);
    virtual bool BuiltInAECIsEnabled() const;

    virtual int SetRecordingSampleRate(unsigned int samples_per_sec);
    virtual int RecordingSampleRate(unsigned int* samples_per_sec) const;
    virtual int SetPlayoutSampleRate(unsigned int samples_per_sec);
    virtual int PlayoutSampleRate(unsigned int* samples_per_sec) const;

protected:
    AoEHardwareImpl(aoe::SharedData* shared);
    virtual ~AoEHardwareImpl();

private:
    aoe::SharedData* _shared;
};

}  // namespace VoIP

#endif  // VOIP_AUDIO_ENGINE_AOE_HARDWARE_IMPL_H
