
#ifndef VOIP_AUDIO_ENGINE_CHANNEL_H
#define VOIP_AUDIO_ENGINE_CHANNEL_H

#include "audio_engine/common_audio/resampler/include/push_resampler.h"
#include "audio_engine/include/common_types.h"
#include "audio_engine/modules/audio_mixer/interface/audio_mixer_defines.h"
#include "audio_engine/modules/utility/interface/file_player.h"
#include "audio_engine/system_wrappers/interface/scoped_ptr.h"

#include "audio_engine/voice_engine/include/aoe_audio_processing.h"
#include "audio_engine/voice_engine/include/aoe_network.h"
#include "audio_engine/voice_engine/level_indicator.h"
#include "audio_engine/voice_engine/shared_data.h"
#include "audio_engine/voice_engine/audio_engine_defines.h"


namespace VoIP {

class AudioDeviceModule;
class Config;
class CriticalSectionWrapper;
class FileWrapper;
class ProcessThread;
class ReceiveStatistics;
class AoEMediaProcess;
class AudioEngineObserver;

namespace aoe {

class Statistics;
class StatisticsProxy;
class TransmitMixer;
class OutputMixer;

#define VOIP_10MS_PCM_AUDIO 960
// Helper class to simplify locking scheme for members that are accessed from
// multiple threads.
// Example: a member can be set on thread T1 and read by an internal audio
// thread T2. Accessing the member via this class ensures that we are
// safe and also avoid TSan v2 warnings.
class ChannelState {
 public:
    struct State {
        State() : rx_apm_is_enabled(false),
                  input_external_media(false),
                  output_file_playing(false),
                  input_file_playing(false),
                  playing(false),
                  sending(false),
                  receiving(false) {}

        bool rx_apm_is_enabled;
        bool input_external_media;
        bool output_file_playing;
        bool input_file_playing;
        bool playing;
        bool sending;
        bool receiving;
    };

    ChannelState() : lock_(CriticalSectionWrapper::CreateCriticalSection()) {
    }
    virtual ~ChannelState() {}

    void Reset() {
        CriticalSectionScoped lock(lock_.get());
        state_ = State();
    }

    State Get() const {
        CriticalSectionScoped lock(lock_.get());
        return state_;
    }

    void SetRxApmIsEnabled(bool enable) {
        CriticalSectionScoped lock(lock_.get());
        state_.rx_apm_is_enabled = enable;
    }

    void SetInputExternalMedia(bool enable) {
        CriticalSectionScoped lock(lock_.get());
        state_.input_external_media = enable;
    }

    void SetOutputFilePlaying(bool enable) {
        CriticalSectionScoped lock(lock_.get());
        state_.output_file_playing = enable;
    }

    void SetInputFilePlaying(bool enable) {
        CriticalSectionScoped lock(lock_.get());
        state_.input_file_playing = enable;
    }

    void SetPlaying(bool enable) {
        CriticalSectionScoped lock(lock_.get());
        state_.playing = enable;
    }

    void SetSending(bool enable) {
        CriticalSectionScoped lock(lock_.get());
        state_.sending = enable;
    }

