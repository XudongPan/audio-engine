
#include "audio_engine/voice_engine/aoe_volume_control_impl.h"

#include "audio_engine/system_wrappers/interface/critical_section_wrapper.h"
#include "audio_engine/system_wrappers/interface/trace.h"
#include "audio_engine/voice_engine/channel.h"
#include "audio_engine/voice_engine/include/aoe_errors.h"
#include "audio_engine/voice_engine/output_mixer.h"
#include "audio_engine/voice_engine/transmit_mixer.h"
#include "audio_engine/voice_engine/audio_engine_impl.h"

namespace VoIP {

AoEVolumeControl* AoEVolumeControl::GetInterface(AudioEngine* audioEngine)
{
#ifndef VOIP_AUDIO_ENGINE_VOLUME_CONTROL_API
    return NULL;
#else
    if (NULL == audioEngine)
    {
        return NULL;
    }
    AudioEngineImpl* s = static_cast<AudioEngineImpl*>(audioEngine);
    s->AddRef();
    return s;
#endif
}

#ifdef VOIP_AUDIO_ENGINE_VOLUME_CONTROL_API

AoEVolumeControlImpl::AoEVolumeControlImpl(aoe::SharedData* shared)
    : _shared(shared)
{
    VOIP_TRACE(kTraceMemory, kTraceVoice, AoEId(_shared->instance_id(), -1),
               "AoEVolumeControlImpl::AoEVolumeControlImpl() - ctor");
}

AoEVolumeControlImpl::~AoEVolumeControlImpl()
{
    VOIP_TRACE(kTraceMemory, kTraceVoice, AoEId(_shared->instance_id(), -1),
               "AoEVolumeControlImpl::~AoEVolumeControlImpl() - dtor");
}

int AoEVolumeControlImpl::SetSpeakerVolume(unsigned int volume)
{
    VOIP_TRACE(kTraceApiCall, kTraceVoice, AoEId(_shared->instance_id(), -1),
               "SetSpeakerVolume(volume=%u)", volume);

    if (!_shared->statistics().Initialized())
    {
        _shared->SetLastError(VE_NOT_INITED, kTraceError);
        return -1;
    }
    if (volume > kMaxVolumeLevel)
    {
        _shared->SetLastError(VE_INVALID_ARGUMENT, kTraceError,
            "SetSpeakerVolume() invalid argument");
        return -1;
    }

    uint32_t maxVol(0);
    uint32_t spkrVol(0);

    // scale: [0,kMaxVolumeLevel] -> [0,MaxSpeakerVolume]
    if (_shared->audio_device()->MaxSpeakerVolume(&maxVol) != 0)
    {
        _shared->SetLastError(VE_MIC_VOL_ERROR, kTraceError,
            "SetSpeakerVolume() failed to get max volume");
        return -1;
    }
    // Round the value and avoid floating computation.
    spkrVol = (uint32_t)((volume * maxVol +
        (int)(kMaxVolumeLevel / 2)) / (kMaxVolumeLevel));

    // set the actual volume using the audio mixer
    if (_shared->audio_device()->SetSpeakerVolume(spkrVol) != 0)
    {
        _shared->SetLastError(VE_MIC_VOL_ERROR, kTraceError,
            "SetSpeakerVolume() failed to set speaker volume");
        return -1;
    }
    return 0;
}

int AoEVolumeControlImpl::GetSpeakerVolume(unsigned int& volume)
{
    VOIP_TRACE(kTraceApiCall, kTraceVoice, AoEId(_shared->instance_id(), -1),
               "GetSpeakerVolume()");

    if (!_shared->statistics().Initialized())
    {
        _shared->SetLastError(VE_NOT_INITED, kTraceError);
        return -1;
    }

    uint32_t spkrVol(0);
    uint32_t maxVol(0);

    if (_shared->audio_device()->SpeakerVolume(&spkrVol) != 0)
    {
        _shared->SetLastError(VE_GET_MIC_VOL_ERROR, kTraceError,
            "GetSpeakerVolume() unable to get speaker volume");
        return -1;
    }

    // scale: [0, MaxSpeakerVolume] -> [0, kMaxVolumeLevel]
    if (_shared->audio_device()->MaxSpeakerVolume(&maxVol) != 0)
    {
        _shared->SetLastError(VE_GET_MIC_VOL_ERROR, kTraceError,
            "GetSpeakerVolume() unable to get max speaker volume");
        return -1;
    }
    // Round the value and avoid floating computation.
    volume = (uint32_t) ((spkrVol * kMaxVolumeLevel +
        (int)(maxVol / 2)) / (maxVol));

    VOIP_TRACE(kTraceStateInfo, kTraceVoice,
        AoEId(_shared->instance_id(), -1),
        "GetSpeakerVolume() => volume=%d", volume);
    return 0;
}

int AoEVolumeControlImpl::SetSystemOutputMute(bool enable)
{
    VOIP_TRACE(kTraceApiCall, kTraceVoice, AoEId(_shared->instance_id(), -1),
               "GetSystemOutputMute(enabled=%d)", enable);

    if (!_shared->statistics().Initialized())
    {
        _shared->SetLastError(VE_NOT_INITED, kTraceError);
        return -1;
    }

    if (_shared->audio_device()->SetSpeakerMute(enable) != 0)
    {
        _shared->SetLastError(VE_GET_MIC_VOL_ERROR, kTraceError,
            "SpeakerMute() unable to Set speaker mute");
        return -1;
    }

    return 0;
}

int AoEVolumeControlImpl::GetSystemOutputMute(bool& enabled)
{
    VOIP_TRACE(kTraceApiCall, kTraceVoice, AoEId(_shared->instance_id(), -1),
               "GetSystemOutputMute(enabled=?)");

    if (!_shared->statistics().Initialized())
    {
        _shared->SetLastError(VE_NOT_INITED, kTraceError);
        return -1;
    }

    if (_shared->audio_device()->SpeakerMute(&enabled) != 0)
    {
        _shared->SetLastError(VE_GET_MIC_VOL_ERROR, kTraceError,
            "SpeakerMute() unable to get speaker mute state");
        return -1;
    }
    VOIP_TRACE(kTraceStateInfo, kTraceVoice,
        AoEId(_shared->instance_id(), -1),
        "GetSystemOutputMute() => %d", enabled);
    return 0;
}

int AoEVolumeControlImpl::SetMicVolume(unsigned int volume)
{
    VOIP_TRACE(kTraceApiCall, kTraceVoice, AoEId(_shared->instance_id(), -1),
               "SetMicVolume(volume=%u)", volume);

    if (!_shared->statistics().Initialized())
    {
        _shared->SetLastError(VE_NOT_INITED, kTraceError);
        return -1;
    }
    if (volume > kMaxVolumeLevel)
    {
        _shared->SetLastError(VE_INVALID_ARGUMENT, kTraceError,
            "SetMicVolume() invalid argument");
        return -1;
    }

    uint32_t maxVol(0);
    uint32_t micVol(0);

    // scale: [0, kMaxVolumeLevel] -> [0,MaxMicrophoneVolume]
    if (_shared->audio_device()->MaxMicrophoneVolume(&maxVol) != 0)
    {
        _shared->SetLastError(VE_MIC_VOL_ERROR, kTraceError,
            "SetMicVolume() failed to get max volume");
        return -1;
    }

    if (volume == kMaxVolumeLevel) {
      // On Linux running pulse, users are able to set the volume above 100%
      // through the volume control panel, where the +100% range is digital
      // scaling. VoIP does not support setting the volume above 100%, and
      // simply ignores changing the volume if the user tries to set it to
      // |kMaxVolumeLevel| while the current volume is higher than |maxVol|.
      if (_shared->audio_device()->MicrophoneVolume(&micVol) != 0) {
        _shared->SetLastError(VE_GET_MIC_VOL_ERROR, kTraceError,
            "SetMicVolume() unable to get microphone volume");
        return -1;
      }
      if (micVol >= maxVol)
        return 0;
    }

    // Round the value and avoid floating point computation.
    micVol = (uint32_t) ((volume * maxVol +
        (int)(kMaxVolumeLevel / 2)) / (kMaxVolumeLevel));

    // set the actual volume using the audio mixer
    if (_shared->audio_device()->SetMicrophoneVolume(micVol) != 0)
    {
        _shared->SetLastError(VE_MIC_VOL_ERROR, kTraceError,
            "SetMicVolume() failed to set mic volume");
        return -1;
    }
    return 0;
}

int AoEVolumeControlImpl::GetMicVolume(unsigned int& volume)
{
    VOIP_TRACE(kTraceApiCall, kTraceVoice, AoEId(_shared->instance_id(), -1),
               "GetMicVolume()");

    if (!_shared->statistics().Initialized())
    {
        _shared->SetLastError(VE_NOT_INITED, kTraceError);
        return -1;
    }

    uint32_t micVol(0);
    uint32_t maxVol(0);

    if (_shared->audio_device()->MicrophoneVolume(&micVol) != 0)
    {
        _shared->SetLastError(VE_GET_MIC_VOL_ERROR, kTraceError,
            "GetMicVolume() unable to get microphone volume");
        return -1;
    }

    // scale: [0, MaxMicrophoneVolume] -> [0, kMaxVolumeLevel]
    if (_shared->audio_device()->MaxMicrophoneVolume(&maxVol) != 0)
    {
        _shared->SetLastError(VE_GET_MIC_VOL_ERROR, kTraceError,
            "GetMicVolume() unable to get max microphone volume");
        return -1;
    }
    if (micVol < maxVol) {
      // Round the value and avoid floating point calculation.
      volume = (uint32_t) ((micVol * kMaxVolumeLevel +
          (int)(maxVol / 2)) / (maxVol));
    } else {
      // Truncate the value to the kMaxVolumeLevel.
      volume = kMaxVolumeLevel;
    }

    VOIP_TRACE(kTraceStateInfo, kTraceVoice,
        AoEId(_shared->instance_id(), -1),
        "GetMicVolume() => volume=%d", volume);
    return 0;
}

int AoEVolumeControlImpl::SetInputMute(int channel, bool enable)
{
    VOIP_TRACE(kTraceApiCall, kTraceVoice, AoEId(_shared->instance_id(), -1),
               "SetInputMute(channel=%d, enable=%d)", channel, enable);

    if (!_shared->statistics().Initialized())
    {
        _shared->SetLastError(VE_NOT_INITED, kTraceError);
        return -1;
    }
    if (channel == -1)
    {
        // Mute before demultiplexing <=> affects all channels
        return _shared->transmit_mixer()->SetMute(enable);
    }
    // Mute after demultiplexing <=> affects one channel only
    aoe::ChannelOwner ch = _shared->channel_manager().GetChannel(channel);
    aoe::Channel* channelPtr = ch.channel();
    if (channelPtr == NULL)
    {
        _shared->SetLastError(VE_CHANNEL_NOT_VALID, kTraceError,
            "SetInputMute() failed to locate channel");
        return -1;
    }
    return channelPtr->SetMute(enable);
}

int AoEVolumeControlImpl::GetInputMute(int channel, bool& enabled)
{
    VOIP_TRACE(kTraceApiCall, kTraceVoice, AoEId(_shared->instance_id(), -1),
               "GetInputMute(channel=%d)", channel);

    if (!_shared->statistics().Initialized())
    {
        _shared->SetLastError(VE_NOT_INITED, kTraceError);
        return -1;
    }
    if (channel == -1)
    {
        enabled = _shared->transmit_mixer()->Mute();
    }
    else
    {
        aoe::ChannelOwner ch = _shared->channel_manager().GetChannel(channel);
        aoe::Channel* channelPtr = ch.channel();
        if (channelPtr == NULL)
        {
            _shared->SetLastError(VE_CHANNEL_NOT_VALID, kTraceError,
                "SetInputMute() failed to locate channel");
            return -1;
        }
        enabled = channelPtr->Mute();
    }
    VOIP_TRACE(kTraceStateInfo, kTraceVoice,
        AoEId(_shared->instance_id(), -1),
        "GetInputMute() => enabled = %d", (int)enabled);
    return 0;
}

int AoEVolumeControlImpl::SetSystemInputMute(bool enable)
{
    VOIP_TRACE(kTraceApiCall, kTraceVoice, AoEId(_shared->instance_id(), -1),
               "SetSystemInputMute(enabled=%d)", enable);

    if (!_shared->statistics().Initialized())
    {
            _shared->SetLastError(VE_NOT_INITED, kTraceError);
            return -1;
    }

    if (_shared->audio_device()->SetMicrophoneMute(enable) != 0)
    {
        _shared->SetLastError(VE_GET_MIC_VOL_ERROR, kTraceError,
            "MicrophoneMute() unable to set microphone mute state");
        return -1;
    }

    return 0;
}

int AoEVolumeControlImpl::GetSystemInputMute(bool& enabled)
{
    VOIP_TRACE(kTraceApiCall, kTraceVoice, AoEId(_shared->instance_id(), -1),
               "GetSystemInputMute(enabled=?)");

    if (!_shared->statistics().Initialized())
    {
        _shared->SetLastError(VE_NOT_INITED, kTraceError);
        return -1;
    }

    if (_shared->audio_device()->MicrophoneMute(&enabled) != 0)
    {
        _shared->SetLastError(VE_GET_MIC_VOL_ERROR, kTraceError,
            "MicrophoneMute() unable to get microphone mute state");
        return -1;
    }
    VOIP_TRACE(kTraceStateInfo, kTraceVoice,
        AoEId(_shared->instance_id(), -1),
        "GetSystemInputMute() => %d", enabled);
	return 0;
}

int AoEVolumeControlImpl::GetSpeechInputLevel(unsigned int& level)
{
    VOIP_TRACE(kTraceApiCall, kTraceVoice, AoEId(_shared->instance_id(), -1),
               "GetSpeechInputLevel()");

    if (!_shared->statistics().Initialized())
    {
        _shared->SetLastError(VE_NOT_INITED, kTraceError);
        return -1;
    }
    int8_t currentLevel = _shared->transmit_mixer()->AudioLevel();
    level = static_cast<unsigned int> (currentLevel);
    VOIP_TRACE(kTraceStateInfo, kTraceVoice,
        AoEId(_shared->instance_id(), -1),
        "GetSpeechInputLevel() => %d", level);
    return 0;
}

int AoEVolumeControlImpl::GetSpeechOutputLevel(int channel,
                                               unsigned int& level)
{
    VOIP_TRACE(kTraceApiCall, kTraceVoice, AoEId(_shared->instance_id(), -1),
               "GetSpeechOutputLevel(channel=%d, level=?)", channel);
	
    if (!_shared->statistics().Initialized())
    {
        _shared->SetLastError(VE_NOT_INITED, kTraceError);
        return -1;
    }
    if (channel == -1)
    {
        return _shared->output_mixer()->GetSpeechOutputLevel(
            (uint32_t&)level);
    }
    else
    {
        aoe::ChannelOwner ch = _shared->channel_manager().GetChannel(channel);
        aoe::Channel* channelPtr = ch.channel();
        if (channelPtr == NULL)
        {
            _shared->SetLastError(VE_CHANNEL_NOT_VALID, kTraceError,
                "GetSpeechOutputLevel() failed to locate channel");
            return -1;
        }
        channelPtr->GetSpeechOutputLevel((uint32_t&)level);
    }
    return 0;
}

int AoEVolumeControlImpl::GetSpeechInputLevelFullRange(unsigned int& level)
{
    VOIP_TRACE(kTraceApiCall, kTraceVoice, AoEId(_shared->instance_id(), -1),
               "GetSpeechInputLevelFullRange(level=?)");

    if (!_shared->statistics().Initialized())
    {
        _shared->SetLastError(VE_NOT_INITED, kTraceError);
        return -1;
    }
    int16_t currentLevel = _shared->transmit_mixer()->
        AudioLevelFullRange();
    level = static_cast<unsigned int> (currentLevel);
    VOIP_TRACE(kTraceStateInfo, kTraceVoice,
        AoEId(_shared->instance_id(), -1),
        "GetSpeechInputLevelFullRange() => %d", level);
    return 0;
}

int AoEVolumeControlImpl::GetSpeechOutputLevelFullRange(int channel,
                                                        unsigned int& level)
{
    VOIP_TRACE(kTraceApiCall, kTraceVoice, AoEId(_shared->instance_id(), -1),
               "GetSpeechOutputLevelFullRange(channel=%d, level=?)", channel);

    if (!_shared->statistics().Initialized())
    {
        _shared->SetLastError(VE_NOT_INITED, kTraceError);
        return -1;
    }
    if (channel == -1)
    {
        return _shared->output_mixer()->GetSpeechOutputLevelFullRange(
            (uint32_t&)level);
    }
    else
    {
        aoe::ChannelOwner ch = _shared->channel_manager().GetChannel(channel);
        aoe::Channel* channelPtr = ch.channel();
        if (channelPtr == NULL)
        {
            _shared->SetLastError(VE_CHANNEL_NOT_VALID, kTraceError,
                "GetSpeechOutputLevelFullRange() failed to locate channel");
            return -1;
        }
        channelPtr->GetSpeechOutputLevelFullRange((uint32_t&)level);
    }
    return 0;
}

int AoEVolumeControlImpl::SetChannelOutputVolumeScaling(int channel,
                                                        float scaling)
{
    VOIP_TRACE(kTraceApiCall, kTraceVoice, AoEId(_shared->instance_id(), -1),
               "SetChannelOutputVolumeScaling(channel=%d, scaling=%3.2f)",
               channel, scaling);
    if (!_shared->statistics().Initialized())
    {
        _shared->SetLastError(VE_NOT_INITED, kTraceError);
        return -1;
    }
    if (scaling < kMinOutputVolumeScaling ||
        scaling > kMaxOutputVolumeScaling)
    {
        _shared->SetLastError(VE_INVALID_ARGUMENT, kTraceError,
            "SetChannelOutputVolumeScaling() invalid parameter");
        return -1;
    }
    aoe::ChannelOwner ch = _shared->channel_manager().GetChannel(channel);
    aoe::Channel* channelPtr = ch.channel();
    if (channelPtr == NULL)
    {
        _shared->SetLastError(VE_CHANNEL_NOT_VALID, kTraceError,
            "SetChannelOutputVolumeScaling() failed to locate channel");
        return -1;
    }
    return channelPtr->SetChannelOutputVolumeScaling(scaling);
}

int AoEVolumeControlImpl::GetChannelOutputVolumeScaling(int channel,
                                                        float& scaling)
{
    VOIP_TRACE(kTraceApiCall, kTraceVoice, AoEId(_shared->instance_id(), -1),
               "GetChannelOutputVolumeScaling(channel=%d, scaling=?)", channel);
    if (!_shared->statistics().Initialized())
    {
        _shared->SetLastError(VE_NOT_INITED, kTraceError);
        return -1;
    }
    aoe::ChannelOwner ch = _shared->channel_manager().GetChannel(channel);
    aoe::Channel* channelPtr = ch.channel();
    if (channelPtr == NULL)
    {
        _shared->SetLastError(VE_CHANNEL_NOT_VALID, kTraceError,
            "GetChannelOutputVolumeScaling() failed to locate channel");
        return -1;
    }
    return channelPtr->GetChannelOutputVolumeScaling(scaling);
}

int AoEVolumeControlImpl::SetOutputVolumePan(int channel,
                                             float left,
                                             float right)
{
    VOIP_TRACE(kTraceApiCall, kTraceVoice, AoEId(_shared->instance_id(), -1),
               "SetOutputVolumePan(channel=%d, left=%2.1f, right=%2.1f)",
               channel, left, right);

    if (!_shared->statistics().Initialized())
    {
        _shared->SetLastError(VE_NOT_INITED, kTraceError);
        return -1;
    }

    bool available(false);
    _shared->audio_device()->StereoPlayoutIsAvailable(&available);
    if (!available)
    {
        _shared->SetLastError(VE_FUNC_NO_STEREO, kTraceError,
            "SetOutputVolumePan() stereo playout not supported");
        return -1;
    }
    if ((left < kMinOutputVolumePanning)  ||
        (left > kMaxOutputVolumePanning)  ||
        (right < kMinOutputVolumePanning) ||
        (right > kMaxOutputVolumePanning))
    {
        _shared->SetLastError(VE_INVALID_ARGUMENT, kTraceError,
            "SetOutputVolumePan() invalid parameter");
        return -1;
    }

    if (channel == -1)
    {
        // Master balance (affectes the signal after output mixing)
        return _shared->output_mixer()->SetOutputVolumePan(left, right);
    }
    // Per-channel balance (affects the signal before output mixing)
    aoe::ChannelOwner ch = _shared->channel_manager().GetChannel(channel);
    aoe::Channel* channelPtr = ch.channel();
    if (channelPtr == NULL)
    {
        _shared->SetLastError(VE_CHANNEL_NOT_VALID, kTraceError,
            "SetOutputVolumePan() failed to locate channel");
        return -1;
    }
    return channelPtr->SetOutputVolumePan(left, right);
}

int AoEVolumeControlImpl::GetOutputVolumePan(int channel,
                                             float& left,
                                             float& right)
{
    VOIP_TRACE(kTraceApiCall, kTraceVoice, AoEId(_shared->instance_id(), -1),
               "GetOutputVolumePan(channel=%d, left=?, right=?)", channel);

    if (!_shared->statistics().Initialized())
    {
        _shared->SetLastError(VE_NOT_INITED, kTraceError);
        return -1;
    }

    bool available(false);
    _shared->audio_device()->StereoPlayoutIsAvailable(&available);
    if (!available)
    {
        _shared->SetLastError(VE_FUNC_NO_STEREO, kTraceError,
            "GetOutputVolumePan() stereo playout not supported");
        return -1;
    }

    if (channel == -1)
    {
        return _shared->output_mixer()->GetOutputVolumePan(left, right);
    }
    aoe::ChannelOwner ch = _shared->channel_manager().GetChannel(channel);
    aoe::Channel* channelPtr = ch.channel();
    if (channelPtr == NULL)
    {
        _shared->SetLastError(VE_CHANNEL_NOT_VALID, kTraceError,
            "GetOutputVolumePan() failed to locate channel");
        return -1;
    }
    return channelPtr->GetOutputVolumePan(left, right);
}

#endif  // #ifdef VOIP_AUDIO_ENGINE_VOLUME_CONTROL_API

}  // namespace VoIP
