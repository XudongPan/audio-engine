
#include "audio_engine/voice_engine/output_mixer.h"
#include "audio_engine/modules/audio_processing/include/audio_processing.h"
#include "audio_engine/modules/utility/interface/audio_frame_operations.h"
#include "audio_engine/system_wrappers/interface/critical_section_wrapper.h"
#include "audio_engine/system_wrappers/interface/file_wrapper.h"
#include "audio_engine/system_wrappers/interface/trace.h"
#include "audio_engine/voice_engine/include/aoe_external_media.h"
#include "audio_engine/voice_engine/statistics.h"
#include "audio_engine/voice_engine/utility.h"

namespace VoIP {
namespace aoe {

void
OutputMixer::NewMixedAudio(int32_t id,
                           const AudioFrame& generalAudioFrame,
                           const AudioFrame** uniqueAudioFrames,
                           uint32_t size)
{
    VOIP_TRACE(kTraceStream, kTraceVoice, AoEId(_instanceId,-1),
                 "OutputMixer::NewMixedAudio(id=%d, size=%u)", id, size);

    _audioFrame.CopyFrom(generalAudioFrame);
    _audioFrame.id_ = id;
}

void OutputMixer::MixedParticipants(
    int32_t id,
    const ParticipantStatistics* participantStatistics,
    uint32_t size)
{
    VOIP_TRACE(kTraceStream, kTraceVoice, AoEId(_instanceId,-1),
                 "OutputMixer::MixedParticipants(id=%d, size=%u)", id, size);
}

void OutputMixer::VADPositiveParticipants(int32_t id,
    const ParticipantStatistics* participantStatistics, uint32_t size)
{
    VOIP_TRACE(kTraceStream, kTraceVoice, AoEId(_instanceId,-1),
                 "OutputMixer::VADPositiveParticipants(id=%d, size=%u)",
                 id, size);
}

void OutputMixer::MixedAudioLevel(int32_t id, uint32_t level)
{
    VOIP_TRACE(kTraceStream, kTraceVoice, AoEId(_instanceId,-1),
                 "OutputMixer::MixedAudioLevel(id=%d, level=%u)", id, level);
}

void OutputMixer::PlayNotification(int32_t id, uint32_t durationMs)
{
    VOIP_TRACE(kTraceStream, kTraceVoice, AoEId(_instanceId,-1),
                 "OutputMixer::PlayNotification(id=%d, durationMs=%d)",
                 id, durationMs);
    // Not implement yet
}

void OutputMixer::RecordNotification(int32_t id,
                                     uint32_t durationMs)
{
    VOIP_TRACE(kTraceStream, kTraceVoice, AoEId(_instanceId,-1),
                 "OutputMixer::RecordNotification(id=%d, durationMs=%d)",
                 id, durationMs);

    // Not implement yet
}

void OutputMixer::PlayFileEnded(int32_t id)
{
    VOIP_TRACE(kTraceStream, kTraceVoice, AoEId(_instanceId,-1),
                 "OutputMixer::PlayFileEnded(id=%d)", id);

    // not needed
}

void OutputMixer::RecordFileEnded(int32_t id)
{
    VOIP_TRACE(kTraceStream, kTraceVoice, AoEId(_instanceId,-1),
                 "OutputMixer::RecordFileEnded(id=%d)", id);
    assert(id == _instanceId);

    CriticalSectionScoped cs(&_fileCritSect);
    _outputFileRecording = false;
    VOIP_TRACE(kTraceStateInfo, kTraceVoice, AoEId(_instanceId,-1),
                 "OutputMixer::RecordFileEnded() =>"
                 "output file recorder module is shutdown");
}

int32_t
OutputMixer::Create(OutputMixer*& mixer, uint32_t instanceId)
{
    VOIP_TRACE(kTraceMemory, kTraceVoice, instanceId,
                 "OutputMixer::Create(instanceId=%d)", instanceId);
    mixer = new OutputMixer(instanceId);
    if (mixer == NULL)
    {
        VOIP_TRACE(kTraceMemory, kTraceVoice, instanceId,
                     "OutputMixer::Create() unable to allocate memory for"
                     "mixer");
        return -1;
    }
    return 0;
}

OutputMixer::OutputMixer(uint32_t instanceId) :
    _callbackCritSect(*CriticalSectionWrapper::CreateCriticalSection()),
    _fileCritSect(*CriticalSectionWrapper::CreateCriticalSection()),
    _mixerModule(*AudioConferenceMixer::Create(instanceId)),
    _audioLevel(),

