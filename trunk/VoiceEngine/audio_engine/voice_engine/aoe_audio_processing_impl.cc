#include "audio_engine/voice_engine/aoe_audio_processing_impl.h"

#include "audio_engine/modules/audio_processing/include/audio_processing.h"
#include "audio_engine/system_wrappers/interface/critical_section_wrapper.h"
#include "audio_engine/system_wrappers/interface/logging.h"
#include "audio_engine/system_wrappers/interface/trace.h"
#include "audio_engine/voice_engine/channel.h"
#include "audio_engine/voice_engine/include/aoe_errors.h"
#include "audio_engine/voice_engine/transmit_mixer.h"
#include "audio_engine/voice_engine/audio_engine_impl.h"

// TODO(andrew): move to a common place.
#define VOIP_VOICE_INIT_CHECK()                        \
  do {                                                   \
    if (!_shared->statistics().Initialized()) {          \
      _shared->SetLastError(VE_NOT_INITED, kTraceError); \
      return -1;                                         \
    }                                                    \
  } while (0)

#define VOIP_VOICE_INIT_CHECK_BOOL()                   \
  do {                                                   \
    if (!_shared->statistics().Initialized()) {          \
      _shared->SetLastError(VE_NOT_INITED, kTraceError); \
      return false;                                      \
    }                                                    \
  } while (0)

