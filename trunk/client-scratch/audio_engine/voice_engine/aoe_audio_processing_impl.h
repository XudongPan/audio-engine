
#ifndef VOIP_AUDIO_ENGINE_AOE_AUDIO_PROCESSING_IMPL_H
#define VOIP_AUDIO_ENGINE_AOE_AUDIO_PROCESSING_IMPL_H

#include "audio_engine/voice_engine/include/aoe_audio_processing.h"

#include "audio_engine/voice_engine/shared_data.h"

namespace VoIP {

class AoEAudioProcessingImpl : public AoEAudioProcessing {
 public:
  virtual int SetNsStatus(bool enable, NsModes mode = kNsUnchanged);

  virtual int GetNsStatus(bool& enabled, NsModes& mode);

  virtual int SetAgcStatus(bool enable, AgcModes mode = kAgcUnchanged);

  virtual int GetAgcStatus(bool& enabled, AgcModes& mode);

  virtual int SetAgcConfig(AgcConfig config);

  virtual int GetAgcConfig(AgcConfig& config);

  virtual int SetRxNsStatus(int channel,
                            bool enable,
                            NsModes mode = kNsUnchanged);

  virtual int GetRxNsStatus(int channel, bool& enabled, NsModes& mode);

  virtual int SetRxAgcStatus(int channel,
                             bool enable,
                             AgcModes mode = kAgcUnchanged);

  virtual int GetRxAgcStatus(int channel, bool& enabled, AgcModes& mode);

  virtual int SetRxAgcConfig(int channel, AgcConfig config);

  virtual int GetRxAgcConfig(int channel, AgcConfig& config);

  virtual int SetEcStatus(bool enable, EcModes mode = kEcUnchanged);
  virtual int GetEcStatus(bool& enabled, EcModes& mode);
  virtual int EnableDriftCompensation(bool enable);
  virtual bool DriftCompensationEnabled();

  virtual void SetDelayOffsetMs(int offset);
  virtual int DelayOffsetMs();

  virtual int SetAecmMode(AecmModes mode = kAecmSpeakerphone,
                          bool enableCNG = true);

  virtual int GetAecmMode(AecmModes& mode, bool& enabledCNG);

  virtual int EnableHighPassFilter(bool enable);
  virtual bool IsHighPassFilterEnabled();

  virtual int RegisterRxVadObserver(int channel,
                                    AoERxVadCallback& observer);

  virtual int DeRegisterRxVadObserver(int channel);

  virtual int VoiceActivityIndicator(int channel);

  virtual int SetEcMetricsStatus(bool enable);

  virtual int GetEcMetricsStatus(bool& enabled);

  virtual int GetEchoMetrics(int& ERL, int& ERLE, int& RERL, int& A_NLP);

  virtual int GetEcDelayMetrics(int& delay_median, int& delay_std);

  virtual int StartDebugRecording(const char* fileNameUTF8);
  virtual int StartDebugRecording(FILE* file_handle);

  virtual int StopDebugRecording();

  virtual int SetTypingDetectionStatus(bool enable);

  virtual int GetTypingDetectionStatus(bool& enabled);

  virtual int TimeSinceLastTyping(int &seconds);

  // TODO(niklase) Remove default argument as soon as libJingle is updated!
  virtual int SetTypingDetectionParameters(int timeWindow,
                                           int costPerTyping,
                                           int reportingThreshold,
                                           int penaltyDecay,
                                           int typeEventDelay = 0);

  virtual void EnableStereoChannelSwapping(bool enable);
  virtual bool IsStereoChannelSwappingEnabled();

 protected:
  AoEAudioProcessingImpl(aoe::SharedData* shared);
  virtual ~AoEAudioProcessingImpl();

 private:
  bool _isAecMode;
  aoe::SharedData* _shared;
};

}  // namespace VoIP

#endif  // VOIP_AUDIO_ENGINE_AOE_AUDIO_PROCESSING_IMPL_H
