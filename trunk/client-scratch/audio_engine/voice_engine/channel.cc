
#include "audio_engine/voice_engine/channel.h"

#include "audio_engine/include/common.h"
#include "audio_engine/modules/audio_device/include/audio_device.h"
#include "audio_engine/modules/audio_processing/include/audio_processing.h"
#include "audio_engine/modules/interface/module_common_types.h"
#include "audio_engine/modules/utility/interface/audio_frame_operations.h"
#include "audio_engine/modules/utility/interface/process_thread.h"
#include "audio_engine/system_wrappers/interface/critical_section_wrapper.h"
#include "audio_engine/system_wrappers/interface/logging.h"
#include "audio_engine/system_wrappers/interface/trace.h"

#include "audio_engine/voice_engine/include/aoe_base.h"
#include "audio_engine/voice_engine/include/aoe_external_media.h"
#include "audio_engine/voice_engine/output_mixer.h"
#include "audio_engine/voice_engine/statistics.h"
#include "audio_engine/voice_engine/transmit_mixer.h"
#include "audio_engine/voice_engine/utility.h"

#if defined(_WIN32)
#include <Qos.h>
#endif

namespace VoIP {
namespace aoe {


// Stereo-to-mono can be used as in-place.
int DownMix(const AudioFrame& frame, int length_out_buff, int16_t* out_buff);

// Mono-to-stereo can be used as in-place.
int UpMix(const AudioFrame& frame, int length_out_buff, int16_t* out_buff);

struct ChannelStatistics : public CProtocolStatistics {
  ChannelStatistics() : ctrlp(), max_jitter(0) {}

  CProtocolStatistics ctrlp;
  uint32_t max_jitter;
};

// Statistics callback, called at each generation of a new  report block.
class StatisticsProxy : public ProtocolStatisticsCallback {
 public:
  StatisticsProxy(uint32_t ssrc)
   : stats_lock_(CriticalSectionWrapper::CreateCriticalSection()),
     ssrc_(ssrc) {}
  virtual ~StatisticsProxy() {}

  virtual void StatisticsUpdated(const CProtocolStatistics& statistics,
                                 uint32_t ssrc) OVERRIDE {
    if (ssrc != ssrc_)
      return;

    CriticalSectionScoped cs(stats_lock_.get());
    stats_.ctrlp = statistics;
    if (statistics.jitter > stats_.max_jitter) {
      stats_.max_jitter = statistics.jitter;
    }
  }

  void ResetStatistics() {
    CriticalSectionScoped cs(stats_lock_.get());
    stats_ = ChannelStatistics();
  }

  ChannelStatistics GetStats() {
    CriticalSectionScoped cs(stats_lock_.get());
    return stats_;
  }

 private:
  // StatisticsUpdated calls are triggered from threads in the RTP module,
  // while GetStats calls can be triggered from the public voice engine API,
  // hence synchronization is needed.
  scoped_ptr<CriticalSectionWrapper> stats_lock_;
  const uint32_t ssrc_;
  ChannelStatistics stats_;
};


int32_t
Channel::OnRxVadDetected(int vadDecision)
{
    VOIP_TRACE(kTraceInfo, kTraceVoice, AoEId(_instanceId, _channelId),
                 "Channel::OnRxVadDetected(vadDecision=%d)", vadDecision);

    CriticalSectionScoped cs(&_callbackCritSect);
    if (_rxVadObserverPtr)
    {
        _rxVadObserverPtr->OnRxVad(_channelId, vadDecision);
    }

    return 0;
}

int32_t Channel::GetAudioFrame(int32_t id, AudioFrame& audioFrame)
{
    VOIP_TRACE(kTraceStream, kTraceVoice, AoEId(_instanceId,_channelId),
                 "Channel::GetAudioFrame(id=%d)", id);


	audioFrame.CopyFrom(_playoutAudioFrame);

    if (_RxVadDetection)
    {
        UpdateRxVadDetection(audioFrame);
    }

    // Convert module ID to internal AoE channel ID
    audioFrame.id_ = AoEChannelId(audioFrame.id_);
    // Store speech type for dead-or-alive detection
    _outputSpeechType = audioFrame.speech_type_;

    ChannelState::State state = channel_state_.Get();

    if (state.rx_apm_is_enabled) {
      int err = rx_audioproc_->ProcessStream(&audioFrame);
      if (err) {
        LOG(LS_ERROR) << "ProcessStream() error: " << err;
        assert(false);
      }
    }

    float output_gain = 1.0f;
    float left_pan =  1.0f;
    float right_pan =  1.0f;
    {
      CriticalSectionScoped cs(&volume_settings_critsect_);
      output_gain = _outputGain;
      left_pan = _panLeft;
      right_pan= _panRight;
    }

    // Output volume scaling
    if (output_gain < 0.99f || output_gain > 1.01f)
    {
        AudioFrameOperations::ScaleWithSat(output_gain, audioFrame);
    }

    // Scale left and/or right channel(s) if stereo and master balance is
    // active

    if (left_pan != 1.0f || right_pan != 1.0f)
    {
        if (audioFrame.num_channels_ == 1)
        {
            // Emulate stereo mode since panning is active.
            // The mono signal is copied to both left and right channels here.
            AudioFrameOperations::MonoToStereo(&audioFrame);
        }
        // For true stereo mode (when we are receiving a stereo signal), no
        // action is needed.

        // Do the panning operation (the audio frame contains stereo at this
        // stage)
        AudioFrameOperations::Scale(left_pan, right_pan, audioFrame);
    }

    // Mix decoded PCM output with file if file mixing is enabled
    if (state.output_file_playing)
    {
        MixAudioWithFile(audioFrame, audioFrame.sample_rate_hz_);
    }

    // External media
    if (_outputExternalMedia)
    {
        CriticalSectionScoped cs(&_callbackCritSect);
        const bool isStereo = (audioFrame.num_channels_ == 2);
        if (_outputExternalMediaCallbackPtr)
        {
            _outputExternalMediaCallbackPtr->Process(
                _channelId,
                kPlaybackPerChannel,
                (int16_t*)audioFrame.data_,
                audioFrame.samples_per_channel_,
                audioFrame.sample_rate_hz_,
                isStereo);
        }
    }

    // Measure audio level (0-9)
    _outputAudioLevel.ComputeLevel(audioFrame);

    return 0;
}

int32_t
Channel::NeededFrequency(int32_t id)
{
    VOIP_TRACE(kTraceStream, kTraceVoice, AoEId(_instanceId,_channelId),
                 "Channel::NeededFrequency(id=%d)", id);

    int highestNeeded = 8000;

  
    // Special case, if we're playing a file on the playout side
    // we take that frequency into consideration as well
    // This is not needed on sending side, since the codec will
    // limit the spectrum anyway.
    if (channel_state_.Get().output_file_playing)
    {
        CriticalSectionScoped cs(&_fileCritSect);
        if (_outputFilePlayerPtr)
        {
            if(_outputFilePlayerPtr->Frequency()>highestNeeded)
            {
                highestNeeded=_outputFilePlayerPtr->Frequency();
            }
        }
    }

    return(highestNeeded);
}

int32_t
Channel::CreateChannel(Channel*& channel,
                       int32_t channelId,
                       uint32_t instanceId,
                       const Config& config)
{
    VOIP_TRACE(kTraceMemory, kTraceVoice, AoEId(instanceId,channelId),
                 "Channel::CreateChannel(channelId=%d, instanceId=%d)",
        channelId, instanceId);

    channel = new Channel(channelId, instanceId, config);
    if (channel == NULL)
    {
        VOIP_TRACE(kTraceMemory, kTraceVoice,
                     AoEId(instanceId,channelId),
                     "Channel::CreateChannel() unable to allocate memory for"
                     " channel");
        return -1;
    }
    return 0;
}

Channel::Channel(int32_t channelId,
                 uint32_t instanceId,
                 const Config& config) :
    _fileCritSect(*CriticalSectionWrapper::CreateCriticalSection()),
    _callbackCritSect(*CriticalSectionWrapper::CreateCriticalSection()),
    volume_settings_critsect_(*CriticalSectionWrapper::CreateCriticalSection()),
	_frameProcessCritSect(*CriticalSectionWrapper::CreateCriticalSection()),
    _instanceId(instanceId),
    _channelId(channelId),
    _outputAudioLevel(),
    _externalTransport(false),
    _audioLevel_dBov(0),
    _inputFilePlayerPtr(NULL),
    _outputFilePlayerPtr(NULL),
    // Avoid conflict with other channels by adding 1024 - 1026,
    // won't use as much as 1024 channels.
    _inputFilePlayerId(AoEModuleId(instanceId, channelId) + 1024),
    _outputFilePlayerId(AoEModuleId(instanceId, channelId) + 1025),
    _outputFileRecorderId(AoEModuleId(instanceId, channelId) + 1026),
    _outputFileRecording(false),

