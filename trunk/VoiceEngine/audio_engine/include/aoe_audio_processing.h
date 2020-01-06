// This sub-API supports the following functionalities:
//
//  - Noise Suppression (NS).
//  - Automatic Gain Control (AGC).
//  - Echo Control (EC).
//  - Receiving side VAD, NS and AGC.
//  - Measurements of instantaneous speech, noise and echo levels.
//  - Generation of AP debug recordings.
//  - Detection of keyboard typing which can disrupt a voice conversation.
//
// Usage example, omitting error checking:
//
//  using namespace VoIP;
//  AudioEngine* Aoe = AudioEngine::Create();
//  AoEBase* base = AoEBase::GetInterface(aoe);
//  AoEAudioProcessing* ap = AoEAudioProcessing::GetInterface(aoe);
//  base->Init();
//  ap->SetEcStatus(true, kAgcAdaptiveAnalog);
//  ...
//  base->Terminate();
//  base->Release();
//  ap->Release();
//  AudioEngine::Delete(aoe);
//
#ifndef VOIP_AUDIO_ENGINE_AUDIO_PROCESSING_H
#define VOIP_AUDIO_ENGINE_AUDIO_PROCESSING_H

#include <stdio.h>

#include "audio_engine/include/common_types.h"

namespace VoIP {

class AudioEngine;

// AoERxVadCallback
class VOIP_DLLEXPORT AoERxVadCallback
{
public:
    virtual void OnRxVad(int channel, int vadDecision) = 0;

protected:
    virtual ~AoERxVadCallback() {}
};

// AoEAudioProcessing
class VOIP_DLLEXPORT AoEAudioProcessing
{
public:
    // Factory for the AoEAudioProcessing sub-API. Increases an internal
    // reference counter if successful. Returns NULL if the API is not
    // supported or if construction fails.
    static AoEAudioProcessing* GetInterface(AudioEngine* audioEngine);

    // Releases the AoEAudioProcessing sub-API and decreases an internal
    // reference counter. Returns the new reference count. This value should
    // be zero for all sub-API:s before the AudioEngine object can be safely
    // deleted.
    virtual int Release() = 0;

    // Sets Noise Suppression (NS) status and mode.
    // The NS reduces noise in the microphone signal.
    virtual int SetNsStatus(bool enable, NsModes mode = kNsUnchanged) = 0;

    // Gets the NS status and mode.
    virtual int GetNsStatus(bool& enabled, NsModes& mode) = 0;

    // Sets the Automatic Gain Control (AGC) status and mode.
    // The AGC adjusts the microphone signal to an appropriate level.
    virtual int SetAgcStatus(bool enable, AgcModes mode = kAgcUnchanged) = 0;

    // Gets the AGC status and mode.
    virtual int GetAgcStatus(bool& enabled, AgcModes& mode) = 0;

    // Sets the AGC configuration.
    // Should only be used in situations where the working environment
    // is well known.
    virtual int SetAgcConfig(AgcConfig config) = 0;

    // Gets the AGC configuration.
    virtual int GetAgcConfig(AgcConfig& config) = 0;

    // Sets the Echo Control (EC) status and mode.
    // The EC mitigates acoustic echo where a user can hear their own
    // speech repeated back due to an acoustic coupling between the
    // speaker and the microphone at the remote end.
    virtual int SetEcStatus(bool enable, EcModes mode = kEcUnchanged) = 0;

    // Gets the EC status and mode.
    virtual int GetEcStatus(bool& enabled, EcModes& mode) = 0;

    // Enables the compensation of clock drift between the capture and render
    // streams by the echo canceller (i.e. only using EcMode==kEcAec). It will
    // only be enabled if supported on the current platform; otherwise an error
    // will be returned. Check if the platform is supported by calling
    // |DriftCompensationSupported()|.
    virtual int EnableDriftCompensation(bool enable) = 0;
    virtual bool DriftCompensationEnabled() = 0;
    static bool DriftCompensationSupported();

    // Sets a delay |offset| in ms to add to the system delay reported by the
    // OS, which is used by the AEC to synchronize far- and near-end streams.
    // In some cases a system may introduce a delay which goes unreported by the
    // OS, but which is known to the user. This method can be used to compensate
    // for the unreported delay.
    virtual void SetDelayOffsetMs(int offset) = 0;
    virtual int DelayOffsetMs() = 0;


    // Sets status and mode of the receiving-side (Rx) NS.
    // The Rx NS reduces noise in the received signal for the specified
    // |channel|. Intended for advanced usage only.
    virtual int SetRxNsStatus(int channel,
                              bool enable,
                              NsModes mode = kNsUnchanged) = 0;

    // Gets status and mode of the receiving-side NS.
    virtual int GetRxNsStatus(int channel,
                              bool& enabled,
                              NsModes& mode) = 0;

    // Sets status and mode of the receiving-side (Rx) AGC.
    // The Rx AGC adjusts the received signal to an appropriate level
    // for the specified |channel|. Intended for advanced usage only.
    virtual int SetRxAgcStatus(int channel,
                               bool enable,
                               AgcModes mode = kAgcUnchanged) = 0;

    // Gets status and mode of the receiving-side AGC.
    virtual int GetRxAgcStatus(int channel,
                               bool& enabled,
                               AgcModes& mode) = 0;

    // Modifies the AGC configuration on the receiving side for the
    // specified |channel|.
    virtual int SetRxAgcConfig(int channel, AgcConfig config) = 0;

    // Gets the AGC configuration on the receiving side.
    virtual int GetRxAgcConfig(int channel, AgcConfig& config) = 0;

    // Registers a AoERxVadCallback |observer| instance and enables Rx VAD
    // notifications for the specified |channel|.
    virtual int RegisterRxVadObserver(int channel,
                                      AoERxVadCallback &observer) = 0;

    // Deregisters the AoERxVadCallback |observer| and disables Rx VAD
    // notifications for the specified |channel|.
    virtual int DeRegisterRxVadObserver(int channel) = 0;

    // Gets the VAD/DTX activity for the specified |channel|.
    // The returned value is 1 if frames of audio contains speech
    // and 0 if silence. The output is always 1 if VAD is disabled.
    virtual int VoiceActivityIndicator(int channel) = 0;

    // Enables or disables the possibility to retrieve echo metrics and delay
    // logging values during an active call. The metrics are only supported in
    // AEC.
    virtual int SetEcMetricsStatus(bool enable) = 0;

    // Gets the current EC metric status.
    virtual int GetEcMetricsStatus(bool& enabled) = 0;

    // Gets the instantaneous echo level metrics.
    virtual int GetEchoMetrics(int& ERL, int& ERLE, int& RERL, int& A_NLP) = 0;

    // Gets the EC internal |delay_median| and |delay_std| in ms between
    // near-end and far-end. The values are calculated over the time period
    // since the last GetEcDelayMetrics() call.
    virtual int GetEcDelayMetrics(int& delay_median, int& delay_std) = 0;

    // Enables recording of Audio Processing (AP) debugging information.
    // The file can later be used for off-line analysis of the AP performance.
    virtual int StartDebugRecording(const char* fileNameUTF8) = 0;

    // Same as above but sets and uses an existing file handle. Takes ownership
    // of |file_handle| and passes it on to the audio processing module.
    virtual int StartDebugRecording(FILE* file_handle) = 0;

    // Disables recording of AP debugging information.
    virtual int StopDebugRecording() = 0;
protected:
    AoEAudioProcessing() {}
    virtual ~AoEAudioProcessing() {}
};

}  // namespace VoIP

#endif  // VOIP_AUDIO_ENGINE_AUDIO_PROCESSING_H
