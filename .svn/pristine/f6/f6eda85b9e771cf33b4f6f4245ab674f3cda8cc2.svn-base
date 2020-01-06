
#ifndef VOIP_AUDIO_ENGINE_TRANSMIT_MIXER_H
#define VOIP_AUDIO_ENGINE_TRANSMIT_MIXER_H

#include "audio_engine/common_audio/resampler/include/push_resampler.h"
#include "audio_engine/include/common_types.h"
#include "audio_engine/modules/audio_processing/typing_detection.h"
#include "audio_engine/modules/interface/module_common_types.h"
#include "audio_engine/modules/utility/interface/file_player.h"
#include "audio_engine/system_wrappers/interface/scoped_ptr.h"
#include "audio_engine/voice_engine/include/aoe_base.h"
#include "audio_engine/voice_engine/level_indicator.h"
#include "audio_engine/voice_engine/monitor_module.h"
#include "audio_engine/voice_engine/audio_engine_defines.h"

namespace VoIP {

class AudioProcessing;
class ProcessThread;
class AoEExternalMedia;
class AoEMediaProcess;

namespace aoe {

class ChannelManager;
class MixedAudio;
class Statistics;

class TransmitMixer : public MonitorObserver {
public:
    static int32_t Create(TransmitMixer*& mixer, uint32_t instanceId);

    static void Destroy(TransmitMixer*& mixer);

    int32_t SetEngineInformation(ProcessThread& processThread,
                                 Statistics& engineStatistics,
                                 ChannelManager& channelManager);

    int32_t SetAudioProcessingModule(
        AudioProcessing* audioProcessingModule);

    int32_t PrepareDemux(const void* audioSamples,
                         uint32_t nSamples,
                         uint8_t  nChannels,
                         uint32_t samplesPerSec,
                         uint16_t totalDelayMS,
                         int32_t  clockDrift,
                         uint16_t currentMicLevel,
                         bool keyPressed);


    int32_t DemuxAndMix();
    // Used by the Chrome to pass the recording data to the specific AoE
    // channels for demux.
    void DemuxAndMix(const int voe_channels[], int number_of_voe_channels);

    int32_t EncodeAndSend();
    // Used by the Chrome to pass the recording data to the specific AoE
    // channels for encoding and sending to the network.
    void EncodeAndSend(const int voe_channels[], int number_of_voe_channels);

    // Must be called on the same thread as PrepareDemux().
    uint32_t CaptureLevel() const;

    int32_t StopSend();

    // AoEDtmf
    void UpdateMuteMicrophoneTime(uint32_t lengthMs);

    // AoEExternalMedia
    int RegisterExternalMediaProcessing(AoEMediaProcess* object,
                                        ProcessingTypes type);
    int DeRegisterExternalMediaProcessing(ProcessingTypes type);

    int GetMixingFrequency();

    // AoEVolumeControl
    int SetMute(bool enable);

    bool Mute() const;

    int8_t AudioLevel() const;

    int16_t AudioLevelFullRange() const;

    bool IsRecordingCall();

    bool IsRecordingMic();

    void SetMixWithMicStatus(bool mix);

    int32_t RegisterAudioEngineObserver(AudioEngineObserver& observer);

    virtual ~TransmitMixer();

    // MonitorObserver
    void OnPeriodicProcess();


    // FileCallback
    void PlayNotification(int32_t id,
                          uint32_t durationMs);

    void RecordNotification(int32_t id,
                            uint32_t durationMs);

    void PlayFileEnded(int32_t id);

    void RecordFileEnded(int32_t id);

#ifdef VOIP_AUDIO_ENGINE_TYPING_DETECTION
    // Typing detection
    int TimeSinceLastTyping(int &seconds);
    int SetTypingDetectionParameters(int timeWindow,
                                     int costPerTyping,
                                     int reportingThreshold,
                                     int penaltyDecay,
                                     int typeEventDelay);
#endif

  void EnableStereoChannelSwapping(bool enable);
  bool IsStereoChannelSwappingEnabled();

private:
    TransmitMixer(uint32_t instanceId);

    // Gets the maximum sample rate and number of channels over all currently
    // sending codecs.
    void GetSendCodecInfo(int* max_sample_rate, int* max_channels);

    void GenerateAudioFrame(const int16_t audioSamples[],
                            int nSamples,
                            int nChannels,
                            int samplesPerSec);
    int32_t RecordAudioToFile(uint32_t mixingFrequency);

    int32_t MixOrReplaceAudioWithFile(
        int mixingFrequency);

    void ProcessAudio(int delay_ms, int clock_drift, int current_mic_level,
                      bool key_pressed);

#ifdef VOIP_AUDIO_ENGINE_TYPING_DETECTION
    void TypingDetection(bool keyPressed);
#endif

    // uses
    Statistics* _engineStatisticsPtr;
    ChannelManager* _channelManagerPtr;
    AudioProcessing* audioproc_;
    AudioEngineObserver* _voiceEngineObserverPtr;
    ProcessThread* _processThreadPtr;

    // owns
    MonitorModule _monitorModule;
    AudioFrame _audioFrame;
    PushResampler<int16_t> resampler_;  // ADM sample rate -> mixing rate
    FilePlayer* _filePlayerPtr;
    int _filePlayerId;
    int _fileRecorderId;
    int _fileCallRecorderId;
    bool _filePlaying;
    bool _fileRecording;
    bool _fileCallRecording;
    aoe::AudioLevel _audioLevel;
    // protect file instances and their variables in MixedParticipants()
    CriticalSectionWrapper& _critSect;
    CriticalSectionWrapper& _callbackCritSect;

#ifdef VOIP_AUDIO_ENGINE_TYPING_DETECTION
    VoIP::TypingDetection _typingDetection;
    bool _typingNoiseWarningPending;
    bool _typingNoiseDetected;
#endif
    bool _saturationWarning;

    int _instanceId;
    bool _mixFileWithMicrophone;
    uint32_t _captureLevel;
    AoEMediaProcess* external_postproc_ptr_;
    AoEMediaProcess* external_preproc_ptr_;
    bool _mute;
    int32_t _remainingMuteMicTimeMs;
    bool stereo_codec_;
    bool swap_stereo_channels_;
    scoped_ptr<int16_t[]> mono_buffer_;
};

}  // namespace aoe

}  // namespace VoIP

#endif  // VOIP_AUDIO_ENGINE_TRANSMIT_MIXER_H