namespace VoIP {

#if defined(VOIP_ANDROID) || defined(VOIP_IOS)
static const EcModes kDefaultEcMode = kEcAecm;
#else
static const EcModes kDefaultEcMode = kEcAec;
#endif

AoEAudioProcessing* AoEAudioProcessing::GetInterface(AudioEngine* audioEngine) {
#ifndef VOIP_AUDIO_ENGINE_AUDIO_PROCESSING_API
  return NULL;
#else
  if (NULL == audioEngine) {
    return NULL;
  }
  AudioEngineImpl* s = static_cast<AudioEngineImpl*>(audioEngine);
  s->AddRef();
  return s;
#endif
}

#ifdef VOIP_AUDIO_ENGINE_AUDIO_PROCESSING_API
AoEAudioProcessingImpl::AoEAudioProcessingImpl(aoe::SharedData* shared)
    : _isAecMode(kDefaultEcMode == kEcAec),
      _shared(shared) {
  VOIP_TRACE(kTraceMemory, kTraceVoice, AoEId(_shared->instance_id(), -1),
               "AoEAudioProcessingImpl::AoEAudioProcessingImpl() - ctor");
}

AoEAudioProcessingImpl::~AoEAudioProcessingImpl() {
  VOIP_TRACE(kTraceMemory, kTraceVoice, AoEId(_shared->instance_id(), -1),
               "AoEAudioProcessingImpl::~AoEAudioProcessingImpl() - dtor");
}

int AoEAudioProcessingImpl::SetNsStatus(bool enable, NsModes mode) {
  VOIP_TRACE(kTraceApiCall, kTraceVoice, AoEId(_shared->instance_id(), -1),
               "SetNsStatus(enable=%d, mode=%d)", enable, mode);
#ifdef VOIP_AUDIO_ENGINE_NR
  if (!_shared->statistics().Initialized()) {
    _shared->SetLastError(VE_NOT_INITED, kTraceError);
    return -1;
  }

  NoiseSuppression::Level nsLevel = kDefaultNsMode;
  switch (mode) {
    case kNsDefault:
      nsLevel = kDefaultNsMode;
      break;
    case kNsUnchanged:
      nsLevel = _shared->audio_processing()->noise_suppression()->level();
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

  if (_shared->audio_processing()->noise_suppression()->
          set_level(nsLevel) != 0) {
    _shared->SetLastError(VE_APM_ERROR, kTraceError,
        "SetNsStatus() failed to set Ns mode");
    return -1;
  }
  if (_shared->audio_processing()->noise_suppression()->Enable(enable) != 0) {
    _shared->SetLastError(VE_APM_ERROR, kTraceError,
        "SetNsStatus() failed to set Ns state");
    return -1;
  }

  return 0;
#else
  _shared->SetLastError(VE_FUNC_NOT_SUPPORTED, kTraceError,
      "SetNsStatus() Ns is not supported");
  return -1;
#endif
}

int AoEAudioProcessingImpl::GetNsStatus(bool& enabled, NsModes& mode) {
  VOIP_TRACE(kTraceApiCall, kTraceVoice, AoEId(_shared->instance_id(), -1),
               "GetNsStatus(enabled=?, mode=?)");
#ifdef VOIP_AUDIO_ENGINE_NR
  if (!_shared->statistics().Initialized()) {
    _shared->SetLastError(VE_NOT_INITED, kTraceError);
    return -1;
  }

  enabled = _shared->audio_processing()->noise_suppression()->is_enabled();
  NoiseSuppression::Level nsLevel =
      _shared->audio_processing()->noise_suppression()->level();

  switch (nsLevel) {
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

  VOIP_TRACE(kTraceStateInfo, kTraceVoice, AoEId(_shared->instance_id(), -1),
               "GetNsStatus() => enabled=% d, mode=%d", enabled, mode);
  return 0;
#else
  _shared->SetLastError(VE_FUNC_NOT_SUPPORTED, kTraceError,
      "GetNsStatus() Ns is not supported");
  return -1;
#endif
}

int AoEAudioProcessingImpl::SetAgcStatus(bool enable, AgcModes mode) {
  VOIP_TRACE(kTraceApiCall, kTraceVoice, AoEId(_shared->instance_id(), -1),
               "SetAgcStatus(enable=%d, mode=%d)", enable, mode);
#ifdef VOIP_AUDIO_ENGINE_AGC
  if (!_shared->statistics().Initialized()) {
    _shared->SetLastError(VE_NOT_INITED, kTraceError);
    return -1;
  }

#if defined(VOIP_IOS) || defined(ATA) || defined(VOIP_ANDROID)
  if (mode == kAgcAdaptiveAnalog) {
    _shared->SetLastError(VE_INVALID_ARGUMENT, kTraceError,
        "SetAgcStatus() invalid Agc mode for mobile device");
    return -1;
  }
#endif

  GainControl::Mode agcMode = kDefaultAgcMode;
  switch (mode) {
    case kAgcDefault:
      agcMode = kDefaultAgcMode;
      break;
    case kAgcUnchanged:
      agcMode = _shared->audio_processing()->gain_control()->mode();
      break;
    case kAgcFixedDigital:
      agcMode = GainControl::kFixedDigital;
      break;
    case kAgcAdaptiveAnalog:
      agcMode = GainControl::kAdaptiveAnalog;
      break;
    case kAgcAdaptiveDigital:
      agcMode = GainControl::kAdaptiveDigital;
      break;
  }

  if (_shared->audio_processing()->gain_control()->set_mode(agcMode) != 0) {
    _shared->SetLastError(VE_APM_ERROR, kTraceError,
        "SetAgcStatus() failed to set Agc mode");
    return -1;
  }
  if (_shared->audio_processing()->gain_control()->Enable(enable) != 0) {
    _shared->SetLastError(VE_APM_ERROR, kTraceError,
        "SetAgcStatus() failed to set Agc state");
    return -1;
  }

  if (agcMode != GainControl::kFixedDigital) {
    // Set Agc state in the ADM when adaptive Agc mode has been selected.
    // Note that we also enable the ADM Agc when Adaptive Digital mode is
    // used since we want to be able to provide the APM with updated mic
    // levels when the user modifies the mic level manually.
    if (_shared->audio_device()->SetAGC(enable) != 0) {
      _shared->SetLastError(VE_AUDIO_DEVICE_MODULE_ERROR,
          kTraceWarning, "SetAgcStatus() failed to set Agc mode");
    }
  }

  return 0;
#else
  _shared->SetLastError(VE_FUNC_NOT_SUPPORTED, kTraceError,
      "SetAgcStatus() Agc is not supported");
  return -1;
#endif
}

int AoEAudioProcessingImpl::GetAgcStatus(bool& enabled, AgcModes& mode) {
  VOIP_TRACE(kTraceApiCall, kTraceVoice, AoEId(_shared->instance_id(), -1),
               "GetAgcStatus(enabled=?, mode=?)");
#ifdef VOIP_AUDIO_ENGINE_AGC
  if (!_shared->statistics().Initialized()) {
    _shared->SetLastError(VE_NOT_INITED, kTraceError);
    return -1;
  }

  enabled = _shared->audio_processing()->gain_control()->is_enabled();
  GainControl::Mode agcMode =
    _shared->audio_processing()->gain_control()->mode();

  switch (agcMode) {
    case GainControl::kFixedDigital:
      mode = kAgcFixedDigital;
      break;
    case GainControl::kAdaptiveAnalog:
      mode = kAgcAdaptiveAnalog;
      break;
    case GainControl::kAdaptiveDigital:
      mode = kAgcAdaptiveDigital;
      break;
  }

  VOIP_TRACE(kTraceStateInfo, kTraceVoice, AoEId(_shared->instance_id(), -1),
               "GetAgcStatus() => enabled=%d, mode=%d", enabled, mode);
  return 0;
#else
  _shared->SetLastError(VE_FUNC_NOT_SUPPORTED, kTraceError,
      "GetAgcStatus() Agc is not supported");
  return -1;
#endif
}

int AoEAudioProcessingImpl::SetAgcConfig(AgcConfig config) {
  VOIP_TRACE(kTraceApiCall, kTraceVoice, AoEId(_shared->instance_id(), -1),
               "SetAgcConfig()");
#ifdef VOIP_AUDIO_ENGINE_AGC
  if (!_shared->statistics().Initialized()) {
    _shared->SetLastError(VE_NOT_INITED, kTraceError);
    return -1;
  }

  if (_shared->audio_processing()->gain_control()->set_target_level_dbfs(
      config.targetLeveldBOv) != 0) {
    _shared->SetLastError(VE_APM_ERROR, kTraceError,
        "SetAgcConfig() failed to set target peak |level|"
        " (or envelope) of the Agc");
    return -1;
  }
  if (_shared->audio_processing()->gain_control()->set_compression_gain_db(
        config.digitalCompressionGaindB) != 0) {
    _shared->SetLastError(VE_APM_ERROR, kTraceError,
        "SetAgcConfig() failed to set the range in |gain| "
        "the digital compression stage may apply");
    return -1;
  }
  if (_shared->audio_processing()->gain_control()->enable_limiter(
        config.limiterEnable) != 0) {
    _shared->SetLastError(VE_APM_ERROR, kTraceError,
        "SetAgcConfig() failed to set hard limiter to the signal");
    return -1;
  }

  return 0;
#else
  _shared->SetLastError(VE_FUNC_NOT_SUPPORTED, kTraceError,
      "SetAgcConfig() EC is not supported");
  return -1;
#endif
}

int AoEAudioProcessingImpl::GetAgcConfig(AgcConfig& config) {
  VOIP_TRACE(kTraceApiCall, kTraceVoice, AoEId(_shared->instance_id(), -1),
               "GetAgcConfig(config=?)");
#ifdef VOIP_AUDIO_ENGINE_AGC
  if (!_shared->statistics().Initialized()) {
    _shared->SetLastError(VE_NOT_INITED, kTraceError);
    return -1;
  }

  config.targetLeveldBOv =
    _shared->audio_processing()->gain_control()->target_level_dbfs();
  config.digitalCompressionGaindB =
    _shared->audio_processing()->gain_control()->compression_gain_db();
  config.limiterEnable =
    _shared->audio_processing()->gain_control()->is_limiter_enabled();

  VOIP_TRACE(kTraceStateInfo, kTraceVoice, AoEId(_shared->instance_id(), -1),
               "GetAgcConfig() => targetLeveldBOv=%u, "
                  "digitalCompressionGaindB=%u, limiterEnable=%d",
               config.targetLeveldBOv,
               config.digitalCompressionGaindB,
               config.limiterEnable);

  return 0;
#else
  _shared->SetLastError(VE_FUNC_NOT_SUPPORTED, kTraceError,
      "GetAgcConfig() EC is not supported");
  return -1;
#endif
}

int AoEAudioProcessingImpl::SetRxNsStatus(int channel,
                                          bool enable,
                                          NsModes mode) {
  LOG_API3(channel, enable, mode);
#ifdef VOIP_AUDIO_ENGINE_NR
  if (!_shared->statistics().Initialized()) {
    _shared->SetLastError(VE_NOT_INITED, kTraceError);
    return -1;
  }

  aoe::ChannelOwner ch = _shared->channel_manager().GetChannel(channel);
  aoe::Channel* channelPtr = ch.channel();
  if (channelPtr == NULL) {
    _shared->SetLastError(VE_CHANNEL_NOT_VALID, kTraceError,
        "SetRxNsStatus() failed to locate channel");
    return -1;
  }
  return channelPtr->SetRxNsStatus(enable, mode);
#else
  _shared->SetLastError(VE_FUNC_NOT_SUPPORTED, kTraceError,
      "SetRxNsStatus() NS is not supported");
  return -1;
#endif
}

int AoEAudioProcessingImpl::GetRxNsStatus(int channel,
                                          bool& enabled,
                                          NsModes& mode) {
  VOIP_TRACE(kTraceApiCall, kTraceVoice, AoEId(_shared->instance_id(), -1),
               "GetRxNsStatus(channel=%d, enable=?, mode=?)", channel);
#ifdef VOIP_AUDIO_ENGINE_NR
  if (!_shared->statistics().Initialized()) {
    _shared->SetLastError(VE_NOT_INITED, kTraceError);
    return -1;
  }

  aoe::ChannelOwner ch = _shared->channel_manager().GetChannel(channel);
  aoe::Channel* channelPtr = ch.channel();
  if (channelPtr == NULL) {
    _shared->SetLastError(VE_CHANNEL_NOT_VALID, kTraceError,
        "GetRxNsStatus() failed to locate channel");
    return -1;
  }
  return channelPtr->GetRxNsStatus(enabled, mode);
#else
  _shared->SetLastError(VE_FUNC_NOT_SUPPORTED, kTraceError,
      "GetRxNsStatus() NS is not supported");
  return -1;
#endif
}

int AoEAudioProcessingImpl::SetRxAgcStatus(int channel,
                                           bool enable,
                                           AgcModes mode) {
  VOIP_TRACE(kTraceApiCall, kTraceVoice, AoEId(_shared->instance_id(), -1),
               "SetRxAgcStatus(channel=%d, enable=%d, mode=%d)",
               channel, (int)enable, (int)mode);
#ifdef VOIP_AUDIO_ENGINE_AGC
  if (!_shared->statistics().Initialized()) {
    _shared->SetLastError(VE_NOT_INITED, kTraceError);
    return -1;
  }

  aoe::ChannelOwner ch = _shared->channel_manager().GetChannel(channel);
  aoe::Channel* channelPtr = ch.channel();
  if (channelPtr == NULL) {
    _shared->SetLastError(VE_CHANNEL_NOT_VALID, kTraceError,
        "SetRxAgcStatus() failed to locate channel");
    return -1;
  }
  return channelPtr->SetRxAgcStatus(enable, mode);
#else
  _shared->SetLastError(VE_FUNC_NOT_SUPPORTED, kTraceError,
      "SetRxAgcStatus() Agc is not supported");
  return -1;
#endif
}

int AoEAudioProcessingImpl::GetRxAgcStatus(int channel,
                                           bool& enabled,
                                           AgcModes& mode) {
  VOIP_TRACE(kTraceApiCall, kTraceVoice, AoEId(_shared->instance_id(), -1),
               "GetRxAgcStatus(channel=%d, enable=?, mode=?)", channel);
#ifdef VOIP_AUDIO_ENGINE_AGC
  if (!_shared->statistics().Initialized()) {
    _shared->SetLastError(VE_NOT_INITED, kTraceError);
    return -1;
  }

  aoe::ChannelOwner ch = _shared->channel_manager().GetChannel(channel);
  aoe::Channel* channelPtr = ch.channel();
  if (channelPtr == NULL) {
    _shared->SetLastError(VE_CHANNEL_NOT_VALID, kTraceError,
        "GetRxAgcStatus() failed to locate channel");
    return -1;
  }
  return channelPtr->GetRxAgcStatus(enabled, mode);
#else
  _shared->SetLastError(VE_FUNC_NOT_SUPPORTED, kTraceError,
      "GetRxAgcStatus() Agc is not supported");
  return -1;
#endif
}

int AoEAudioProcessingImpl::SetRxAgcConfig(int channel,
                                           AgcConfig config) {
  VOIP_TRACE(kTraceApiCall, kTraceVoice, AoEId(_shared->instance_id(), -1),
               "SetRxAgcConfig(channel=%d)", channel);
#ifdef VOIP_AUDIO_ENGINE_AGC
  if (!_shared->statistics().Initialized()) {
    _shared->SetLastError(VE_NOT_INITED, kTraceError);
    return -1;
  }

  aoe::ChannelOwner ch = _shared->channel_manager().GetChannel(channel);
  aoe::Channel* channelPtr = ch.channel();
  if (channelPtr == NULL) {
    _shared->SetLastError(VE_CHANNEL_NOT_VALID, kTraceError,
      "SetRxAgcConfig() failed to locate channel");
    return -1;
  }
  return channelPtr->SetRxAgcConfig(config);
#else
  _shared->SetLastError(VE_FUNC_NOT_SUPPORTED, kTraceError,
      "SetRxAgcConfig() Agc is not supported");
  return -1;
#endif
}

int AoEAudioProcessingImpl::GetRxAgcConfig(int channel, AgcConfig& config) {
  VOIP_TRACE(kTraceApiCall, kTraceVoice, AoEId(_shared->instance_id(), -1),
               "GetRxAgcConfig(channel=%d)", channel);
#ifdef VOIP_AUDIO_ENGINE_AGC
  if (!_shared->statistics().Initialized()) {
    _shared->SetLastError(VE_NOT_INITED, kTraceError);
    return -1;
  }

  aoe::ChannelOwner ch = _shared->channel_manager().GetChannel(channel);
  aoe::Channel* channelPtr = ch.channel();
  if (channelPtr == NULL) {
    _shared->SetLastError(VE_CHANNEL_NOT_VALID, kTraceError,
        "GetRxAgcConfig() failed to locate channel");
    return -1;
  }
  return channelPtr->GetRxAgcConfig(config);
#else
  _shared->SetLastError(VE_FUNC_NOT_SUPPORTED, kTraceError,
      "GetRxAgcConfig() Agc is not supported");
  return -1;
#endif
}

bool AoEAudioProcessing::DriftCompensationSupported() {
#if defined(VOIP_DRIFT_COMPENSATION_SUPPORTED)
  return true;
#else
  return false;
#endif
}

int AoEAudioProcessingImpl::EnableDriftCompensation(bool enable) {
  LOG_API1(enable);
  VOIP_VOICE_INIT_CHECK();

  if (!DriftCompensationSupported()) {
    _shared->SetLastError(VE_APM_ERROR, kTraceWarning,
        "Drift compensation is not supported on this platform.");
    return -1;
  }

  EchoCancellation* aec = _shared->audio_processing()->echo_cancellation();
  if (aec->enable_drift_compensation(enable) != 0) {
    _shared->SetLastError(VE_APM_ERROR, kTraceError,
        "aec->enable_drift_compensation() failed");
    return -1;
  }
  return 0;
}

bool AoEAudioProcessingImpl::DriftCompensationEnabled() {
  LOG_API0();
  VOIP_VOICE_INIT_CHECK_BOOL();

  EchoCancellation* aec = _shared->audio_processing()->echo_cancellation();
  return aec->is_drift_compensation_enabled();
}

int AoEAudioProcessingImpl::SetEcStatus(bool enable, EcModes mode) {
  VOIP_TRACE(kTraceApiCall, kTraceVoice, AoEId(_shared->instance_id(), -1),
               "SetEcStatus(enable=%d, mode=%d)", enable, mode);
#ifdef VOIP_AUDIO_ENGINE_ECHO
  if (!_shared->statistics().Initialized()) {
    _shared->SetLastError(VE_NOT_INITED, kTraceError);
    return -1;
  }

  // AEC mode
  if ((mode == kEcDefault) ||
      (mode == kEcConference) ||
      (mode == kEcAec) ||
      ((mode == kEcUnchanged) &&
       (_isAecMode == true))) {
    if (enable) {
      // Disable the AECM before enable the AEC
      if (_shared->audio_processing()->echo_control_mobile()->is_enabled()) {
        _shared->SetLastError(VE_APM_ERROR, kTraceWarning,
            "SetEcStatus() disable AECM before enabling AEC");
        if (_shared->audio_processing()->echo_control_mobile()->
            Enable(false) != 0) {
          _shared->SetLastError(VE_APM_ERROR, kTraceError,
              "SetEcStatus() failed to disable AECM");
          return -1;
        }
      }
    }
    if (_shared->audio_processing()->echo_cancellation()->Enable(enable) != 0) {
      _shared->SetLastError(VE_APM_ERROR, kTraceError,
          "SetEcStatus() failed to set AEC state");
      return -1;
    }
    if (mode == kEcConference) {
      if (_shared->audio_processing()->echo_cancellation()->
          set_suppression_level(EchoCancellation::kHighSuppression) != 0) {
        _shared->SetLastError(VE_APM_ERROR, kTraceError,
            "SetEcStatus() failed to set aggressiveness to high");
        return -1;
      }
    } else {
      if (_shared->audio_processing()->echo_cancellation()->
          set_suppression_level(
            EchoCancellation::kModerateSuppression) != 0) {
        _shared->SetLastError(VE_APM_ERROR, kTraceError,
            "SetEcStatus() failed to set aggressiveness to moderate");
        return -1;
      }
    }

    _isAecMode = true;
  } else if ((mode == kEcAecm) ||
             ((mode == kEcUnchanged) &&
              (_isAecMode == false))) {
    if (enable) {
      // Disable the AEC before enable the AECM
      if (_shared->audio_processing()->echo_cancellation()->is_enabled()) {
        _shared->SetLastError(VE_APM_ERROR, kTraceWarning,
            "SetEcStatus() disable AEC before enabling AECM");
        if (_shared->audio_processing()->echo_cancellation()->
            Enable(false) != 0) {
          _shared->SetLastError(VE_APM_ERROR, kTraceError,
              "SetEcStatus() failed to disable AEC");
          return -1;
        }
      }
    }
    if (_shared->audio_processing()->echo_control_mobile()->
        Enable(enable) != 0) {
      _shared->SetLastError(VE_APM_ERROR, kTraceError,
          "SetEcStatus() failed to set AECM state");
      return -1;
    }
    _isAecMode = false;
  } else {
    _shared->SetLastError(VE_INVALID_ARGUMENT, kTraceError,
                                   "SetEcStatus() invalid EC mode");
    return -1;
  }

  return 0;
#else
  _shared->SetLastError(VE_FUNC_NOT_SUPPORTED, kTraceError,
      "SetEcStatus() EC is not supported");
  return -1;
#endif
}

int AoEAudioProcessingImpl::GetEcStatus(bool& enabled, EcModes& mode) {
  VOIP_TRACE(kTraceApiCall, kTraceVoice, AoEId(_shared->instance_id(), -1),
               "GetEcStatus()");
#ifdef VOIP_AUDIO_ENGINE_ECHO
  if (!_shared->statistics().Initialized()) {
    _shared->SetLastError(VE_NOT_INITED, kTraceError);
    return -1;
  }

  if (_isAecMode == true) {
    mode = kEcAec;
    enabled = _shared->audio_processing()->echo_cancellation()->is_enabled();
  } else {
    mode = kEcAecm;
    enabled = _shared->audio_processing()->echo_control_mobile()->
              is_enabled();
  }

  VOIP_TRACE(kTraceStateInfo, kTraceVoice, AoEId(_shared->instance_id(), -1),
               "GetEcStatus() => enabled=%i, mode=%i",
               enabled, (int)mode);
  return 0;
#else
  _shared->SetLastError(VE_FUNC_NOT_SUPPORTED, kTraceError,
      "GetEcStatus() EC is not supported");
  return -1;
#endif
}

void AoEAudioProcessingImpl::SetDelayOffsetMs(int offset) {
  VOIP_TRACE(kTraceApiCall, kTraceVoice, AoEId(_shared->instance_id(), -1),
               "SetDelayOffsetMs(offset = %d)", offset);
  _shared->audio_processing()->set_delay_offset_ms(offset);
}

int AoEAudioProcessingImpl::DelayOffsetMs() {
  VOIP_TRACE(kTraceApiCall, kTraceVoice, AoEId(_shared->instance_id(), -1),
               "DelayOffsetMs()");
  return _shared->audio_processing()->delay_offset_ms();
}

int AoEAudioProcessingImpl::SetAecmMode(AecmModes mode, bool enableCNG) {
  VOIP_TRACE(kTraceApiCall, kTraceVoice, AoEId(_shared->instance_id(), -1),
               "SetAECMMode(mode = %d)", mode);
#ifdef VOIP_AUDIO_ENGINE_ECHO
  if (!_shared->statistics().Initialized()) {
    _shared->SetLastError(VE_NOT_INITED, kTraceError);
    return -1;
  }

  EchoControlMobile::RoutingMode aecmMode(
      EchoControlMobile::kQuietEarpieceOrHeadset);

  switch (mode) {
    case kAecmQuietEarpieceOrHeadset:
      aecmMode = EchoControlMobile::kQuietEarpieceOrHeadset;
      break;
    case kAecmEarpiece:
      aecmMode = EchoControlMobile::kEarpiece;
      break;
    case kAecmLoudEarpiece:
      aecmMode = EchoControlMobile::kLoudEarpiece;
      break;
    case kAecmSpeakerphone:
      aecmMode = EchoControlMobile::kSpeakerphone;
      break;
    case kAecmLoudSpeakerphone:
      aecmMode = EchoControlMobile::kLoudSpeakerphone;
      break;
  }


  if (_shared->audio_processing()->echo_control_mobile()->
      set_routing_mode(aecmMode) != 0) {
    _shared->SetLastError(VE_APM_ERROR, kTraceError,
        "SetAECMMode() failed to set AECM routing mode");
    return -1;
  }
  if (_shared->audio_processing()->echo_control_mobile()->
      enable_comfort_noise(enableCNG) != 0) {
    _shared->SetLastError(VE_APM_ERROR, kTraceError,
        "SetAECMMode() failed to set comfort noise state for AECM");
    return -1;
  }

  return 0;
#else
  _shared->SetLastError(VE_FUNC_NOT_SUPPORTED, kTraceError,
      "SetAECMMode() EC is not supported");
  return -1;
#endif
}

int AoEAudioProcessingImpl::GetAecmMode(AecmModes& mode, bool& enabledCNG) {
  VOIP_TRACE(kTraceApiCall, kTraceVoice, AoEId(_shared->instance_id(), -1),
               "GetAECMMode(mode=?)");
#ifdef VOIP_AUDIO_ENGINE_ECHO
  if (!_shared->statistics().Initialized()) {
    _shared->SetLastError(VE_NOT_INITED, kTraceError);
    return -1;
  }

  enabledCNG = false;

  EchoControlMobile::RoutingMode aecmMode =
      _shared->audio_processing()->echo_control_mobile()->routing_mode();
  enabledCNG = _shared->audio_processing()->echo_control_mobile()->
      is_comfort_noise_enabled();

  switch (aecmMode) {
    case EchoControlMobile::kQuietEarpieceOrHeadset:
      mode = kAecmQuietEarpieceOrHeadset;
      break;
    case EchoControlMobile::kEarpiece:
      mode = kAecmEarpiece;
      break;
    case EchoControlMobile::kLoudEarpiece:
      mode = kAecmLoudEarpiece;
      break;
    case EchoControlMobile::kSpeakerphone:
      mode = kAecmSpeakerphone;
      break;
    case EchoControlMobile::kLoudSpeakerphone:
      mode = kAecmLoudSpeakerphone;
      break;
  }

  return 0;
#else
  _shared->SetLastError(VE_FUNC_NOT_SUPPORTED, kTraceError,
      "GetAECMMode() EC is not supported");
  return -1;
#endif
}

int AoEAudioProcessingImpl::EnableHighPassFilter(bool enable) {
  VOIP_TRACE(kTraceApiCall, kTraceVoice, AoEId(_shared->instance_id(), -1),
               "EnableHighPassFilter(%d)", enable);
  if (_shared->audio_processing()->high_pass_filter()->Enable(enable) !=
      AudioProcessing::kNoError) {
    _shared->SetLastError(VE_APM_ERROR, kTraceError,
        "HighPassFilter::Enable() failed.");
    return -1;
  }

  return 0;
}

bool AoEAudioProcessingImpl::IsHighPassFilterEnabled() {
  VOIP_TRACE(kTraceApiCall, kTraceVoice, AoEId(_shared->instance_id(), -1),
               "IsHighPassFilterEnabled()");
  return _shared->audio_processing()->high_pass_filter()->is_enabled();
}

int AoEAudioProcessingImpl::RegisterRxVadObserver(
  int channel,
  AoERxVadCallback& observer) {
  VOIP_TRACE(kTraceApiCall, kTraceVoice, AoEId(_shared->instance_id(), -1),
               "RegisterRxVadObserver()");
  if (!_shared->statistics().Initialized()) {
    _shared->SetLastError(VE_NOT_INITED, kTraceError);
    return -1;
  }
  aoe::ChannelOwner ch = _shared->channel_manager().GetChannel(channel);
  aoe::Channel* channelPtr = ch.channel();
  if (channelPtr == NULL) {
    _shared->SetLastError(VE_CHANNEL_NOT_VALID, kTraceError,
        "RegisterRxVadObserver() failed to locate channel");
    return -1;
  }
  return channelPtr->RegisterRxVadObserver(observer);
}

int AoEAudioProcessingImpl::DeRegisterRxVadObserver(int channel) {
  VOIP_TRACE(kTraceApiCall, kTraceVoice, AoEId(_shared->instance_id(), -1),
               "DeRegisterRxVadObserver()");
  if (!_shared->statistics().Initialized()) {
    _shared->SetLastError(VE_NOT_INITED, kTraceError);
    return -1;
  }
  aoe::ChannelOwner ch = _shared->channel_manager().GetChannel(channel);
  aoe::Channel* channelPtr = ch.channel();
  if (channelPtr == NULL) {
    _shared->SetLastError(VE_CHANNEL_NOT_VALID, kTraceError,
        "DeRegisterRxVadObserver() failed to locate channel");
    return -1;
  }

  return channelPtr->DeRegisterRxVadObserver();
}

int AoEAudioProcessingImpl::VoiceActivityIndicator(int channel) {
  VOIP_TRACE(kTraceApiCall, kTraceVoice, AoEId(_shared->instance_id(), -1),
               "VoiceActivityIndicator(channel=%d)", channel);
  if (!_shared->statistics().Initialized()) {
    _shared->SetLastError(VE_NOT_INITED, kTraceError);
    return -1;
  }

  aoe::ChannelOwner ch = _shared->channel_manager().GetChannel(channel);
  aoe::Channel* channelPtr = ch.channel();
  if (channelPtr == NULL) {
    _shared->SetLastError(VE_CHANNEL_NOT_VALID, kTraceError,
        "DeRegisterRxVadObserver() failed to locate channel");
    return -1;
  }
  int activity(-1);
  channelPtr->VoiceActivityIndicator(activity);

  return activity;
}

int AoEAudioProcessingImpl::SetEcMetricsStatus(bool enable) {
  VOIP_TRACE(kTraceApiCall, kTraceVoice, AoEId(_shared->instance_id(), -1),
               "SetEcMetricsStatus(enable=%d)", enable);
#ifdef VOIP_AUDIO_ENGINE_ECHO
  if (!_shared->statistics().Initialized()) {
    _shared->SetLastError(VE_NOT_INITED, kTraceError);
    return -1;
  }

  if ((_shared->audio_processing()->echo_cancellation()->enable_metrics(enable)
       != 0) ||
      (_shared->audio_processing()->echo_cancellation()->enable_delay_logging(
         enable) != 0)) {
    _shared->SetLastError(VE_APM_ERROR, kTraceError,
        "SetEcMetricsStatus() unable to set EC metrics mode");
    return -1;
  }
  return 0;
#else
  _shared->SetLastError(VE_FUNC_NOT_SUPPORTED, kTraceError,
      "SetEcStatus() EC is not supported");
  return -1;
#endif
}

int AoEAudioProcessingImpl::GetEcMetricsStatus(bool& enabled) {
  VOIP_TRACE(kTraceApiCall, kTraceVoice, AoEId(_shared->instance_id(), -1),
               "GetEcMetricsStatus(enabled=?)");
#ifdef VOIP_AUDIO_ENGINE_ECHO
  if (!_shared->statistics().Initialized()) {
    _shared->SetLastError(VE_NOT_INITED, kTraceError);
    return -1;
  }

  bool echo_mode =
    _shared->audio_processing()->echo_cancellation()->are_metrics_enabled();
  bool delay_mode = _shared->audio_processing()->echo_cancellation()->
      is_delay_logging_enabled();

  if (echo_mode != delay_mode) {
    _shared->SetLastError(VE_APM_ERROR, kTraceError,
        "GetEcMetricsStatus() delay logging and echo mode are not the same");
    return -1;
  }

  enabled = echo_mode;

  VOIP_TRACE(kTraceStateInfo, kTraceVoice, AoEId(_shared->instance_id(), -1),
               "GetEcMetricsStatus() => enabled=%d", enabled);
  return 0;
#else
  _shared->SetLastError(VE_FUNC_NOT_SUPPORTED, kTraceError,
      "SetEcStatus() EC is not supported");
  return -1;
#endif
}

int AoEAudioProcessingImpl::GetEchoMetrics(int& ERL,
                                           int& ERLE,
                                           int& RERL,
                                           int& A_NLP) {
  VOIP_TRACE(kTraceApiCall, kTraceVoice, AoEId(_shared->instance_id(), -1),
               "GetEchoMetrics(ERL=?, ERLE=?, RERL=?, A_NLP=?)");
#ifdef VOIP_AUDIO_ENGINE_ECHO
  if (!_shared->statistics().Initialized()) {
    _shared->SetLastError(VE_NOT_INITED, kTraceError);
    return -1;
  }
  if (!_shared->audio_processing()->echo_cancellation()->is_enabled()) {
    _shared->SetLastError(VE_APM_ERROR, kTraceWarning,
        "GetEchoMetrics() AudioProcessingModule AEC is not enabled");
    return -1;
  }

  // Get Echo Metrics from Audio Processing Module.
  EchoCancellation::Metrics echoMetrics;
  if (_shared->audio_processing()->echo_cancellation()->GetMetrics(
          &echoMetrics)) {
    VOIP_TRACE(kTraceError, kTraceVoice, AoEId(_shared->instance_id(), -1),
                 "GetEchoMetrics(), AudioProcessingModule metrics error");
    return -1;
  }

  // Echo quality metrics.
  ERL = echoMetrics.echo_return_loss.instant;
  ERLE = echoMetrics.echo_return_loss_enhancement.instant;
  RERL = echoMetrics.residual_echo_return_loss.instant;
  A_NLP = echoMetrics.a_nlp.instant;

  VOIP_TRACE(kTraceStateInfo, kTraceVoice, AoEId(_shared->instance_id(), -1),
               "GetEchoMetrics() => ERL=%d, ERLE=%d, RERL=%d, A_NLP=%d",
               ERL, ERLE, RERL, A_NLP);
  return 0;
#else
  _shared->SetLastError(VE_FUNC_NOT_SUPPORTED, kTraceError,
      "SetEcStatus() EC is not supported");
  return -1;
#endif
}

int AoEAudioProcessingImpl::GetEcDelayMetrics(int& delay_median,
                                              int& delay_std) {
  VOIP_TRACE(kTraceApiCall, kTraceVoice, AoEId(_shared->instance_id(), -1),
               "GetEcDelayMetrics(median=?, std=?)");
#ifdef VOIP_AUDIO_ENGINE_ECHO
  if (!_shared->statistics().Initialized()) {
    _shared->SetLastError(VE_NOT_INITED, kTraceError);
    return -1;
  }
  if (!_shared->audio_processing()->echo_cancellation()->is_enabled()) {
    _shared->SetLastError(VE_APM_ERROR, kTraceWarning,
        "GetEcDelayMetrics() AudioProcessingModule AEC is not enabled");
    return -1;
  }

  int median = 0;
  int std = 0;
  // Get delay-logging values from Audio Processing Module.
  if (_shared->audio_processing()->echo_cancellation()->GetDelayMetrics(
        &median, &std)) {
    VOIP_TRACE(kTraceError, kTraceVoice, AoEId(_shared->instance_id(), -1),
                 "GetEcDelayMetrics(), AudioProcessingModule delay-logging "
                 "error");
    return -1;
  }

  // EC delay-logging metrics
  delay_median = median;
  delay_std = std;

  VOIP_TRACE(kTraceStateInfo, kTraceVoice, AoEId(_shared->instance_id(), -1),
               "GetEcDelayMetrics() => delay_median=%d, delay_std=%d",
               delay_median, delay_std);
  return 0;
#else
  _shared->SetLastError(VE_FUNC_NOT_SUPPORTED, kTraceError,
      "SetEcStatus() EC is not supported");
  return -1;
#endif
}

int AoEAudioProcessingImpl::StartDebugRecording(const char* fileNameUTF8) {
  VOIP_TRACE(kTraceApiCall, kTraceVoice, AoEId(_shared->instance_id(), -1),
               "StartDebugRecording()");
  if (!_shared->statistics().Initialized()) {
    _shared->SetLastError(VE_NOT_INITED, kTraceError);
    return -1;
  }

  return _shared->audio_processing()->StartDebugRecording(fileNameUTF8);
}

int AoEAudioProcessingImpl::StartDebugRecording(FILE* file_handle) {
  VOIP_TRACE(kTraceApiCall, kTraceVoice, AoEId(_shared->instance_id(), -1),
               "StartDebugRecording()");
  if (!_shared->statistics().Initialized()) {
    _shared->SetLastError(VE_NOT_INITED, kTraceError);
    return -1;
  }

  return _shared->audio_processing()->StartDebugRecording(file_handle);
}

int AoEAudioProcessingImpl::StopDebugRecording() {
  VOIP_TRACE(kTraceApiCall, kTraceVoice, AoEId(_shared->instance_id(), -1),
               "StopDebugRecording()");
  if (!_shared->statistics().Initialized()) {
    _shared->SetLastError(VE_NOT_INITED, kTraceError);
    return -1;
  }

  return _shared->audio_processing()->StopDebugRecording();
}

int AoEAudioProcessingImpl::SetTypingDetectionStatus(bool enable) {
  VOIP_TRACE(kTraceApiCall, kTraceVoice, AoEId(_shared->instance_id(), -1),
               "SetTypingDetectionStatus()");
#if !defined(VOIP_AUDIO_ENGINE_TYPING_DETECTION)
  NOT_SUPPORTED(_shared->statistics());
#else
  if (!_shared->statistics().Initialized()) {
    _shared->SetLastError(VE_NOT_INITED, kTraceError);
    return -1;
  }

  // Just use the VAD state to determine if we should enable typing detection
  // or not

  if (_shared->audio_processing()->voice_detection()->Enable(enable)) {
    _shared->SetLastError(VE_APM_ERROR, kTraceWarning,
        "SetTypingDetectionStatus() failed to set VAD state");
    return -1;
  }
  if (_shared->audio_processing()->voice_detection()->set_likelihood(
          VoiceDetection::kVeryLowLikelihood)) {
    _shared->SetLastError(VE_APM_ERROR, kTraceWarning,
        "SetTypingDetectionStatus() failed to set VAD likelihood to low");
    return -1;
  }

  return 0;
#endif
}

int AoEAudioProcessingImpl::GetTypingDetectionStatus(bool& enabled) {
  VOIP_TRACE(kTraceApiCall, kTraceVoice, AoEId(_shared->instance_id(), -1),
               "GetTypingDetectionStatus()");
  if (!_shared->statistics().Initialized()) {
    _shared->SetLastError(VE_NOT_INITED, kTraceError);
    return -1;
  }
  // Just use the VAD state to determine if we should enable typing
  // detection or not

  enabled = _shared->audio_processing()->voice_detection()->is_enabled();

  return 0;
}


int AoEAudioProcessingImpl::TimeSinceLastTyping(int &seconds) {
  VOIP_TRACE(kTraceApiCall, kTraceVoice, AoEId(_shared->instance_id(), -1),
               "TimeSinceLastTyping()");
#if !defined(VOIP_AUDIO_ENGINE_TYPING_DETECTION)
  NOT_SUPPORTED(_shared->statistics());
#else
  if (!_shared->statistics().Initialized()) {
    _shared->SetLastError(VE_NOT_INITED, kTraceError);
    return -1;
  }
  // Check if typing detection is enabled
  bool enabled = _shared->audio_processing()->voice_detection()->is_enabled();
  if (enabled)
  {
    _shared->transmit_mixer()->TimeSinceLastTyping(seconds);
    return 0;
  }
  else
  {
    _shared->SetLastError(VE_FUNC_NOT_SUPPORTED, kTraceError,
      "SetTypingDetectionStatus is not enabled");
  return -1;
  }
#endif
}

int AoEAudioProcessingImpl::SetTypingDetectionParameters(int timeWindow,
                                                         int costPerTyping,
                                                         int reportingThreshold,
                                                         int penaltyDecay,
                                                         int typeEventDelay) {
  VOIP_TRACE(kTraceApiCall, kTraceVoice, AoEId(_shared->instance_id(), -1),
               "SetTypingDetectionParameters()");
#if !defined(VOIP_AUDIO_ENGINE_TYPING_DETECTION)
  NOT_SUPPORTED(_shared->statistics());
#else
  if (!_shared->statistics().Initialized()) {
    _shared->statistics().SetLastError(VE_NOT_INITED, kTraceError);
    return -1;
  }
  return (_shared->transmit_mixer()->SetTypingDetectionParameters(timeWindow,
      costPerTyping, reportingThreshold, penaltyDecay, typeEventDelay));
#endif
}

void AoEAudioProcessingImpl::EnableStereoChannelSwapping(bool enable) {
  LOG_API1(enable);
  _shared->transmit_mixer()->EnableStereoChannelSwapping(enable);
}

bool AoEAudioProcessingImpl::IsStereoChannelSwappingEnabled() {
  LOG_API0();
  return _shared->transmit_mixer()->IsStereoChannelSwappingEnabled();
}

#endif  // #ifdef VOIP_AUDIO_ENGINE_AUDIO_PROCESSING_API

}  // namespace VoIP
