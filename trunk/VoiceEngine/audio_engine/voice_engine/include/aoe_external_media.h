// In some cases it is desirable to use an audio source or sink which may
// not be available to the AudioEngine, such as a DV camera. This sub-API
// contains functions that allow for the use of such external recording
// sources and playout sinks. It also describes how recorded data, or data
// to be played out, can be modified outside the AudioEngine.
//
// Usage example, omitting error checking:
//
//  using namespace voip;
//  AudioEngine* voe = AudioEngine::Create();
//  AoEBase* base = AoEBase::GetInterface(voe);
//  AoEMediaProcess media = AoEMediaProcess::GetInterface(voe);
//  base->Init();
//  ...
//  media->SetExternalRecordingStatus(true);
//  ...
//  base->Terminate();
//  base->Release();
//  media->Release();
//  AudioEngine::Delete(voe);
//
#ifndef VOIP_AUDIO_ENGINE_AOE_EXTERNAL_MEDIA_H
#define VOIP_AUDIO_ENGINE_AOE_EXTERNAL_MEDIA_H

#include "audio_engine/include/common_types.h"

namespace VoIP {

class AudioEngine;
class AudioFrame;

class VOIP_DLLEXPORT AoEMediaProcess
{
public:
    virtual void Process(int channel, ProcessingTypes type,
                         int16_t audio10ms[], int length,
                         int samplingFreq, bool isStereo) = 0;

protected:
    virtual ~AoEMediaProcess() {}
};

class VOIP_DLLEXPORT AoEExternalMedia
{
public:
   
    static AoEExternalMedia* GetInterface(AudioEngine* audioEngine);

  
    virtual int Release() = 0;

   
    virtual int RegisterExternalMediaProcessing(
        int channel, ProcessingTypes type, AoEMediaProcess& processObject) = 0;

   
    virtual int DeRegisterExternalMediaProcessing(
        int channel, ProcessingTypes type) = 0;

    // Toogles state of external recording.
    virtual int SetExternalRecordingStatus(bool enable) = 0;

    // Toogles state of external playout.
    virtual int SetExternalPlayoutStatus(bool enable) = 0;

    virtual int ExternalRecordingInsertData(
        const int16_t speechData10ms[], int lengthSamples,
        int samplingFreqHz, int current_delay_ms) = 0;

    virtual int ExternalPlayoutGetData(
        int16_t speechData10ms[], int samplingFreqHz,
        int current_delay_ms, int& lengthSamples) = 0;

  
    virtual int GetAudioFrame(int channel, int desired_sample_rate_hz,
                              AudioFrame* frame) = 0;

    // Sets the state of external mixing. Cannot be changed during playback.
    virtual int SetExternalMixing(int channel, bool enable) = 0;

protected:
    AoEExternalMedia() {}
    virtual ~AoEExternalMedia() {}
};

}  // namespace VoIP

#endif  //  VOIP_AUDIO_ENGINE_AOE_EXTERNAL_MEDIA_H
