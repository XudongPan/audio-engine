
#ifndef VOIP_AUDIO_ENGINE_OUTPUT_MIXER_H_
#define VOIP_AUDIO_ENGINE_OUTPUT_MIXER_H_

#include "audio_engine/common_audio/resampler/include/push_resampler.h"
#include "audio_engine/include/common_types.h"
#include "audio_engine/modules/audio_mixer/interface/audio_mixer_defines.h"
#include "audio_engine/modules/audio_mixer/interface/audio_mixer.h"
#include "audio_engine/modules/audio_mixer/interface/audio_mixer_defines.h"
#include "audio_engine/voice_engine/level_indicator.h"
#include "audio_engine/voice_engine/audio_engine_defines.h"

namespace VoIP {

class AudioProcessing;
class CriticalSectionWrapper;
class FileWrapper;
class AoEMediaProcess;

namespace aoe {

class Statistics;

class OutputMixer : public AudioMixerOutputReceiver,
                    public AudioMixerStatusReceiver
{
public:
    static int32_t Create(OutputMixer*& mixer, uint32_t instanceId);

    static void Destroy(OutputMixer*& mixer);

    int32_t SetEngineInformation(Statistics& engineStatistics);

    int32_t SetAudioProcessingModule(
        AudioProcessing* audioProcessingModule);

    // AoEExternalMedia
    int RegisterExternalMediaProcessing(
        AoEMediaProcess& proccess_object);

    int DeRegisterExternalMediaProcessing();

  
    int32_t MixActiveChannels();

    int32_t DoOperationsOnCombinedSignal(bool feed_data_to_apm);

    int32_t SetMixabilityStatus(MixerParticipant& participant,
                                bool mixable);

    int32_t SetAnonymousMixabilityStatus(MixerParticipant& participant,
                                         bool mixable);

    int GetMixedAudio(int sample_rate_hz, int num_channels,
                      AudioFrame* audioFrame);

    // AoEVolumeControl
    int GetSpeechOutputLevel(uint32_t& level);

    int GetSpeechOutputLevelFullRange(uint32_t& level);

    int SetOutputVolumePan(float left, float right);

    int GetOutputVolumePan(float& left, float& right);

    virtual ~OutputMixer();

    // from AudioMixerOutputReceiver
    virtual void NewMixedAudio(
        int32_t id,
        const AudioFrame& generalAudioFrame,
        const AudioFrame** uniqueAudioFrames,
        uint32_t size);

    // from AudioMixerStatusReceiver
    virtual void MixedParticipants(
        int32_t id,
        const ParticipantStatistics* participantStatistics,
        uint32_t size);

    virtual void VADPositiveParticipants(
        int32_t id,
        const ParticipantStatistics* participantStatistics,
        uint32_t size);

    virtual void MixedAudioLevel(int32_t id, uint32_t level);

    // For file recording
    void PlayNotification(int32_t id, uint32_t durationMs);

    void RecordNotification(int32_t id, uint32_t durationMs);

    void PlayFileEnded(int32_t id);
    void RecordFileEnded(int32_t id);

private:
    OutputMixer(uint32_t instanceId);
    void APMAnalyzeReverseStream();
    int InsertInbandDtmfTone();

    // uses
    Statistics* _engineStatisticsPtr;
    AudioProcessing* _audioProcessingModulePtr;

    // owns
    CriticalSectionWrapper& _callbackCritSect;
    CriticalSectionWrapper& _fileCritSect;
    AudioConferenceMixer& _mixerModule;
    AudioFrame _audioFrame;
    // Converts mixed audio to the audio device output rate.
    PushResampler<int16_t> resampler_;
    // Converts mixed audio to the audio processing rate.
    PushResampler<int16_t> audioproc_resampler_;
    AudioLevel _audioLevel;    // measures audio level for the combined signal
    int _instanceId;
    AoEMediaProcess* _externalMediaCallbackPtr;
    bool _externalMedia;
    float _panLeft;
    float _panRight;
    int _mixingFrequencyHz;
    bool _outputFileRecording;
};

}  // namespace aoe

}  // namespace werbtc

#endif  // AUDIO_ENGINE_OUTPUT_MIXER_H_
