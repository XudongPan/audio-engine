// This sub-API supports the following functionalities:
//
//  - Speaker volume controls.
//  - Microphone volume control.
//  - Non-linear speech level control.
//  - Mute functions.
//  - Additional stereo scaling methods.
//
// Usage example, omitting error checking:
//
//  using namespace VoIP;
//  AudioEngine* aoe = AudioEngine::Create();
//  AoEBase* base = AoEBase::GetInterface(aoe);
//  AoEVolumeControl* volume  = AoEVolumeControl::GetInterface(aoe);
//  base->Init();
//  int ch = base->CreateChannel();
//  ...
//  volume->SetInputMute(ch, true);
//  ...
//  base->DeleteChannel(ch);
//  base->Terminate();
//  base->Release();
//  volume->Release();
//  AudioEngine::Delete(aoe);
//
#ifndef VOIP_AUDIO_ENGINE_VOLUME_CONTROL_H
#define VOIP_AUDIO_ENGINE_VOLUME_CONTROL_H

#include "audio_engine/include/common_types.h"

namespace VoIP {

class AudioEngine;

class VOIP_DLLEXPORT AoEVolumeControl
{
public:
    // Factory for the AoEVolumeControl sub-API. Increases an internal
    // reference counter if successful. Returns NULL if the API is not
    // supported or if construction fails.
    static AoEVolumeControl* GetInterface(AudioEngine* AudioEngine);

    // Releases the AoEVolumeControl sub-API and decreases an internal
    // reference counter. Returns the new reference count. This value should
    // be zero for all sub-API:s before the AudioEngine object can be safely
    // deleted.
    virtual int Release() = 0;

    // Sets the speaker |volume| level. Valid range is [0,255].
    virtual int SetSpeakerVolume(unsigned int volume) = 0;

    // Gets the speaker |volume| level.
    virtual int GetSpeakerVolume(unsigned int& volume) = 0;

    // Mutes the speaker device completely in the operating system.
    virtual int SetSystemOutputMute(bool enable) = 0;

    // Gets the output device mute state in the operating system.
    virtual int GetSystemOutputMute(bool &enabled) = 0;

    // Sets the microphone volume level. Valid range is [0,255].
    virtual int SetMicVolume(unsigned int volume) = 0;

    // Gets the microphone volume level.
    virtual int GetMicVolume(unsigned int& volume) = 0;

    // Mutes the microphone input signal completely without affecting
    // the audio device volume.
    virtual int SetInputMute(int channel, bool enable) = 0;

    // Gets the current microphone input mute state.
    virtual int GetInputMute(int channel, bool& enabled) = 0;

    // Mutes the microphone device completely in the operating system.
    virtual int SetSystemInputMute(bool enable) = 0;

    // Gets the mute state of the input device in the operating system.
    virtual int GetSystemInputMute(bool& enabled) = 0;

    // Sets a volume |scaling| applied to the outgoing signal of a specific
    // channel. Valid scale range is [0.0, 10.0].
    virtual int SetChannelOutputVolumeScaling(int channel, float scaling) = 0;

    // Gets the current volume scaling for a specified |channel|.
    virtual int GetChannelOutputVolumeScaling(int channel, float& scaling) = 0;

    // Scales volume of the |left| and |right| channels independently.
    // Valid scale range is [0.0, 1.0].
    virtual int SetOutputVolumePan(int channel, float left, float right) = 0;

    // Gets the current left and right scaling factors.
    virtual int GetOutputVolumePan(int channel, float& left, float& right) = 0;

protected:
    AoEVolumeControl() {};
    virtual ~AoEVolumeControl() {};
};

}  // namespace VoIP

#endif  // #ifndef VOIP_AUDIO_ENGINE_VOLUME_CONTROL_H
