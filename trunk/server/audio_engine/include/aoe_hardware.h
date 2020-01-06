// This sub-API supports the following functionalities:
//
//  - Audio device handling.
//  - Device information.
//  - CPU load monitoring.
//
// Usage example, omitting error checking:
//
//  using namespace VoIP;
//  AudioEngine* aoe = AudioEngine::Create();
//  AoEBase* base = AoEBase::GetInterface(aoe);
//  AoEHardware* hardware  = AoEHardware::GetInterface(aoe);
//  base->Init();
//  ...
//  int n_devices = hardware->GetNumOfPlayoutDevices();
//  ...
//  base->Terminate();
//  base->Release();
//  hardware->Release();
//  AudioEngine::Delete(aoe);
//
#ifndef VOIP_AUDIO_ENGINE_HARDWARE_H
#define VOIP_AUDIO_ENGINE_HARDWARE_H

#include "audio_engine/include/common_types.h"
#include "audio_engine/include/hardware.h"
namespace VoIP {

class AudioEngine;

class VOIP_DLLEXPORT AoEHardware
{
public:
    // Factory for the AoEHardware sub-API. Increases an internal
    // reference counter if successful. Returns NULL if the API is not
    // supported or if construction fails.
    static AoEHardware* GetInterface(AudioEngine* audioEngine);

    // Releases the AoEHardware sub-API and decreases an internal
    // reference counter. Returns the new reference count. This value should
    // be zero for all sub-API:s before the AudioEngine object can be safely
    // deleted.
    virtual int Release() = 0;

    // Gets the number of audio devices available for recording.
    virtual int GetNumOfRecordingDevices(int& devices) = 0;

    // Gets the number of audio devices available for playout.
    virtual int GetNumOfPlayoutDevices(int& devices) = 0;

    // Gets the name of a specific recording device given by an |index|.
    // On Windows Vista/7, it also retrieves an additional unique ID
    // (GUID) for the recording device.
    virtual int GetRecordingDeviceName(int index, char strNameUTF8[128],
                                       char strGuidUTF8[128]) = 0;

    // Gets the name of a specific playout device given by an |index|.
    // On Windows Vista/7, it also retrieves an additional unique ID
    // (GUID) for the playout device.
    virtual int GetPlayoutDeviceName(int index, char strNameUTF8[128],
                                     char strGuidUTF8[128]) = 0;

    // Checks if the sound card is available to be opened for recording.
    virtual int GetRecordingDeviceStatus(bool& isAvailable) = 0;

    // Checks if the sound card is available to be opened for playout.
    virtual int GetPlayoutDeviceStatus(bool& isAvailable) = 0;

    // Sets the audio device used for recording.
    virtual int SetRecordingDevice(
        int index, StereoChannel recordingChannel = kStereoBoth) = 0;

    // Sets the audio device used for playout.
    virtual int SetPlayoutDevice(int index) = 0;

    // Sets the type of audio device layer to use.
    virtual int SetAudioDeviceLayer(AudioLayers audioLayer) = 0;

    // Gets the currently used (active) audio device layer.
    virtual int GetAudioDeviceLayer(AudioLayers& audioLayer) = 0;

    // Gets the AudioEngine's current CPU consumption in terms of the percent
    // of total CPU availability. [Windows only]
    virtual int GetCPULoad(int& loadPercent) = 0;

    // Not supported
    virtual int ResetAudioDevice() = 0;

    // Native sample rate controls (samples/sec)
    virtual int SetRecordingSampleRate(unsigned int samples_per_sec) = 0;
    virtual int RecordingSampleRate(unsigned int* samples_per_sec) const = 0;
    virtual int SetPlayoutSampleRate(unsigned int samples_per_sec) = 0;
    virtual int PlayoutSampleRate(unsigned int* samples_per_sec) const = 0;

    // *Experimental - not recommended for use.*
    // Enables the Windows Core Audio built-in AEC. Fails on other platforms.
    //
    // Currently incompatible with the standard AoE AEC and AGC; don't attempt
    // to enable them while this is active.
    //
    // Must be called before AoEBase::StartSend(). When enabled:
    // 1. AoEBase::StartPlayout() must be called before AoEBase::StartSend().
    // 2. AoEBase::StopSend() should be called before AoEBase::StopPlayout().
    //    The reverse order may cause garbage audio to be rendered or the
    //    capture side to halt until StopSend() is called.
    //
    //    As a consequence, SetPlayoutDevice() should be used with caution
    //    during a call. It will function, but may cause the above issues for
    //    the duration it takes to complete. (In practice, it should complete
    //    fast enough to avoid audible degradation).
    virtual int EnableBuiltInAEC(bool enable) = 0;
    virtual bool BuiltInAECIsEnabled() const = 0;

protected:
    AoEHardware() {}
    virtual ~AoEHardware() {}
};

}  // namespace VoIP

#endif  //  VOIP_AUDIO_ENGINE_HARDWARE_H
