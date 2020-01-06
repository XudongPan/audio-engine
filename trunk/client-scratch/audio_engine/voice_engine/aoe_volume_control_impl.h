
#ifndef VOIP_AUDIO_ENGINE_AOE_VOLUME_CONTROL_IMPL_H
#define VOIP_AUDIO_ENGINE_AOE_VOLUME_CONTROL_IMPL_H

#include "audio_engine/voice_engine/include/aoe_volume_control.h"

#include "audio_engine/voice_engine/shared_data.h"

namespace VoIP {

class AoEVolumeControlImpl : public AoEVolumeControl
{
public:
    virtual int SetSpeakerVolume(unsigned int volume);

    virtual int GetSpeakerVolume(unsigned int& volume);

    virtual int SetSystemOutputMute(bool enable);

    virtual int GetSystemOutputMute(bool& enabled);

    virtual int SetMicVolume(unsigned int volume);

    virtual int GetMicVolume(unsigned int& volume);

    virtual int SetInputMute(int channel, bool enable);

    virtual int GetInputMute(int channel, bool& enabled);

    virtual int SetSystemInputMute(bool enable);

    virtual int GetSystemInputMute(bool& enabled);

    virtual int GetSpeechInputLevel(unsigned int& level);

    virtual int GetSpeechOutputLevel(int channel, unsigned int& level);

    virtual int GetSpeechInputLevelFullRange(unsigned int& level);

    virtual int GetSpeechOutputLevelFullRange(int channel,
                                              unsigned int& level);

    virtual int SetChannelOutputVolumeScaling(int channel, float scaling);

    virtual int GetChannelOutputVolumeScaling(int channel, float& scaling);

    virtual int SetOutputVolumePan(int channel, float left, float right);

    virtual int GetOutputVolumePan(int channel, float& left, float& right);


protected:
    AoEVolumeControlImpl(aoe::SharedData* shared);
    virtual ~AoEVolumeControlImpl();

private:
    aoe::SharedData* _shared;
};

}  // namespace VoIP

#endif    // VOIP_AUDIO_ENGINE_AOE_VOLUME_CONTROL_IMPL_H