    _outputExternalMedia(false),
    _inputExternalMediaCallbackPtr(NULL),
    _outputExternalMediaCallbackPtr(NULL),
    _timeStamp(0), // This is just an offset, RTP module will add it's own random offset
    _engineStatisticsPtr(NULL),
    _outputMixerPtr(NULL),
    _transmitMixerPtr(NULL),
    _moduleProcessThreadPtr(NULL),
    _audioDeviceModulePtr(NULL),
    _voiceEngineObserverPtr(NULL),
    _callbackCritSectPtr(NULL),
    _transportPtr(NULL),
    _rxVadObserverPtr(NULL),
    _oldVadDecision(-1),
    _sendFrameType(0),
    _externalPlayout(false),
    _externalMixing(false),
    _mixFileWithMicrophone(false),
    _mute(false),
    _panLeft(1.0f),
    _panRight(1.0f),
    _outputGain(1.0f),
    _includeAudioLevelIndication(true),
    _outputSpeechType(AudioFrame::kNormalSpeech),
    _RxVadDetection(false),
    _rxAgcIsEnabled(false),
    _rxNsIsEnabled(false),
    restored_packet_in_use_(false)
{
    VOIP_TRACE(kTraceMemory, kTraceVoice, AoEId(_instanceId,_channelId),
                 "Channel::Channel() - ctor");

    _outputAudioLevel.Clear();

    Config audioproc_config;
    audioproc_config.Set<ExperimentalAgc>(new ExperimentalAgc(false));
    rx_audioproc_.reset(AudioProcessing::Create(audioproc_config));
	last_in_timestamp_ = (0xD87F3F9F);
	last_timestamp_ = (0xD87F3F9F);
	

}

Channel::~Channel()
{
    VOIP_TRACE(kTraceMemory, kTraceVoice, AoEId(_instanceId,_channelId),
                 "Channel::~Channel() - dtor");

    if (_outputExternalMedia)
    {
        DeRegisterExternalMediaProcessing(kPlaybackPerChannel);
    }
    if (channel_state_.Get().input_external_media)
    {
        DeRegisterExternalMediaProcessing(kRecordingPerChannel);
    }
    StopSend();
    StopPlayout();

    {
        CriticalSectionScoped cs(&_fileCritSect);
        if (_inputFilePlayerPtr)
        {
            _inputFilePlayerPtr->RegisterModuleFileCallback(NULL);
            _inputFilePlayerPtr->StopPlayingFile();
            FilePlayer::DestroyFilePlayer(_inputFilePlayerPtr);
            _inputFilePlayerPtr = NULL;
        }
        if (_outputFilePlayerPtr)
        {
            _outputFilePlayerPtr->RegisterModuleFileCallback(NULL);
            _outputFilePlayerPtr->StopPlayingFile();
            FilePlayer::DestroyFilePlayer(_outputFilePlayerPtr);
            _outputFilePlayerPtr = NULL;
        }
      
    }

	delete &_callbackCritSect;
    delete &_fileCritSect;
    delete &volume_settings_critsect_;
	delete &_frameProcessCritSect;
}

int32_t
Channel::Init()
{
    VOIP_TRACE(kTraceInfo, kTraceVoice, AoEId(_instanceId,_channelId),
                 "Channel::Init()");

    channel_state_.Reset();

    // --- Initial sanity

    if ((_engineStatisticsPtr == NULL) ||
        (_moduleProcessThreadPtr == NULL))
    {
        VOIP_TRACE(kTraceError, kTraceVoice,
                     AoEId(_instanceId,_channelId),
                     "Channel::Init() must call SetEngineInformation() first");
        return -1;
    }
	
    if (rx_audioproc_->noise_suppression()->set_level(kDefaultNsMode) != 0) {
      LOG_FERR1(LS_ERROR, noise_suppression()->set_level, kDefaultNsMode);
      return -1;
    }
    if (rx_audioproc_->gain_control()->set_mode(kDefaultRxAgcMode) != 0) {
      LOG_FERR1(LS_ERROR, gain_control()->set_mode, kDefaultRxAgcMode);
      return -1;
    }

    return 0;
}

int32_t
Channel::SetEngineInformation(Statistics& engineStatistics,
                              OutputMixer& outputMixer,
                              aoe::TransmitMixer& transmitMixer,
                              ProcessThread& moduleProcessThread,
                              AudioDeviceModule& audioDeviceModule,
                              AudioEngineObserver* voiceEngineObserver,
                              CriticalSectionWrapper* callbackCritSect)
{
    VOIP_TRACE(kTraceInfo, kTraceVoice, AoEId(_instanceId,_channelId),
                 "Channel::SetEngineInformation()");
    _engineStatisticsPtr = &engineStatistics;
    _outputMixerPtr = &outputMixer;
    _transmitMixerPtr = &transmitMixer,
    _moduleProcessThreadPtr = &moduleProcessThread;
    _audioDeviceModulePtr = &audioDeviceModule;
    _voiceEngineObserverPtr = voiceEngineObserver;
    _callbackCritSectPtr = callbackCritSect;
    return 0;
}

int32_t
Channel::UpdateLocalTimeStamp()
{

    _timeStamp += _recAudioFrame.samples_per_channel_;
    return 0;
}

int32_t
Channel::StartPlayout()
{
    VOIP_TRACE(kTraceInfo, kTraceVoice, AoEId(_instanceId,_channelId),
                 "Channel::StartPlayout()");
    if (channel_state_.Get().playing)
    {
        return 0;
    }

    if (!_externalMixing) {
        // Add participant as candidates for mixing.
        if (_outputMixerPtr->SetMixabilityStatus(*this, true) != 0)
        {
            _engineStatisticsPtr->SetLastError(
                VE_AUDIO_CONF_MIX_MODULE_ERROR, kTraceError,
                "StartPlayout() failed to add participant to mixer");
            return -1;
        }
    }

    channel_state_.SetPlaying(true);
   
    return 0;
}

int32_t
Channel::StopPlayout()
{
    VOIP_TRACE(kTraceInfo, kTraceVoice, AoEId(_instanceId,_channelId),
                 "Channel::StopPlayout()");
    if (!channel_state_.Get().playing)
    {
        return 0;
    }

    if (!_externalMixing) {
        // Remove participant as candidates for mixing
        if (_outputMixerPtr->SetMixabilityStatus(*this, false) != 0)
        {
            _engineStatisticsPtr->SetLastError(
                VE_AUDIO_CONF_MIX_MODULE_ERROR, kTraceError,
                "StopPlayout() failed to remove participant from mixer");
            return -1;
        }
    }

    channel_state_.SetPlaying(false);
    _outputAudioLevel.Clear();

    return 0;
}

int32_t
Channel::StartSend()
{
    VOIP_TRACE(kTraceInfo, kTraceVoice, AoEId(_instanceId,_channelId),
                 "Channel::StartSend()");
    // Resume the previous sequence number which was reset by StopSend().

    if (channel_state_.Get().sending)
    {
      return 0;
    }
    channel_state_.SetSending(true);


    return 0;
}

int32_t
Channel::StopSend()
{
    VOIP_TRACE(kTraceInfo, kTraceVoice, AoEId(_instanceId,_channelId),
                 "Channel::StopSend()");
    if (!channel_state_.Get().sending)
    {
      return 0;
    }
    channel_state_.SetSending(false);

    return 0;
}

int32_t
Channel::StartReceiving()
{
    VOIP_TRACE(kTraceInfo, kTraceVoice, AoEId(_instanceId,_channelId),
                 "Channel::StartReceiving()");
    if (channel_state_.Get().receiving)
    {
        return 0;
    }
    channel_state_.SetReceiving(true);
    
    return 0;
}

int32_t
Channel::StopReceiving()
{
    VOIP_TRACE(kTraceInfo, kTraceVoice, AoEId(_instanceId,_channelId),
                 "Channel::StopReceiving()");
    if (!channel_state_.Get().receiving)
    {
        return 0;
    }

    channel_state_.SetReceiving(false);
    return 0;
}


int32_t
Channel::RegisterAudioEngineObserver(AudioEngineObserver& observer)
{
    VOIP_TRACE(kTraceInfo, kTraceVoice, AoEId(_instanceId,_channelId),
                 "Channel::RegisterAudioEngineObserver()");
    CriticalSectionScoped cs(&_callbackCritSect);

    if (_voiceEngineObserverPtr)
    {
        _engineStatisticsPtr->SetLastError(
            VE_INVALID_OPERATION, kTraceError,
            "RegisterAudioEngineObserver() observer already enabled");
        return -1;
    }
    _voiceEngineObserverPtr = &observer;
    return 0;
}

int32_t
Channel::DeRegisterAudioEngineObserver()
{
    VOIP_TRACE(kTraceInfo, kTraceVoice, AoEId(_instanceId,_channelId),
                 "Channel::DeRegisterAudioEngineObserver()");
    CriticalSectionScoped cs(&_callbackCritSect);

    if (!_voiceEngineObserverPtr)
    {
        _engineStatisticsPtr->SetLastError(
            VE_INVALID_OPERATION, kTraceWarning,
            "DeRegisterAudioEngineObserver() observer already disabled");
        return 0;
    }
    _voiceEngineObserverPtr = NULL;
    return 0;
}

int32_t Channel::RegisterExternalTransport(Transport& transport)
{
    VOIP_TRACE(kTraceInfo, kTraceVoice, AoEId(_instanceId, _channelId),
               "Channel::RegisterExternalTransport()");

    CriticalSectionScoped cs(&_callbackCritSect);

    if (_externalTransport)
    {
        _engineStatisticsPtr->SetLastError(VE_INVALID_OPERATION,
                                           kTraceError,
              "RegisterExternalTransport() external transport already enabled");
       return -1;
    }
    _externalTransport = true;
    _transportPtr = &transport;
    return 0;
}

int32_t
Channel::DeRegisterExternalTransport()
{
    VOIP_TRACE(kTraceInfo, kTraceVoice, AoEId(_instanceId,_channelId),
                 "Channel::DeRegisterExternalTransport()");

    CriticalSectionScoped cs(&_callbackCritSect);

    if (!_transportPtr)
    {
        _engineStatisticsPtr->SetLastError(
            VE_INVALID_OPERATION, kTraceWarning,
            "DeRegisterExternalTransport() external transport already "
            "disabled");
        return 0;
    }
    _externalTransport = false;
    _transportPtr = NULL;
    VOIP_TRACE(kTraceInfo, kTraceVoice, AoEId(_instanceId,_channelId),
                 "DeRegisterExternalTransport() all transport is disabled");
    return 0;
}

int
Channel::GetSpeechOutputLevel(uint32_t& level) const
{
    int8_t currentLevel = _outputAudioLevel.Level();
    level = static_cast<int32_t> (currentLevel);
    VOIP_TRACE(kTraceStateInfo, kTraceVoice,
               AoEId(_instanceId,_channelId),
               "GetSpeechOutputLevel() => level=%u", level);
    return 0;
}

int
Channel::GetSpeechOutputLevelFullRange(uint32_t& level) const
{
    int16_t currentLevel = _outputAudioLevel.LevelFullRange();
    level = static_cast<int32_t> (currentLevel);
    VOIP_TRACE(kTraceStateInfo, kTraceVoice,
               AoEId(_instanceId,_channelId),
               "GetSpeechOutputLevelFullRange() => level=%u", level);
    return 0;
}

int
Channel::SetMute(bool enable)
{
    CriticalSectionScoped cs(&volume_settings_critsect_);
    VOIP_TRACE(kTraceInfo, kTraceVoice, AoEId(_instanceId,_channelId),
               "Channel::SetMute(enable=%d)", enable);
    _mute = enable;
    return 0;
}

bool
Channel::Mute() const
{
    CriticalSectionScoped cs(&volume_settings_critsect_);
    return _mute;
}

int
Channel::SetOutputVolumePan(float left, float right)
{
    CriticalSectionScoped cs(&volume_settings_critsect_);
    VOIP_TRACE(kTraceInfo, kTraceVoice, AoEId(_instanceId,_channelId),
               "Channel::SetOutputVolumePan()");
    _panLeft = left;
    _panRight = right;
    return 0;
}

int
Channel::GetOutputVolumePan(float& left, float& right) const
{
    CriticalSectionScoped cs(&volume_settings_critsect_);
    left = _panLeft;
    right = _panRight;
    VOIP_TRACE(kTraceStateInfo, kTraceVoice,
               AoEId(_instanceId,_channelId),
               "GetOutputVolumePan() => left=%3.2f, right=%3.2f", left, right);
    return 0;
}

int
Channel::SetChannelOutputVolumeScaling(float scaling)
{
    CriticalSectionScoped cs(&volume_settings_critsect_);
    VOIP_TRACE(kTraceInfo, kTraceVoice, AoEId(_instanceId,_channelId),
               "Channel::SetChannelOutputVolumeScaling()");
    _outputGain = scaling;
    return 0;
}

int
Channel::GetChannelOutputVolumeScaling(float& scaling) const
{
    CriticalSectionScoped cs(&volume_settings_critsect_);
    scaling = _outputGain;
    VOIP_TRACE(kTraceStateInfo, kTraceVoice,
               AoEId(_instanceId,_channelId),
               "GetChannelOutputVolumeScaling() => scaling=%3.2f", scaling);
    return 0;
}


int
Channel::UpdateRxVadDetection(AudioFrame& audioFrame)
{
    VOIP_TRACE(kTraceStream, kTraceVoice, AoEId(_instanceId,_channelId),
                 "Channel::UpdateRxVadDetection()");

    int vadDecision = 1;

    vadDecision = (audioFrame.vad_activity_ == AudioFrame::kVadActive)? 1 : 0;

    if ((vadDecision != _oldVadDecision) && _rxVadObserverPtr)
    {
        OnRxVadDetected(vadDecision);
        _oldVadDecision = vadDecision;
    }

    VOIP_TRACE(kTraceStream, kTraceVoice, AoEId(_instanceId,_channelId),
                 "Channel::UpdateRxVadDetection() => vadDecision=%d",
                 vadDecision);
    return 0;
}

int
Channel::RegisterRxVadObserver(AoERxVadCallback &observer)
{
    VOIP_TRACE(kTraceInfo, kTraceVoice, AoEId(_instanceId,_channelId),
                 "Channel::RegisterRxVadObserver()");
    CriticalSectionScoped cs(&_callbackCritSect);

    if (_rxVadObserverPtr)
    {
        _engineStatisticsPtr->SetLastError(
            VE_INVALID_OPERATION, kTraceError,
            "RegisterRxVadObserver() observer already enabled");
        return -1;
    }
    _rxVadObserverPtr = &observer;
    _RxVadDetection = true;
    return 0;
}

int
Channel::DeRegisterRxVadObserver()
{
    VOIP_TRACE(kTraceInfo, kTraceVoice, AoEId(_instanceId,_channelId),
                 "Channel::DeRegisterRxVadObserver()");
    CriticalSectionScoped cs(&_callbackCritSect);

    if (!_rxVadObserverPtr)
    {
        _engineStatisticsPtr->SetLastError(
            VE_INVALID_OPERATION, kTraceWarning,
            "DeRegisterRxVadObserver() observer already disabled");
        return 0;
    }
    _rxVadObserverPtr = NULL;
    _RxVadDetection = false;
    return 0;
}

int
Channel::VoiceActivityIndicator(int &activity)
{
    activity = _sendFrameType;

    VOIP_TRACE(kTraceInfo, kTraceVoice, AoEId(_instanceId,_channelId),
                 "Channel::VoiceActivityIndicator(indicator=%d)", activity);
    return 0;
}

#ifdef VOIP_AUDIO_ENGINE_AGC

int
Channel::SetRxAgcStatus(bool enable, AgcModes mode)
{
    VOIP_TRACE(kTraceInfo, kTraceVoice, AoEId(_instanceId,_channelId),
                 "Channel::SetRxAgcStatus(enable=%d, mode=%d)",
                 (int)enable, (int)mode);

    GainControl::Mode agcMode = kDefaultRxAgcMode;
    switch (mode)
    {
        case kAgcDefault:
            break;
        case kAgcUnchanged:
            agcMode = rx_audioproc_->gain_control()->mode();
            break;
        case kAgcFixedDigital:
            agcMode = GainControl::kFixedDigital;
            break;
        case kAgcAdaptiveDigital:
            agcMode =GainControl::kAdaptiveDigital;
            break;
        default:
            _engineStatisticsPtr->SetLastError(
                VE_INVALID_ARGUMENT, kTraceError,
                "SetRxAgcStatus() invalid Agc mode");
            return -1;
    }

    if (rx_audioproc_->gain_control()->set_mode(agcMode) != 0)
    {
        _engineStatisticsPtr->SetLastError(
            VE_APM_ERROR, kTraceError,
            "SetRxAgcStatus() failed to set Agc mode");
        return -1;
    }
    if (rx_audioproc_->gain_control()->Enable(enable) != 0)
    {
        _engineStatisticsPtr->SetLastError(
            VE_APM_ERROR, kTraceError,
            "SetRxAgcStatus() failed to set Agc state");
        return -1;
    }

    _rxAgcIsEnabled = enable;
    channel_state_.SetRxApmIsEnabled(_rxAgcIsEnabled || _rxNsIsEnabled);

    return 0;
}

int
Channel::GetRxAgcStatus(bool& enabled, AgcModes& mode)
{
    VOIP_TRACE(kTraceInfo, kTraceVoice, AoEId(_instanceId,_channelId),
                     "Channel::GetRxAgcStatus(enable=?, mode=?)");

    bool enable = rx_audioproc_->gain_control()->is_enabled();
    GainControl::Mode agcMode =
        rx_audioproc_->gain_control()->mode();

    enabled = enable;

    switch (agcMode)
    {
        case GainControl::kFixedDigital:
            mode = kAgcFixedDigital;
            break;
        case GainControl::kAdaptiveDigital:
            mode = kAgcAdaptiveDigital;
            break;
        default:
            _engineStatisticsPtr->SetLastError(
                VE_APM_ERROR, kTraceError,
                "GetRxAgcStatus() invalid Agc mode");
            return -1;
    }

    return 0;
}

int
Channel::SetRxAgcConfig(AgcConfig config)
{
    VOIP_TRACE(kTraceInfo, kTraceVoice, AoEId(_instanceId,_channelId),
                 "Channel::SetRxAgcConfig()");

    if (rx_audioproc_->gain_control()->set_target_level_dbfs(
        config.targetLeveldBOv) != 0)
    {
        _engineStatisticsPtr->SetLastError(
            VE_APM_ERROR, kTraceError,
            "SetRxAgcConfig() failed to set target peak |level|"
            "(or envelope) of the Agc");
        return -1;
    }
    if (rx_audioproc_->gain_control()->set_compression_gain_db(
        config.digitalCompressionGaindB) != 0)
    {
        _engineStatisticsPtr->SetLastError(
            VE_APM_ERROR, kTraceError,
            "SetRxAgcConfig() failed to set the range in |gain| the"
            " digital compression stage may apply");
        return -1;
    }
    if (rx_audioproc_->gain_control()->enable_limiter(
        config.limiterEnable) != 0)
    {
        _engineStatisticsPtr->SetLastError(
            VE_APM_ERROR, kTraceError,
            "SetRxAgcConfig() failed to set hard limiter to the signal");
        return -1;
    }

    return 0;
}

int
Channel::GetRxAgcConfig(AgcConfig& config)
{
    VOIP_TRACE(kTraceInfo, kTraceVoice, AoEId(_instanceId,_channelId),
                 "Channel::GetRxAgcConfig(config=%?)");

    config.targetLeveldBOv =
        rx_audioproc_->gain_control()->target_level_dbfs();
    config.digitalCompressionGaindB =
        rx_audioproc_->gain_control()->compression_gain_db();
    config.limiterEnable =
        rx_audioproc_->gain_control()->is_limiter_enabled();

    VOIP_TRACE(kTraceStateInfo, kTraceVoice,
               AoEId(_instanceId,_channelId), "GetRxAgcConfig() => "
                   "targetLeveldBOv=%u, digitalCompressionGaindB=%u,"
                   " limiterEnable=%d",
                   config.targetLeveldBOv,
                   config.digitalCompressionGaindB,
                   config.limiterEnable);

    return 0;
}

#endif // #ifdef VOIP_AUDIO_ENGINE_AGC

#ifdef VOIP_AUDIO_ENGINE_NR

int
Channel::SetRxNsStatus(bool enable, NsModes mode)
{
    VOIP_TRACE(kTraceInfo, kTraceVoice, AoEId(_instanceId,_channelId),
                 "Channel::SetRxNsStatus(enable=%d, mode=%d)",
                 (int)enable, (int)mode);

    NoiseSuppression::Level nsLevel = kDefaultNsMode;
    switch (mode)
    {

        case kNsDefault:
            break;
        case kNsUnchanged:
            nsLevel = rx_audioproc_->noise_suppression()->level();
            break;
        case kNsConference:
            nsLevel = NoiseSuppression::kHigh;
            break;
        case kNsLowSuppression:
            nsLevel = NoiseSuppression::kLow;
            break;
        case kNsModerateSuppression:
            nsLevel = NoiseSuppression::kModerate;
            break;
        case kNsHighSuppression:
            nsLevel = NoiseSuppression::kHigh;
            break;
        case kNsVeryHighSuppression:
            nsLevel = NoiseSuppression::kVeryHigh;
            break;
    }

    if (rx_audioproc_->noise_suppression()->set_level(nsLevel)
        != 0)
    {
        _engineStatisticsPtr->SetLastError(
            VE_APM_ERROR, kTraceError,
            "SetRxNsStatus() failed to set NS level");
        return -1;
    }
    if (rx_audioproc_->noise_suppression()->Enable(enable) != 0)
    {
        _engineStatisticsPtr->SetLastError(
            VE_APM_ERROR, kTraceError,
            "SetRxNsStatus() failed to set NS state");
        return -1;
    }

    _rxNsIsEnabled = enable;
    channel_state_.SetRxApmIsEnabled(_rxAgcIsEnabled || _rxNsIsEnabled);

    return 0;
}

int
Channel::GetRxNsStatus(bool& enabled, NsModes& mode)
{
    VOIP_TRACE(kTraceInfo, kTraceVoice, AoEId(_instanceId,_channelId),
                 "Channel::GetRxNsStatus(enable=?, mode=?)");

    bool enable =
        rx_audioproc_->noise_suppression()->is_enabled();
    NoiseSuppression::Level ncLevel =
        rx_audioproc_->noise_suppression()->level();

    enabled = enable;

    switch (ncLevel)
    {
        case NoiseSuppression::kLow:
            mode = kNsLowSuppression;
            break;
        case NoiseSuppression::kModerate:
            mode = kNsModerateSuppression;
            break;
        case NoiseSuppression::kHigh:
            mode = kNsHighSuppression;
            break;
        case NoiseSuppression::kVeryHigh:
            mode = kNsVeryHighSuppression;
            break;
    }

    VOIP_TRACE(kTraceStateInfo, kTraceVoice,
               AoEId(_instanceId,_channelId),
               "GetRxNsStatus() => enabled=%d, mode=%d", enabled, mode);
    return 0;
}

#endif // #ifdef VOIP_AUDIO_ENGINE_NR

uint32_t
Channel::Demultiplex(const AudioFrame& audioFrame)
{
    VOIP_TRACE(kTraceStream, kTraceVoice, AoEId(_instanceId,_channelId),
                 "Channel::Demultiplex()");
    _recAudioFrame.CopyFrom(audioFrame);
    _recAudioFrame.id_ = _channelId;
    return 0;
}


uint32_t
Channel::PrepareEncodeAndSend(int mixingFrequency)
{
    VOIP_TRACE(kTraceStream, kTraceVoice, AoEId(_instanceId,_channelId),
                 "Channel::PrepareEncodeAndSend()");

    if (_recAudioFrame.samples_per_channel_ == 0)
    {
        VOIP_TRACE(kTraceWarning, kTraceVoice, AoEId(_instanceId,_channelId),
                     "Channel::PrepareEncodeAndSend() invalid audio frame");
        return -1;
    }

    if (channel_state_.Get().input_file_playing)
    {
        MixOrReplaceAudioWithFile(mixingFrequency);
    }

    if (Mute())
    {
        AudioFrameOperations::Mute(_recAudioFrame);
    }

    if (channel_state_.Get().input_external_media)
    {
        CriticalSectionScoped cs(&_callbackCritSect);
        const bool isStereo = (_recAudioFrame.num_channels_ == 2);
        if (_inputExternalMediaCallbackPtr)
        {
            _inputExternalMediaCallbackPtr->Process(
                _channelId,
                kRecordingPerChannel,
               (int16_t*)_recAudioFrame.data_,
                _recAudioFrame.samples_per_channel_,
                _recAudioFrame.sample_rate_hz_,
                isStereo);
        }
    }

    return 0;
}

uint32_t
Channel::EncodeAndSend()
{
    VOIP_TRACE(kTraceStream, kTraceVoice, AoEId(_instanceId,_channelId),
                 "Channel::EncodeAndSend()");

    assert(_recAudioFrame.num_channels_ <= 2);
    if (_recAudioFrame.samples_per_channel_ == 0)
    {
        VOIP_TRACE(kTraceWarning, kTraceVoice, AoEId(_instanceId,_channelId),
                     "Channel::EncodeAndSend() invalid audio frame");
        return -1;
    }

    _recAudioFrame.id_ = _channelId;

	_recAudioFrame.timestamp_ = _timeStamp;
	_recAudioFrame.energy_ /= 2;
	
	if (Process10msFrame((AudioFrame&)_recAudioFrame) != 0)
	{
		VOIP_TRACE(kTraceError, kTraceVoice, AoEId(_instanceId,_channelId),
			"Channel::EncodeAndSend() ACM process failed");
		return -1;
	}
	rx_audioproc_->gain_control()->Enable(true);
	rx_audioproc_->noise_suppression()->Enable(true);
	rx_audioproc_->noise_suppression()->set_level(VoIP::NoiseSuppression::kVeryHigh);
	rx_audioproc_->high_pass_filter()->Enable(true);
 	rx_audioproc_->ProcessStream(&_recAudioFrame);
    _timeStamp += _recAudioFrame.samples_per_channel_;
	return SendRecPacket(_recAudioFrame.id_, _recAudioFrame);
   
}

int Channel::RegisterExternalMediaProcessing(
    ProcessingTypes type,
    AoEMediaProcess& processObject)
{
    VOIP_TRACE(kTraceInfo, kTraceVoice, AoEId(_instanceId,_channelId),
                 "Channel::RegisterExternalMediaProcessing()");

    CriticalSectionScoped cs(&_callbackCritSect);

    if (kPlaybackPerChannel == type)
    {
        if (_outputExternalMediaCallbackPtr)
        {
            _engineStatisticsPtr->SetLastError(
                VE_INVALID_OPERATION, kTraceError,
                "Channel::RegisterExternalMediaProcessing() "
                "output external media already enabled");
            return -1;
        }
        _outputExternalMediaCallbackPtr = &processObject;
        _outputExternalMedia = true;
    }
    else if (kRecordingPerChannel == type)
    {
        if (_inputExternalMediaCallbackPtr)
        {
            _engineStatisticsPtr->SetLastError(
                VE_INVALID_OPERATION, kTraceError,
                "Channel::RegisterExternalMediaProcessing() "
                "output external media already enabled");
            return -1;
        }
        _inputExternalMediaCallbackPtr = &processObject;
        channel_state_.SetInputExternalMedia(true);
    }
    return 0;
}

int Channel::DeRegisterExternalMediaProcessing(ProcessingTypes type)
{
    VOIP_TRACE(kTraceInfo, kTraceVoice, AoEId(_instanceId,_channelId),
                 "Channel::DeRegisterExternalMediaProcessing()");

    CriticalSectionScoped cs(&_callbackCritSect);

    if (kPlaybackPerChannel == type)
    {
        if (!_outputExternalMediaCallbackPtr)
        {
            _engineStatisticsPtr->SetLastError(
                VE_INVALID_OPERATION, kTraceWarning,
                "Channel::DeRegisterExternalMediaProcessing() "
                "output external media already disabled");
            return 0;
        }
        _outputExternalMedia = false;
        _outputExternalMediaCallbackPtr = NULL;
    }
    else if (kRecordingPerChannel == type)
    {
        if (!_inputExternalMediaCallbackPtr)
        {
            _engineStatisticsPtr->SetLastError(
                VE_INVALID_OPERATION, kTraceWarning,
                "Channel::DeRegisterExternalMediaProcessing() "
                "input external media already disabled");
            return 0;
        }
        channel_state_.SetInputExternalMedia(false);
        _inputExternalMediaCallbackPtr = NULL;
    }

    return 0;
}

int Channel::SetExternalMixing(bool enabled) {
    VOIP_TRACE(kTraceInfo, kTraceVoice, AoEId(_instanceId,_channelId),
                 "Channel::SetExternalMixing(enabled=%d)", enabled);

    if (channel_state_.Get().playing)
    {
        _engineStatisticsPtr->SetLastError(
            VE_INVALID_OPERATION, kTraceError,
            "Channel::SetExternalMixing() "
            "external mixing cannot be changed while playing.");
        return -1;
    }

    _externalMixing = enabled;

    return 0;
}

int32_t
Channel::MixOrReplaceAudioWithFile(int mixingFrequency)
{
    scoped_array<int16_t> fileBuffer(new int16_t[640]);
    int fileSamples(0);

    {
        CriticalSectionScoped cs(&_fileCritSect);

        if (_inputFilePlayerPtr == NULL)
        {
            VOIP_TRACE(kTraceWarning, kTraceVoice,
                         AoEId(_instanceId, _channelId),
                         "Channel::MixOrReplaceAudioWithFile() fileplayer"
                             " doesnt exist");
            return -1;
        }

        if (_inputFilePlayerPtr->Get10msAudioFromFile(fileBuffer.get(),
                                                      fileSamples,
                                                      mixingFrequency) == -1)
        {
            VOIP_TRACE(kTraceWarning, kTraceVoice,
                         AoEId(_instanceId, _channelId),
                         "Channel::MixOrReplaceAudioWithFile() file mixing "
                         "failed");
            return -1;
        }
        if (fileSamples == 0)
        {
            VOIP_TRACE(kTraceWarning, kTraceVoice,
                         AoEId(_instanceId, _channelId),
                         "Channel::MixOrReplaceAudioWithFile() file is ended");
            return 0;
        }
    }

    assert(_recAudioFrame.samples_per_channel_ == fileSamples);

    if (_mixFileWithMicrophone)
    {
        // Currently file stream is always mono.
        // TODO(xians): Change the code when FilePlayer supports real stereo.
        MixWithSat(_recAudioFrame.data_,
                   _recAudioFrame.num_channels_,
                   fileBuffer.get(),
                   1,
                   fileSamples);
    }
    else
    {
        // Replace ACM audio with file.
        // Currently file stream is always mono.
        // TODO(xians): Change the code when FilePlayer supports real stereo.
        _recAudioFrame.UpdateFrame(_channelId,
                                -1,
                                fileBuffer.get(),
                                fileSamples,
                                mixingFrequency,
                                AudioFrame::kNormalSpeech,
                                AudioFrame::kVadUnknown,
                                1);

    }
    return 0;
}

int32_t
Channel::MixAudioWithFile(AudioFrame& audioFrame,
                          int mixingFrequency)
{
    assert(mixingFrequency <= 32000);

    scoped_array<int16_t> fileBuffer(new int16_t[640]);
    int fileSamples(0);

    {
        CriticalSectionScoped cs(&_fileCritSect);

        if (_outputFilePlayerPtr == NULL)
        {
            VOIP_TRACE(kTraceWarning, kTraceVoice,
                         AoEId(_instanceId, _channelId),
                         "Channel::MixAudioWithFile() file mixing failed");
            return -1;
        }

        // We should get the frequency we ask for.
        if (_outputFilePlayerPtr->Get10msAudioFromFile(fileBuffer.get(),
                                                       fileSamples,
                                                       mixingFrequency) == -1)
        {
            VOIP_TRACE(kTraceWarning, kTraceVoice,
                         AoEId(_instanceId, _channelId),
                         "Channel::MixAudioWithFile() file mixing failed");
            return -1;
        }
    }

    if (audioFrame.samples_per_channel_ == fileSamples)
    {
        // Currently file stream is always mono.
        // TODO(xians): Change the code when FilePlayer supports real stereo.
        MixWithSat(audioFrame.data_,
                   audioFrame.num_channels_,
                   fileBuffer.get(),
                   1,
                   fileSamples);
    }
    else
    {
        VOIP_TRACE(kTraceWarning, kTraceVoice, AoEId(_instanceId,_channelId),
            "Channel::MixAudioWithFile() samples_per_channel_(%d) != "
            "fileSamples(%d)",
            audioFrame.samples_per_channel_, fileSamples);
        return -1;
    }

    return 0;
}

int32_t Channel::ReceivedPlayoutData(AudioFrame &frame )
{
	CriticalSectionScoped cs(&_callbackCritSect);
	VOIP_TRACE(kTraceInfo, kTraceVoice, AoEId(_instanceId,_channelId),
		"Channel::ReceivedPlayoutData()");

	_playoutAudioFrame.CopyFrom(frame);

	return 0;
}

int Channel::SendRecPacket( int channel, AudioFrame & frame )
{
	//channel = AoEChannelId(channel);
	assert(channel == _channelId);

	VOIP_TRACE(kTraceStream, kTraceVoice, AoEId(_instanceId,_channelId),
		"Channel::SendPacket(channel=%d)", frame.id_);

	CriticalSectionScoped cs(&_callbackCritSect);

	if (_transportPtr == NULL)
	{
		VOIP_TRACE(kTraceError, kTraceVoice, AoEId(_instanceId,_channelId),
			"Channel::SendPacket() failed to send audio packet due to"
			" invalid transport object");
		return -1;
	}


	if (_transportPtr->SendRecPacket(channel, frame) != 0) {
		std::string transport_name = "transport SendRecPacket";          
		VOIP_TRACE(kTraceError, kTraceVoice,
			AoEId(_instanceId,_channelId),
			"Channel::SendPacket() transmission using %s failed",
			transport_name.c_str());
		return -1;
	}

	return 0;
}

int Channel::Process10msFrame( AudioFrame& frame )
{
	if (frame.samples_per_channel_ <= 0) {
		assert(false);
		VOIP_TRACE(VoIP::kTraceError, VoIP::kTraceVoice, AoEId(_instanceId,_channelId),
			"Cannot Add 10 ms audio, payload length is negative or "
			"zero");
		return -1;
	}

	if (frame.sample_rate_hz_ > 48000) {
		assert(false);
		VOIP_TRACE(VoIP::kTraceError, VoIP::kTraceVoice, AoEId(_instanceId,_channelId),
			"Cannot Add 10 ms audio, input frequency not valid");
		return -1;
	}

	// If the length and frequency matches. We currently just support raw PCM.
	if ((frame.sample_rate_hz_ / 100)
		!= frame.samples_per_channel_) {
			VOIP_TRACE(VoIP::kTraceError, VoIP::kTraceVoice, AoEId(_instanceId,_channelId),
				"Cannot Add 10 ms audio, input frequency and length doesn't"
				" match");
			return -1;
	}

	if (frame.num_channels_ != 1 && frame.num_channels_ != 2) {
		VOIP_TRACE(VoIP::kTraceError, VoIP::kTraceAudioCoding, AoEId(_instanceId,_channelId),
			"Cannot Add 10 ms audio, invalid number of channels.");
		return -1;
	}

	CriticalSectionScoped lock(&_frameProcessCritSect);

	const AudioFrame* ptr_frame;
	// Perform a resampling, also down-mix if it is required and can be
	// performed before resampling (a down mix prior to resampling will take
	// place if both primary and secondary encoders are mono and input is in
	// stereo).
	if (PreprocessToAddData(frame, &ptr_frame) < 0) {
		return -1;
	}

	// If a re-mix is required (up or down), this buffer will store re-mixed
	// version of the input.
	int16_t buffer[VOIP_10MS_PCM_AUDIO];
	if (ptr_frame->num_channels_ == 1) {
		if (UpMix(*ptr_frame, VOIP_10MS_PCM_AUDIO, buffer) < 0)
			return -1;
	} else {
		if (DownMix(*ptr_frame, VOIP_10MS_PCM_AUDIO, buffer) < 0)
			return -1;
	}
	_recAudioFrame.CopyFrom(*ptr_frame);
	return 0;
}


int Channel::PreprocessToAddData(const AudioFrame& in_frame,const AudioFrame** ptr_out)
{
		bool resample = static_cast<int32_t>(in_frame.sample_rate_hz_) != 8000;

		// This variable is true if primary codec and secondary codec (if exists)
		// are both mono and input is stereo.
		bool down_mix;

		down_mix = (in_frame.num_channels_ == 2) &&true;

		if (!down_mix && !resample) {
// 			// No pre-processing is required.
// 			last_in_timestamp_ = in_frame.timestamp_;
// 			last_timestamp_ = in_frame.timestamp_;
			*ptr_out = &in_frame;
			return 0;
		}

		*ptr_out = &preprocess_frame_;
		preprocess_frame_.num_channels_ = in_frame.num_channels_;
		int16_t audio[VOIP_10MS_PCM_AUDIO];
		const int16_t* src_ptr_audio = in_frame.data_;
		int16_t* dest_ptr_audio = preprocess_frame_.data_;
		if (down_mix) {
			// If a resampling is required the output of a down-mix is written into a
			// local buffer, otherwise, it will be written to the output frame.
			if (resample)
				dest_ptr_audio = audio;
			if (DownMix(in_frame, VOIP_10MS_PCM_AUDIO, dest_ptr_audio) < 0)
				return -1;
			preprocess_frame_.num_channels_ = 1;
			// Set the input of the resampler is the down-mixed signal.
			src_ptr_audio = audio;
		}

		preprocess_frame_.timestamp_ = in_frame.timestamp_;
		preprocess_frame_.samples_per_channel_ = in_frame.samples_per_channel_;
		preprocess_frame_.sample_rate_hz_ = in_frame.sample_rate_hz_;
		// If it is required, we have to do a resampling.
		if (resample) {
			// The result of the resampler is written to output frame.
			dest_ptr_audio = preprocess_frame_.data_;

			uint32_t timestamp_diff;

			// Calculate the timestamp of this frame.
			if (last_in_timestamp_ > in_frame.timestamp_) {
				// A wrap around has happened.
				timestamp_diff = (static_cast<uint32_t>(0xFFFFFFFF) - last_in_timestamp_)
					+ in_frame.timestamp_;
			} else {
				timestamp_diff = in_frame.timestamp_ - last_in_timestamp_;
			}
			preprocess_frame_.timestamp_ = last_timestamp_ +
				static_cast<uint32_t>(timestamp_diff *
				(static_cast<double>(8000) /
				static_cast<double>(in_frame.sample_rate_hz_)));

 			preprocess_frame_.samples_per_channel_ = 80;//input_acm_resampler_.Resample10Msec(
// 				src_ptr_audio, in_frame.sample_rate_hz_, dest_ptr_audio,
// 				8000, preprocess_frame_.num_channels_);

			if (preprocess_frame_.samples_per_channel_ < 0) {
				VOIP_TRACE(VoIP::kTraceError, VoIP::kTraceAudioCoding, AoEId(_instanceId,_channelId),
					"Cannot add 10 ms audio, resampling failed");
				return -1;
			}
			preprocess_frame_.sample_rate_hz_ = 8000;
		}
		last_in_timestamp_ = in_frame.timestamp_;
		last_timestamp_ = preprocess_frame_.timestamp_;

		return 0;
}

// Stereo-to-mono can be used as in-place.
int DownMix(const AudioFrame& frame, int length_out_buff, int16_t* out_buff) {
	if (length_out_buff < frame.samples_per_channel_) {
		return -1;
	}
	for (int n = 0; n < frame.samples_per_channel_; ++n)
		out_buff[n] = (frame.data_[2 * n] + frame.data_[2 * n + 1]) >> 1;
	return 0;
}

// Mono-to-stereo can be used as in-place.
int UpMix(const AudioFrame& frame, int length_out_buff, int16_t* out_buff) {
	if (length_out_buff < frame.samples_per_channel_) {
		return -1;
	}
	for (int n = frame.samples_per_channel_ - 1; n >= 0; --n) {
		out_buff[2 * n + 1] = frame.data_[n];
		out_buff[2 * n] = frame.data_[n];
	}
	return 0;
}


}  // namespace aoe
}  // namespace VoIP
