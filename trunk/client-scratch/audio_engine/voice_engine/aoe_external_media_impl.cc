#include "audio_engine/voice_engine/aoe_external_media_impl.h"

#include "audio_engine/system_wrappers/interface/critical_section_wrapper.h"
#include "audio_engine/system_wrappers/interface/trace.h"
#include "audio_engine/voice_engine/channel.h"
#include "audio_engine/voice_engine/include/aoe_errors.h"
#include "audio_engine/voice_engine/output_mixer.h"
#include "audio_engine/voice_engine/transmit_mixer.h"
#include "audio_engine/voice_engine/audio_engine_impl.h"

namespace VoIP {

AoEExternalMedia* AoEExternalMedia::GetInterface(AudioEngine* audioEngine)
{
#ifndef VOIP_AUDIO_ENGINE_EXTERNAL_MEDIA_API
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

#ifdef VOIP_AUDIO_ENGINE_EXTERNAL_MEDIA_API

AoEExternalMediaImpl::AoEExternalMediaImpl(aoe::SharedData* shared)
    :
#ifdef VOIP_AOE_EXTERNAL_REC_AND_PLAYOUT
    playout_delay_ms_(0),
#endif
    shared_(shared)
{
    VOIP_TRACE(kTraceMemory, kTraceVoice, AoEId(shared_->instance_id(), -1),
                 "AoEExternalMediaImpl() - ctor");
}

AoEExternalMediaImpl::~AoEExternalMediaImpl()
{
    VOIP_TRACE(kTraceMemory, kTraceVoice, AoEId(shared_->instance_id(), -1),
                 "~AoEExternalMediaImpl() - dtor");
}

int AoEExternalMediaImpl::RegisterExternalMediaProcessing(
    int channel,
    ProcessingTypes type,
    AoEMediaProcess& processObject)
{
    VOIP_TRACE(kTraceApiCall, kTraceVoice, AoEId(shared_->instance_id(), -1),
                 "RegisterExternalMediaProcessing(channel=%d, type=%d, "
                 "processObject=0x%x)", channel, type, &processObject);
    if (!shared_->statistics().Initialized())
    {
        shared_->SetLastError(VE_NOT_INITED, kTraceError);
        return -1;
    }
    switch (type)
    {
        case kPlaybackPerChannel:
        case kRecordingPerChannel:
        {
            aoe::ChannelOwner ch =
                shared_->channel_manager().GetChannel(channel);
            aoe::Channel* channelPtr = ch.channel();
            if (channelPtr == NULL)
            {
                shared_->SetLastError(VE_CHANNEL_NOT_VALID, kTraceError,
                    "RegisterExternalMediaProcessing() failed to locate "
                    "channel");
                return -1;
            }
            return channelPtr->RegisterExternalMediaProcessing(type,
                                                               processObject);
        }
        case kPlaybackAllChannelsMixed:
        {
            return shared_->output_mixer()->RegisterExternalMediaProcessing(
                processObject);
        }
        case kRecordingAllChannelsMixed:
        case kRecordingPreprocessing:
        {
            return shared_->transmit_mixer()->RegisterExternalMediaProcessing(
                &processObject, type);
        }
    }
    return -1;
}

int AoEExternalMediaImpl::DeRegisterExternalMediaProcessing(
    int channel,
    ProcessingTypes type)
{
    VOIP_TRACE(kTraceApiCall, kTraceVoice, AoEId(shared_->instance_id(), -1),
                 "DeRegisterExternalMediaProcessing(channel=%d)", channel);
    if (!shared_->statistics().Initialized())
    {
        shared_->SetLastError(VE_NOT_INITED, kTraceError);
        return -1;
    }
    switch (type)
    {
        case kPlaybackPerChannel:
        case kRecordingPerChannel:
        {
            aoe::ChannelOwner ch =
                shared_->channel_manager().GetChannel(channel);
            aoe::Channel* channelPtr = ch.channel();
            if (channelPtr == NULL)
            {
                shared_->SetLastError(VE_CHANNEL_NOT_VALID, kTraceError,
                    "RegisterExternalMediaProcessing() "
                    "failed to locate channel");
                return -1;
            }
            return channelPtr->DeRegisterExternalMediaProcessing(type);
        }
        case kPlaybackAllChannelsMixed:
        {
            return shared_->output_mixer()->
                DeRegisterExternalMediaProcessing();
        }
        case kRecordingAllChannelsMixed:
        case kRecordingPreprocessing:
        {
            return shared_->transmit_mixer()->
                DeRegisterExternalMediaProcessing(type);
        }
    }
    return -1;
}

int AoEExternalMediaImpl::SetExternalRecordingStatus(bool enable)
{
    VOIP_TRACE(kTraceApiCall, kTraceVoice, AoEId(shared_->instance_id(), -1),
                 "SetExternalRecordingStatus(enable=%d)", enable);
#ifdef VOIP_AOE_EXTERNAL_REC_AND_PLAYOUT
    if (shared_->audio_device()->Recording())
    {
        shared_->SetLastError(VE_ALREADY_SENDING, kTraceError,
            "SetExternalRecordingStatus() cannot set state while sending");
        return -1;
    }
    shared_->set_ext_recording(enable);
    return 0;
#else
    shared_->SetLastError(VE_FUNC_NOT_SUPPORTED, kTraceError,
        "SetExternalRecordingStatus() external recording is not supported");
    return -1;
#endif
}

int AoEExternalMediaImpl::ExternalRecordingInsertData(
        const int16_t speechData10ms[],
        int lengthSamples,
        int samplingFreqHz,
        int current_delay_ms)
{
    VOIP_TRACE(kTraceStream, kTraceVoice, AoEId(shared_->instance_id(), -1),
                 "ExternalRecordingInsertData(speechData10ms=0x%x,"
                 " lengthSamples=%u, samplingFreqHz=%d, current_delay_ms=%d)",
                 &speechData10ms[0], lengthSamples, samplingFreqHz,
              current_delay_ms);
#ifdef VOIP_AOE_EXTERNAL_REC_AND_PLAYOUT
    if (!shared_->statistics().Initialized())
    {
        shared_->SetLastError(VE_NOT_INITED, kTraceError);
        return -1;
    }
    if (!shared_->ext_recording())
    {
       shared_->SetLastError(VE_INVALID_OPERATION, kTraceError,
           "ExternalRecordingInsertData() external recording is not enabled");
        return -1;
    }
    if (shared_->NumOfSendingChannels() == 0)
    {
        shared_->SetLastError(VE_ALREADY_SENDING, kTraceError,
            "SetExternalRecordingStatus() no channel is sending");
        return -1;
    }
    if ((16000 != samplingFreqHz) && (32000 != samplingFreqHz) &&
        (48000 != samplingFreqHz) && (44000 != samplingFreqHz))
    {
         shared_->SetLastError(VE_INVALID_ARGUMENT, kTraceError,
             "SetExternalRecordingStatus() invalid sample rate");
        return -1;
    }
    if ((0 == lengthSamples) ||
        ((lengthSamples % (samplingFreqHz / 100)) != 0))
    {
         shared_->SetLastError(VE_INVALID_ARGUMENT, kTraceError,
             "SetExternalRecordingStatus() invalid buffer size");
        return -1;
    }
    if (current_delay_ms < 0)
    {
        shared_->SetLastError(VE_INVALID_ARGUMENT, kTraceError,
            "SetExternalRecordingStatus() invalid delay)");
        return -1;
    }

    uint16_t blockSize = samplingFreqHz / 100;
    uint32_t nBlocks = lengthSamples / blockSize;
    int16_t totalDelayMS = 0;
    uint16_t playoutDelayMS = 0;

    for (uint32_t i = 0; i < nBlocks; i++)
    {
        if (!shared_->ext_playout())
        {
            // Use real playout delay if external playout is not enabled.
            if (shared_->audio_device()->PlayoutDelay(&playoutDelayMS) != 0) {
              shared_->SetLastError(VE_AUDIO_DEVICE_MODULE_ERROR, kTraceWarning,
                  "PlayoutDelay() unable to get the playout delay");
            }
            totalDelayMS = current_delay_ms + playoutDelayMS;
        }
        else
        {
            // Use stored delay value given the last call
            // to ExternalPlayoutGetData.
            totalDelayMS = current_delay_ms + playout_delay_ms_;
            // Compensate for block sizes larger than 10ms
            totalDelayMS -= (int16_t)(i*10);
            if (totalDelayMS < 0)
                totalDelayMS = 0;
        }
        shared_->transmit_mixer()->PrepareDemux(
            (const int8_t*)(&speechData10ms[i*blockSize]),
            blockSize,
            1,
            samplingFreqHz,
            totalDelayMS,
            0,
            0,
            false); // Typing detection not supported

        shared_->transmit_mixer()->DemuxAndMix();
        shared_->transmit_mixer()->EncodeAndSend();
    }
    return 0;
#else
       shared_->SetLastError(VE_FUNC_NOT_SUPPORTED, kTraceError,
        "ExternalRecordingInsertData() external recording is not supported");
    return -1;
#endif
}

int AoEExternalMediaImpl::SetExternalPlayoutStatus(bool enable)
{
    VOIP_TRACE(kTraceApiCall, kTraceVoice, AoEId(shared_->instance_id(), -1),
                 "SetExternalPlayoutStatus(enable=%d)", enable);
#ifdef VOIP_AOE_EXTERNAL_REC_AND_PLAYOUT
    if (shared_->audio_device()->Playing())
    {
        shared_->SetLastError(VE_ALREADY_SENDING, kTraceError,
            "SetExternalPlayoutStatus() cannot set state while playing");
        return -1;
    }
    shared_->set_ext_playout(enable);
    return 0;
#else
    shared_->SetLastError(VE_FUNC_NOT_SUPPORTED, kTraceError,
        "SetExternalPlayoutStatus() external playout is not supported");
    return -1;
#endif
}

int AoEExternalMediaImpl::ExternalPlayoutGetData(
    int16_t speechData10ms[],
    int samplingFreqHz,
    int current_delay_ms,
    int& lengthSamples)
{
    VOIP_TRACE(kTraceStream, kTraceVoice, AoEId(shared_->instance_id(), -1),
                 "ExternalPlayoutGetData(speechData10ms=0x%x, samplingFreqHz=%d"
                 ",  current_delay_ms=%d)", &speechData10ms[0], samplingFreqHz,
                 current_delay_ms);
#ifdef VOIP_AOE_EXTERNAL_REC_AND_PLAYOUT
    if (!shared_->statistics().Initialized())
    {
        shared_->SetLastError(VE_NOT_INITED, kTraceError);
        return -1;
    }
    if (!shared_->ext_playout())
    {
       shared_->SetLastError(VE_INVALID_OPERATION, kTraceError,
           "ExternalPlayoutGetData() external playout is not enabled");
        return -1;
    }
    if ((16000 != samplingFreqHz) && (32000 != samplingFreqHz) &&
        (48000 != samplingFreqHz) && (44000 != samplingFreqHz))
    {
        shared_->SetLastError(VE_INVALID_ARGUMENT, kTraceError,
            "ExternalPlayoutGetData() invalid sample rate");
        return -1;
    }
    if (current_delay_ms < 0)
    {
        shared_->SetLastError(VE_INVALID_ARGUMENT, kTraceError,
            "ExternalPlayoutGetData() invalid delay)");
        return -1;
    }

    AudioFrame audioFrame;

    // Retrieve mixed output at the specified rate
    shared_->output_mixer()->MixActiveChannels();
    shared_->output_mixer()->DoOperationsOnCombinedSignal(true);
    shared_->output_mixer()->GetMixedAudio(samplingFreqHz, 1, &audioFrame);

    // Deliver audio (PCM) samples to the external sink
    memcpy(speechData10ms,
           audioFrame.data_,
           sizeof(int16_t)*(audioFrame.samples_per_channel_));
    lengthSamples = audioFrame.samples_per_channel_;

    // Store current playout delay (to be used by ExternalRecordingInsertData).
    playout_delay_ms_ = current_delay_ms;

    return 0;
#else
    shared_->SetLastError(VE_FUNC_NOT_SUPPORTED, kTraceError,
       "ExternalPlayoutGetData() external playout is not supported");
    return -1;
#endif
}

int AoEExternalMediaImpl::GetAudioFrame(int channel, int desired_sample_rate_hz,
                                        AudioFrame* frame) {
    VOIP_TRACE(kTraceApiCall, kTraceVoice,
                 AoEId(shared_->instance_id(), channel),
                 "GetAudioFrame(channel=%d, desired_sample_rate_hz=%d)",
                 channel, desired_sample_rate_hz);
    if (!shared_->statistics().Initialized())
    {
        shared_->SetLastError(VE_NOT_INITED, kTraceError);
        return -1;
    }
    aoe::ChannelOwner ch = shared_->channel_manager().GetChannel(channel);
    aoe::Channel* channelPtr = ch.channel();
    if (channelPtr == NULL)
    {
        shared_->SetLastError(VE_CHANNEL_NOT_VALID, kTraceError,
            "GetAudioFrame() failed to locate channel");
        return -1;
    }
    if (!channelPtr->ExternalMixing()) {
        shared_->SetLastError(VE_INVALID_OPERATION, kTraceError,
            "GetAudioFrame() was called on channel that is not"
            " externally mixed.");
        return -1;
    }
    if (!channelPtr->Playing()) {
        shared_->SetLastError(VE_INVALID_OPERATION, kTraceError,
            "GetAudioFrame() was called on channel that is not playing.");
        return -1;
    }
    if (desired_sample_rate_hz == -1) {
          shared_->SetLastError(VE_BAD_ARGUMENT, kTraceError,
              "GetAudioFrame() was called with bad sample rate.");
          return -1;
    }
    frame->sample_rate_hz_ = desired_sample_rate_hz == 0 ? -1 :
                             desired_sample_rate_hz;
    return channelPtr->GetAudioFrame(channel, *frame);
}

int AoEExternalMediaImpl::SetExternalMixing(int channel, bool enable) {
    VOIP_TRACE(kTraceApiCall, kTraceVoice,
                 AoEId(shared_->instance_id(), channel),
                 "SetExternalMixing(channel=%d, enable=%d)", channel, enable);
    if (!shared_->statistics().Initialized())
    {
        shared_->SetLastError(VE_NOT_INITED, kTraceError);
        return -1;
    }
    aoe::ChannelOwner ch = shared_->channel_manager().GetChannel(channel);
    aoe::Channel* channelPtr = ch.channel();
    if (channelPtr == NULL)
    {
        shared_->SetLastError(VE_CHANNEL_NOT_VALID, kTraceError,
            "SetExternalMixing() failed to locate channel");
        return -1;
    }
    return channelPtr->SetExternalMixing(enable);
}

#endif  // VOIP_AUDIO_ENGINE_EXTERNAL_MEDIA_API

}  // namespace VoIP