    _instanceId(instanceId),
    _externalMediaCallbackPtr(NULL),
    _externalMedia(false),
    _panLeft(1.0f),
    _panRight(1.0f),
    _mixingFrequencyHz(8000),
    _outputFileRecording(false)
{
    VOIP_TRACE(kTraceMemory, kTraceVoice, AoEId(_instanceId,-1),
                 "OutputMixer::OutputMixer() - ctor");

    if ((_mixerModule.RegisterMixedStreamCallback(*this) == -1) ||
        (_mixerModule.RegisterMixerStatusCallback(*this, 100) == -1))
    {
        VOIP_TRACE(kTraceError, kTraceVoice, AoEId(_instanceId,-1),
                     "OutputMixer::OutputMixer() failed to register mixer"
                     "callbacks");
    }


}

void
OutputMixer::Destroy(OutputMixer*& mixer)
{
    if (mixer)
    {
        delete mixer;
        mixer = NULL;
    }
}

OutputMixer::~OutputMixer()
{
    VOIP_TRACE(kTraceMemory, kTraceVoice, AoEId(_instanceId,-1),
                 "OutputMixer::~OutputMixer() - dtor");
    if (_externalMedia)
    {
        DeRegisterExternalMediaProcessing();
    }

    _mixerModule.UnRegisterMixerStatusCallback();
    _mixerModule.UnRegisterMixedStreamCallback();
    delete &_mixerModule;
    delete &_callbackCritSect;
    delete &_fileCritSect;
}

int32_t
OutputMixer::SetEngineInformation(aoe::Statistics& engineStatistics)
{
    VOIP_TRACE(kTraceInfo, kTraceVoice, AoEId(_instanceId,-1),
                 "OutputMixer::SetEngineInformation()");
    _engineStatisticsPtr = &engineStatistics;
    return 0;
}

int32_t
OutputMixer::SetAudioProcessingModule(AudioProcessing* audioProcessingModule)
{
    VOIP_TRACE(kTraceInfo, kTraceVoice, AoEId(_instanceId,-1),
                 "OutputMixer::SetAudioProcessingModule("
                 "audioProcessingModule=0x%x)", audioProcessingModule);
    _audioProcessingModulePtr = audioProcessingModule;
    return 0;
}

int OutputMixer::RegisterExternalMediaProcessing(
    AoEMediaProcess& proccess_object)
{
    VOIP_TRACE(kTraceInfo, kTraceVoice, AoEId(_instanceId,-1),
               "OutputMixer::RegisterExternalMediaProcessing()");

    CriticalSectionScoped cs(&_callbackCritSect);
    _externalMediaCallbackPtr = &proccess_object;
    _externalMedia = true;

    return 0;
}

int OutputMixer::DeRegisterExternalMediaProcessing()
{
    VOIP_TRACE(kTraceInfo, kTraceVoice, AoEId(_instanceId,-1),
                 "OutputMixer::DeRegisterExternalMediaProcessing()");

    CriticalSectionScoped cs(&_callbackCritSect);
    _externalMedia = false;
    _externalMediaCallbackPtr = NULL;

    return 0;
}


int32_t
OutputMixer::SetMixabilityStatus(MixerParticipant& participant,
                                 bool mixable)
{
    return _mixerModule.SetMixabilityStatus(participant, mixable);
}

int32_t
OutputMixer::SetAnonymousMixabilityStatus(MixerParticipant& participant,
                                          bool mixable)
{
    return _mixerModule.SetAnonymousMixabilityStatus(participant,mixable);
}

int32_t
OutputMixer::MixActiveChannels()
{
    return _mixerModule.Process();
}

int
OutputMixer::GetSpeechOutputLevel(uint32_t& level)
{
    int8_t currentLevel = _audioLevel.Level();
    level = static_cast<uint32_t> (currentLevel);
    VOIP_TRACE(kTraceStateInfo, kTraceVoice, AoEId(_instanceId,-1),
                 "GetSpeechOutputLevel() => level=%u", level);
    return 0;
}

int
OutputMixer::GetSpeechOutputLevelFullRange(uint32_t& level)
{
    int16_t currentLevel = _audioLevel.LevelFullRange();
    level = static_cast<uint32_t> (currentLevel);
    VOIP_TRACE(kTraceStateInfo, kTraceVoice, AoEId(_instanceId,-1),
                 "GetSpeechOutputLevelFullRange() => level=%u", level);
    return 0;
}

int
OutputMixer::SetOutputVolumePan(float left, float right)
{
    VOIP_TRACE(kTraceInfo, kTraceVoice, AoEId(_instanceId,-1),
                 "OutputMixer::SetOutputVolumePan()");
    _panLeft = left;
    _panRight = right;
    return 0;
}

int
OutputMixer::GetOutputVolumePan(float& left, float& right)
{
    left = _panLeft;
    right = _panRight;
    VOIP_TRACE(kTraceStateInfo, kTraceVoice, AoEId(_instanceId,-1),
                 "GetOutputVolumePan() => left=%2.1f, right=%2.1f",
                 left, right);
    return 0;
}


int OutputMixer::GetMixedAudio(int sample_rate_hz,
                               int num_channels,
                               AudioFrame* frame) {
  VOIP_TRACE(kTraceStream, kTraceVoice, AoEId(_instanceId,-1),
               "OutputMixer::GetMixedAudio(sample_rate_hz=%d, num_channels=%d)",
               sample_rate_hz, num_channels);

   frame->num_channels_ = num_channels;
  frame->sample_rate_hz_ = sample_rate_hz;
  // TODO(andrew): Ideally the downmixing would occur much earlier, in
  // AudioCodingModule.
  RemixAndResample(_audioFrame, &resampler_, frame);
  return 0;
}

int32_t
OutputMixer::DoOperationsOnCombinedSignal(bool feed_data_to_apm)
{
    if (_audioFrame.sample_rate_hz_ != _mixingFrequencyHz)
    {
        VOIP_TRACE(kTraceStream, kTraceVoice, AoEId(_instanceId,-1),
                     "OutputMixer::DoOperationsOnCombinedSignal() => "
                     "mixing frequency = %d", _audioFrame.sample_rate_hz_);
        _mixingFrequencyHz = _audioFrame.sample_rate_hz_;
    }



    // Scale left and/or right channel(s) if balance is active
    if (_panLeft != 1.0 || _panRight != 1.0)
    {
        if (_audioFrame.num_channels_ == 1)
        {
            AudioFrameOperations::MonoToStereo(&_audioFrame);
        }
        else
        {
            // Pure stereo mode (we are receiving a stereo signal).
        }

        assert(_audioFrame.num_channels_ == 2);
        AudioFrameOperations::Scale(_panLeft, _panRight, _audioFrame);
    }

    // --- Far-end Voice Quality Enhancement (AudioProcessing Module)
    if (feed_data_to_apm)
      APMAnalyzeReverseStream();

    // --- External media processing
    {
        CriticalSectionScoped cs(&_callbackCritSect);
        if (_externalMedia)
        {
            const bool is_stereo = (_audioFrame.num_channels_ == 2);
            if (_externalMediaCallbackPtr)
            {
                _externalMediaCallbackPtr->Process(
                    -1,
                    kPlaybackAllChannelsMixed,
                    (int16_t*)_audioFrame.data_,
                    _audioFrame.samples_per_channel_,
                    _audioFrame.sample_rate_hz_,
                    is_stereo);
            }
        }
    }

    // --- Measure audio level (0-9) for the combined signal
    _audioLevel.ComputeLevel(_audioFrame);

    return 0;
}

// ----------------------------------------------------------------------------
//                             Private methods
// ----------------------------------------------------------------------------

void OutputMixer::APMAnalyzeReverseStream() {
  // Convert from mixing to AudioProcessing sample rate, determined by the send
  // side. Downmix to mono.
  AudioFrame frame;
  frame.num_channels_ = 1;
  frame.sample_rate_hz_ = _audioProcessingModulePtr->sample_rate_hz();
  RemixAndResample(_audioFrame, &audioproc_resampler_, &frame);

  if (_audioProcessingModulePtr->AnalyzeReverseStream(&frame) == -1) {
    VOIP_TRACE(kTraceWarning, kTraceVoice, AoEId(_instanceId,-1),
                 "AudioProcessingModule::AnalyzeReverseStream() => error");
  }
}

}  // namespace aoe
}  // namespace VoIP