    void SetReceiving(bool enable) {
        CriticalSectionScoped lock(lock_.get());
        state_.receiving = enable;
    }

private:
    scoped_ptr<CriticalSectionWrapper> lock_;
    State state_;
};

class Channel:
    public Transport,
    public MixerParticipant // supplies output mixer with audio frames
{
public:
    enum {KNumSocketThreads = 1};
    enum {KNumberOfSocketBuffers = 8};
    virtual ~Channel();
    static int32_t CreateChannel(Channel*& channel,
                                 int32_t channelId,
                                 uint32_t instanceId,
                                 const Config& config);
    Channel(int32_t channelId, uint32_t instanceId, const Config& config);
    int32_t Init();
    int32_t SetEngineInformation(
        Statistics& engineStatistics,
        OutputMixer& outputMixer,
        TransmitMixer& transmitMixer,
        ProcessThread& moduleProcessThread,
        AudioDeviceModule& audioDeviceModule,
        AudioEngineObserver* voiceEngineObserver,
        CriticalSectionWrapper* callbackCritSect);
    int32_t UpdateLocalTimeStamp();

    // API methods

    // AoEBase
    int32_t StartPlayout();
    int32_t StopPlayout();
    int32_t StartSend();
    int32_t StopSend();
    int32_t StartReceiving();
    int32_t StopReceiving();

    int32_t RegisterAudioEngineObserver(AudioEngineObserver& observer);
    int32_t DeRegisterAudioEngineObserver();

    // AoENetwork
    int32_t RegisterExternalTransport(Transport& transport);
    int32_t DeRegisterExternalTransport();
	int32_t ReceivedPlayoutData(AudioFrame &frame);

    // AoEExternalMediaProcessing
    int RegisterExternalMediaProcessing(ProcessingTypes type,
                                        AoEMediaProcess& processObject);
    int DeRegisterExternalMediaProcessing(ProcessingTypes type);
    int SetExternalMixing(bool enabled);

    // AoEVolumeControl
    int GetSpeechOutputLevel(uint32_t& level) const;
    int GetSpeechOutputLevelFullRange(uint32_t& level) const;
    int SetMute(bool enable);
    bool Mute() const;
    int SetOutputVolumePan(float left, float right);
    int GetOutputVolumePan(float& left, float& right) const;
    int SetChannelOutputVolumeScaling(float scaling);
    int GetChannelOutputVolumeScaling(float& scaling) const;

    // AoEAudioProcessingImpl
    int UpdateRxVadDetection(AudioFrame& audioFrame);
    int RegisterRxVadObserver(AoERxVadCallback &observer);
    int DeRegisterRxVadObserver();
    int VoiceActivityIndicator(int &activity);
#ifdef VOIP_AUDIO_ENGINE_AGC
    int SetRxAgcStatus(bool enable, AgcModes mode);
    int GetRxAgcStatus(bool& enabled, AgcModes& mode);
    int SetRxAgcConfig(AgcConfig config);
    int GetRxAgcConfig(AgcConfig& config);
#endif
#ifdef VOIP_AUDIO_ENGINE_NR
    int SetRxNsStatus(bool enable, NsModes mode);
    int GetRxNsStatus(bool& enabled, NsModes& mode);
#endif

    int32_t OnRxVadDetected(int vadDecision);

    // From Transport 
//     int SendPacket(int /*channel*/, const void *data, int len);
//     int SendCtrlPacket(int /*channel*/, const void *data, int len);
	int SendRecPacket(int channel, AudioFrame & frame);

    // From MixerParticipant
    int32_t GetAudioFrame(int32_t id, AudioFrame& audioFrame);
    int32_t NeededFrequency(int32_t id);

    // From MonitorObserver
    void OnPeriodicProcess();

    uint32_t InstanceId() const
    {
        return _instanceId;
    }
    int32_t ChannelId() const
    {
        return _channelId;
    }
    bool Playing() const
    {
        return channel_state_.Get().playing;
    }
    bool Sending() const
    {
        return channel_state_.Get().sending;
    }
    bool Receiving() const
    {
        return channel_state_.Get().receiving;
    }
    bool ExternalTransport() const
    {
        CriticalSectionScoped cs(&_callbackCritSect);
        return _externalTransport;
    }
    bool ExternalMixing() const
    {
        return _externalMixing;
    }
    int8_t OutputEnergyLevel() const
    {
        return _outputAudioLevel.Level();
    }
    uint32_t Demultiplex(const AudioFrame& audioFrame);
    // Demultiplex the data to the channel's |_audioFrame|. The difference
    // between this method and the overloaded method above is that |audio_data|
    // does not go through transmit_mixer and APM.
    void Demultiplex(const int16_t* audio_data,
                     int sample_rate,
                     int number_of_frames,
                     int number_of_channels);
    uint32_t PrepareEncodeAndSend(int mixingFrequency);
    uint32_t EncodeAndSend();

private:

    int32_t MixOrReplaceAudioWithFile(int mixingFrequency);
    int32_t MixAudioWithFile(AudioFrame& audioFrame, int mixingFrequency);
   
    int Process10msFrame(AudioFrame& frame);
	int PreprocessToAddData(const AudioFrame& in_frame,const AudioFrame** ptr_out);

    CriticalSectionWrapper& _fileCritSect;
    CriticalSectionWrapper& _callbackCritSect;
    CriticalSectionWrapper& volume_settings_critsect_;
	CriticalSectionWrapper& _frameProcessCritSect;

	uint32_t last_in_timestamp_;
	uint32_t last_timestamp_;

    uint32_t _instanceId;
    int32_t _channelId;

    ChannelState channel_state_;

    AudioLevel _outputAudioLevel;
    bool _externalTransport;
    AudioFrame _recAudioFrame;
	AudioFrame _playoutAudioFrame;
	AudioFrame preprocess_frame_;

    scoped_ptr<int16_t[]> mono_recording_audio_;
    // Downsamples to the codec rate if necessary.
    PushResampler<int16_t> input_resampler_;
    uint8_t _audioLevel_dBov;
    FilePlayer* _inputFilePlayerPtr;
    FilePlayer* _outputFilePlayerPtr;
    int _inputFilePlayerId;
    int _outputFilePlayerId;
    int _outputFileRecorderId;
    bool _outputFileRecording;

    bool _outputExternalMedia;
    AoEMediaProcess* _inputExternalMediaCallbackPtr;
    AoEMediaProcess* _outputExternalMediaCallbackPtr;
    uint32_t _timeStamp;
    uint8_t _sendTelephoneEventPayloadType;

    // uses
    Statistics* _engineStatisticsPtr;
    OutputMixer* _outputMixerPtr;
    TransmitMixer* _transmitMixerPtr;
    ProcessThread* _moduleProcessThreadPtr;
    AudioDeviceModule* _audioDeviceModulePtr;
    AudioEngineObserver* _voiceEngineObserverPtr; // owned by base
    CriticalSectionWrapper* _callbackCritSectPtr; // owned by base
    Transport* _transportPtr; // VoIP socket or external transport
    scoped_ptr<AudioProcessing> rx_audioproc_; // far end AudioProcessing
    AoERxVadCallback* _rxVadObserverPtr;
    int32_t _oldVadDecision;
    int32_t _sendFrameType; // Send data is voice, 1-voice, 0-otherwise

    // AoEBase
    bool _externalPlayout;
    bool _externalMixing;
    bool _mixFileWithMicrophone;

    // AoEVolumeControl
    bool _mute;
    float _panLeft;
    float _panRight;
    float _outputGain;

    bool _includeAudioLevelIndication;

    AudioFrame::SpeechType _outputSpeechType;

    // AoEAudioProcessing
    bool _RxVadDetection;
    bool _rxAgcIsEnabled;
    bool _rxNsIsEnabled;
    bool restored_packet_in_use_;
};

}  // namespace aoe
}  // namespace VoIP

#endif  // VOIP_AUDIO_ENGINE_CHANNEL_H
