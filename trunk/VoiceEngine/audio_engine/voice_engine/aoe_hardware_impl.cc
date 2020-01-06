
#include "audio_engine/voice_engine/aoe_hardware_impl.h"

#include <assert.h>

#include "audio_engine/system_wrappers/interface/critical_section_wrapper.h"
#include "audio_engine/system_wrappers/interface/trace.h"
#include "audio_engine/voice_engine/include/aoe_errors.h"
#include "audio_engine/voice_engine/audio_engine_impl.h"

namespace VoIP
{

AoEHardware* AoEHardware::GetInterface(AudioEngine* audioEngine)
{
#ifndef VOIP_AUDIO_ENGINE_HARDWARE_API
    return NULL;
#else
    if (NULL == audioEngine)
    {
        return NULL;
    }
    AudioEngineImpl* s = static_cast<AudioEngineImpl*>(audioEngine);
    s->AddRef();
    return s;
#endif
}

#ifdef VOIP_AUDIO_ENGINE_HARDWARE_API

AoEHardwareImpl::AoEHardwareImpl(aoe::SharedData* shared) : _shared(shared)
{
    VOIP_TRACE(kTraceMemory, kTraceVoice, AoEId(_shared->instance_id(), -1),
                 "AoEHardwareImpl() - ctor");
}

AoEHardwareImpl::~AoEHardwareImpl()
{
    VOIP_TRACE(kTraceMemory, kTraceVoice, AoEId(_shared->instance_id(), -1),
                 "~AoEHardwareImpl() - dtor");
}

int AoEHardwareImpl::SetAudioDeviceLayer(AudioLayers audioLayer)
{
    VOIP_TRACE(kTraceApiCall, kTraceVoice, AoEId(_shared->instance_id(), -1),
                 "SetAudioDeviceLayer(audioLayer=%d)", audioLayer);

    // Don't allow a change if AoE is initialized
    if (_shared->statistics().Initialized())
    {
        _shared->SetLastError(VE_ALREADY_INITED, kTraceError);
        return -1;
    }

    // Map to AudioDeviceModule::AudioLayer
    AudioDeviceModule::AudioLayer
        wantedLayer(AudioDeviceModule::kPlatformDefaultAudio);
    switch (audioLayer)
    {
        case kAudioPlatformDefault:
            // already set above
            break;
        case kAudioWindowsCore:
            wantedLayer = AudioDeviceModule::kWindowsCoreAudio;
            break;
        case kAudioWindowsWave:
            wantedLayer = AudioDeviceModule::kWindowsWaveAudio;
            break;
        case kAudioLinuxAlsa:
            wantedLayer = AudioDeviceModule::kLinuxAlsaAudio;
            break;
        case kAudioLinuxPulse:
            wantedLayer = AudioDeviceModule::kLinuxPulseAudio;
            break;
    }

    // Save the audio device layer for Init()
    _shared->set_audio_device_layer(wantedLayer);

    return 0;
}

int AoEHardwareImpl::GetAudioDeviceLayer(AudioLayers& audioLayer)
{
    VOIP_TRACE(kTraceApiCall, kTraceVoice, AoEId(_shared->instance_id(), -1),
               "GetAudioDeviceLayer(devices=?)");

    // Can always be called regardless of AoE state

    AudioDeviceModule::AudioLayer
        activeLayer(AudioDeviceModule::kPlatformDefaultAudio);

    if (_shared->audio_device())
    {
        // Get active audio layer from ADM
        if (_shared->audio_device()->ActiveAudioLayer(&activeLayer) != 0)
        {
            _shared->SetLastError(VE_UNDEFINED_SC_ERR, kTraceError,
                "  Audio Device error");
            return -1;
        }
    }
    else
    {
        // Return AoE's internal layer setting
        activeLayer = _shared->audio_device_layer();
    }

    // Map to AudioLayers
    switch (activeLayer)
    {
        case AudioDeviceModule::kPlatformDefaultAudio:
            audioLayer = kAudioPlatformDefault;
            break;
        case AudioDeviceModule::kWindowsCoreAudio:
            audioLayer = kAudioWindowsCore;
            break;
        case AudioDeviceModule::kWindowsWaveAudio:
            audioLayer = kAudioWindowsWave;
            break;
        case AudioDeviceModule::kLinuxAlsaAudio:
            audioLayer = kAudioLinuxAlsa;
            break;
        case AudioDeviceModule::kLinuxPulseAudio:
            audioLayer = kAudioLinuxPulse;
            break;
        default:
            _shared->SetLastError(VE_UNDEFINED_SC_ERR, kTraceError,
                "  unknown audio layer");
    }

    VOIP_TRACE(kTraceStateInfo, kTraceVoice,
        AoEId(_shared->instance_id(), -1),
        "  Output: audioLayer=%d", audioLayer);

    return 0;
}
int AoEHardwareImpl::GetNumOfRecordingDevices(int& devices)
{
    VOIP_TRACE(kTraceApiCall, kTraceVoice, AoEId(_shared->instance_id(), -1),
                 "GetNumOfRecordingDevices(devices=?)");

    if (!_shared->statistics().Initialized())
    {
        _shared->SetLastError(VE_NOT_INITED, kTraceError);
        return -1;
    }

    devices = static_cast<int> (_shared->audio_device()->RecordingDevices());

    VOIP_TRACE(kTraceStateInfo, kTraceVoice,
        AoEId(_shared->instance_id(), -1), "  Output: devices=%d", devices);

    return 0;
}

int AoEHardwareImpl::GetNumOfPlayoutDevices(int& devices)
{
    VOIP_TRACE(kTraceApiCall, kTraceVoice, AoEId(_shared->instance_id(), -1),
                 "GetNumOfPlayoutDevices(devices=?)");

    if (!_shared->statistics().Initialized())
    {
        _shared->SetLastError(VE_NOT_INITED, kTraceError);
        return -1;
    }

    devices = static_cast<int> (_shared->audio_device()->PlayoutDevices());

    VOIP_TRACE(kTraceStateInfo, kTraceVoice,
        AoEId(_shared->instance_id(), -1),
        "  Output: devices=%d", devices);

    return 0;
}

int AoEHardwareImpl::GetRecordingDeviceName(int index,
                                            char strNameUTF8[128],
                                            char strGuidUTF8[128])
{
    VOIP_TRACE(kTraceApiCall, kTraceVoice, AoEId(_shared->instance_id(), -1),
                 "GetRecordingDeviceName(index=%d)", index);

    if (!_shared->statistics().Initialized())
    {
        _shared->SetLastError(VE_NOT_INITED, kTraceError);
        return -1;
    }
    if (strNameUTF8 == NULL)
    {
        _shared->SetLastError(VE_INVALID_ARGUMENT, kTraceError,
            "GetRecordingDeviceName() invalid argument");
        return -1;
    }

    // Note that strGuidUTF8 is allowed to be NULL

    // Init len variable to length of supplied vectors
    const uint16_t strLen = 128;

    // Check if length has been changed in module
    assert(strLen == kAdmMaxDeviceNameSize);
    assert(strLen == kAdmMaxGuidSize);

    char name[strLen];
    char guid[strLen];

    // Get names from module
    if (_shared->audio_device()->RecordingDeviceName(index, name, guid) != 0)
    {
        _shared->SetLastError(VE_CANNOT_RETRIEVE_DEVICE_NAME, kTraceError,
            "GetRecordingDeviceName() failed to get device name");
        return -1;
    }

    // Copy to vectors supplied by user
    strncpy(strNameUTF8, name, strLen);
    VOIP_TRACE(kTraceStateInfo, kTraceVoice,
        AoEId(_shared->instance_id(), -1),
        "  Output: strNameUTF8=%s", strNameUTF8);

    if (strGuidUTF8 != NULL)
    {
        strncpy(strGuidUTF8, guid, strLen);
        VOIP_TRACE(kTraceStateInfo, kTraceVoice,
            AoEId(_shared->instance_id(), -1),
            "  Output: strGuidUTF8=%s", strGuidUTF8);
    }

    return 0;
}

int AoEHardwareImpl::GetPlayoutDeviceName(int index,
                                          char strNameUTF8[128],
                                          char strGuidUTF8[128])
{
    VOIP_TRACE(kTraceApiCall, kTraceVoice, AoEId(_shared->instance_id(), -1),
                 "GetPlayoutDeviceName(index=%d)", index);

    if (!_shared->statistics().Initialized())
    {
        _shared->SetLastError(VE_NOT_INITED, kTraceError);
        return -1;
    }
    if (strNameUTF8 == NULL)
    {
        _shared->SetLastError(VE_INVALID_ARGUMENT, kTraceError,
            "GetPlayoutDeviceName() invalid argument");
        return -1;
    }

    // Note that strGuidUTF8 is allowed to be NULL

    // Init len variable to length of supplied vectors
    const uint16_t strLen = 128;

    // Check if length has been changed in module
    assert(strLen == kAdmMaxDeviceNameSize);
    assert(strLen == kAdmMaxGuidSize);

    char name[strLen];
    char guid[strLen];

    // Get names from module
    if (_shared->audio_device()->PlayoutDeviceName(index, name, guid) != 0)
    {
        _shared->SetLastError(VE_CANNOT_RETRIEVE_DEVICE_NAME, kTraceError,
            "GetPlayoutDeviceName() failed to get device name");
        return -1;
    }

    // Copy to vectors supplied by user
    strncpy(strNameUTF8, name, strLen);
    VOIP_TRACE(kTraceStateInfo, kTraceVoice,
        AoEId(_shared->instance_id(), -1),
        "  Output: strNameUTF8=%s", strNameUTF8);

    if (strGuidUTF8 != NULL)
    {
        strncpy(strGuidUTF8, guid, strLen);
        VOIP_TRACE(kTraceStateInfo, kTraceVoice,
            AoEId(_shared->instance_id(), -1),
            "  Output: strGuidUTF8=%s", strGuidUTF8);
    }

    return 0;
}

int AoEHardwareImpl::SetRecordingDevice(int index,
                                        StereoChannel recordingChannel)
{
    VOIP_TRACE(kTraceApiCall, kTraceVoice, AoEId(_shared->instance_id(), -1),
                 "SetRecordingDevice(index=%d, recordingChannel=%d)",
                 index, (int) recordingChannel);
    CriticalSectionScoped cs(_shared->crit_sec());

    if (!_shared->statistics().Initialized())
    {
        _shared->SetLastError(VE_NOT_INITED, kTraceError);
        return -1;
    }

    bool isRecording(false);

    // Store state about activated recording to be able to restore it after the
    // recording device has been modified.
    if (_shared->audio_device()->Recording())
    {
        VOIP_TRACE(kTraceInfo, kTraceVoice, AoEId(_shared->instance_id(), -1),
                     "SetRecordingDevice() device is modified while recording"
                     " is active...");
        isRecording = true;
        if (_shared->audio_device()->StopRecording() == -1)
        {
            _shared->SetLastError(VE_AUDIO_DEVICE_MODULE_ERROR, kTraceError,
                "SetRecordingDevice() unable to stop recording");
            return -1;
        }
    }

    // We let the module do the index sanity

    // Set recording channel
    AudioDeviceModule::ChannelType recCh =
        AudioDeviceModule::kChannelBoth;
    switch (recordingChannel)
    {
        case kStereoLeft:
            recCh = AudioDeviceModule::kChannelLeft;
            break;
        case kStereoRight:
            recCh = AudioDeviceModule::kChannelRight;
            break;
        case kStereoBoth:
            // default setting kChannelBoth (<=> mono)
            break;
    }

    if (_shared->audio_device()->SetRecordingChannel(recCh) != 0) {
      _shared->SetLastError(VE_AUDIO_DEVICE_MODULE_ERROR, kTraceWarning,
          "SetRecordingChannel() unable to set the recording channel");
    }

    // Map indices to unsigned since underlying functions need that
    uint16_t indexU = static_cast<uint16_t> (index);

    int32_t res(0);

    if (index == -1)
    {
        res = _shared->audio_device()->SetRecordingDevice(
            AudioDeviceModule::kDefaultCommunicationDevice);
    }
    else if (index == -2)
    {
        res = _shared->audio_device()->SetRecordingDevice(
            AudioDeviceModule::kDefaultDevice);
    }
    else
    {
        res = _shared->audio_device()->SetRecordingDevice(indexU);
    }

    if (res != 0)
    {
        _shared->SetLastError(VE_AUDIO_DEVICE_MODULE_ERROR, kTraceError,
            "SetRecordingDevice() unable to set the recording device");
        return -1;
    }

    // Init microphone, so user can do volume settings etc
    if (_shared->audio_device()->InitMicrophone() == -1)
    {
        _shared->SetLastError(VE_CANNOT_ACCESS_MIC_VOL, kTraceWarning,
            "SetRecordingDevice() cannot access microphone");
    }

    // Set number of channels
    bool available = false;
    if (_shared->audio_device()->StereoRecordingIsAvailable(&available) != 0) {
      _shared->SetLastError(VE_SOUNDCARD_ERROR, kTraceWarning,
          "StereoRecordingIsAvailable() failed to query stereo recording");
    }

    if (_shared->audio_device()->SetStereoRecording(available) != 0)
    {
        _shared->SetLastError(VE_SOUNDCARD_ERROR, kTraceWarning,
            "SetRecordingDevice() failed to set mono recording mode");
    }

    // Restore recording if it was enabled already when calling this function.
    if (isRecording)
    {
        if (!_shared->ext_recording())
        {
            VOIP_TRACE(kTraceInfo, kTraceVoice,
                AoEId(_shared->instance_id(), -1),
                "SetRecordingDevice() recording is now being restored...");
            if (_shared->audio_device()->InitRecording() != 0)
            {
                VOIP_TRACE(kTraceError, kTraceVoice,
                    AoEId(_shared->instance_id(), -1),
                    "SetRecordingDevice() failed to initialize recording");
                return -1;
            }
            if (_shared->audio_device()->StartRecording() != 0)
            {
                VOIP_TRACE(kTraceError, kTraceVoice,
                             AoEId(_shared->instance_id(), -1),
                             "SetRecordingDevice() failed to start recording");
                return -1;
            }
        }
    }

    return 0;
}

int AoEHardwareImpl::SetPlayoutDevice(int index)
{
    VOIP_TRACE(kTraceApiCall, kTraceVoice, AoEId(_shared->instance_id(), -1),
                 "SetPlayoutDevice(index=%d)", index);
    CriticalSectionScoped cs(_shared->crit_sec());

    if (!_shared->statistics().Initialized())
    {
        _shared->SetLastError(VE_NOT_INITED, kTraceError);
        return -1;
    }

    bool isPlaying(false);

    // Store state about activated playout to be able to restore it after the
    // playout device has been modified.
    if (_shared->audio_device()->Playing())
    {
        VOIP_TRACE(kTraceInfo, kTraceVoice, AoEId(_shared->instance_id(), -1),
                     "SetPlayoutDevice() device is modified while playout is "
                     "active...");
        isPlaying = true;
        if (_shared->audio_device()->StopPlayout() == -1)
        {
            _shared->SetLastError(VE_AUDIO_DEVICE_MODULE_ERROR, kTraceError,
                "SetPlayoutDevice() unable to stop playout");
            return -1;
        }
    }

    // We let the module do the index sanity

    // Map indices to unsigned since underlying functions need that
    uint16_t indexU = static_cast<uint16_t> (index);

    int32_t res(0);

    if (index == -1)
    {
        res = _shared->audio_device()->SetPlayoutDevice(
            AudioDeviceModule::kDefaultCommunicationDevice);
    }
    else if (index == -2)
    {
        res = _shared->audio_device()->SetPlayoutDevice(
            AudioDeviceModule::kDefaultDevice);
    }
    else
    {
        res = _shared->audio_device()->SetPlayoutDevice(indexU);
    }

    if (res != 0)
    {
        _shared->SetLastError(VE_SOUNDCARD_ERROR, kTraceError,
            "SetPlayoutDevice() unable to set the playout device");
        return -1;
    }

    // Init speaker, so user can do volume settings etc
    if (_shared->audio_device()->InitSpeaker() == -1)
    {
        _shared->SetLastError(VE_CANNOT_ACCESS_SPEAKER_VOL, kTraceWarning,
            "SetPlayoutDevice() cannot access speaker");
    }

    // Set number of channels
    bool available = false;
    _shared->audio_device()->StereoPlayoutIsAvailable(&available);
    if (_shared->audio_device()->SetStereoPlayout(available) != 0)
    {
        _shared->SetLastError(VE_SOUNDCARD_ERROR, kTraceWarning,
            "SetPlayoutDevice() failed to set stereo playout mode");
    }

    // Restore playout if it was enabled already when calling this function.
    if (isPlaying)
    {
        if (!_shared->ext_playout())
        {
            VOIP_TRACE(kTraceInfo, kTraceVoice,
                AoEId(_shared->instance_id(), -1),
                "SetPlayoutDevice() playout is now being restored...");
            if (_shared->audio_device()->InitPlayout() != 0)
            {
                VOIP_TRACE(kTraceError, kTraceVoice,
                  AoEId(_shared->instance_id(), -1),
                  "SetPlayoutDevice() failed to initialize playout");
                return -1;
            }
            if (_shared->audio_device()->StartPlayout() != 0)
            {
                VOIP_TRACE(kTraceError, kTraceVoice,
                             AoEId(_shared->instance_id(), -1),
                             "SetPlayoutDevice() failed to start playout");
                return -1;
            }
        }
    }

    return 0;
}

int AoEHardwareImpl::GetRecordingDeviceStatus(bool& isAvailable)
{
    VOIP_TRACE(kTraceApiCall, kTraceVoice, AoEId(_shared->instance_id(), -1),
                 "GetRecordingDeviceStatus()");

    if (!_shared->statistics().Initialized())
    {
        _shared->SetLastError(VE_NOT_INITED, kTraceError);
        return -1;
    }

    // We let the module do isRecording sanity

    bool available(false);

    // Check availability
    if (_shared->audio_device()->RecordingIsAvailable(&available) != 0)
    {
        _shared->SetLastError(VE_UNDEFINED_SC_REC_ERR, kTraceError,
            "  Audio Device error");
        return -1;
    }

    isAvailable = available;

    VOIP_TRACE(kTraceStateInfo, kTraceVoice,
        AoEId(_shared->instance_id(), -1),
        "  Output: isAvailable = %d)", (int) isAvailable);

    return 0;
}

int AoEHardwareImpl::GetPlayoutDeviceStatus(bool& isAvailable)
{
    VOIP_TRACE(kTraceApiCall, kTraceVoice, AoEId(_shared->instance_id(), -1),
                 "GetPlayoutDeviceStatus()");

    if (!_shared->statistics().Initialized())
    {
        _shared->SetLastError(VE_NOT_INITED, kTraceError);
        return -1;
    }

    // We let the module do isPlaying sanity

    bool available(false);

    // Check availability
    if (_shared->audio_device()->PlayoutIsAvailable(&available) != 0)
    {
        _shared->SetLastError(VE_PLAY_UNDEFINED_SC_ERR, kTraceError,
            "  Audio Device error");
        return -1;
    }

    isAvailable = available;

    VOIP_TRACE(kTraceStateInfo, kTraceVoice,
        AoEId(_shared->instance_id(), -1),
        "  Output: isAvailable = %d)", (int) isAvailable);

    return 0;
}

int AoEHardwareImpl::ResetAudioDevice()
{
    VOIP_TRACE(kTraceApiCall, kTraceVoice, AoEId(_shared->instance_id(), -1),
                 "ResetAudioDevice()");

    if (!_shared->statistics().Initialized())
    {
        _shared->SetLastError(VE_NOT_INITED, kTraceError);
        return -1;
    }

#if defined(VOIP_IOS)
    if (_shared->audio_device()->ResetAudioDevice() < 0)
    {
        _shared->SetLastError(VE_SOUNDCARD_ERROR, kTraceError,
            "  Failed to reset sound device");
        return -1;
    }
    return 0;
#else
    _shared->SetLastError(VE_FUNC_NOT_SUPPORTED, kTraceError,
        "  no support for resetting sound device");
    return -1;
#endif
}

int AoEHardwareImpl::AudioDeviceControl(unsigned int par1, unsigned int par2,
                                        unsigned int par3)
{
    VOIP_TRACE(kTraceApiCall, kTraceVoice, AoEId(_shared->instance_id(), -1),
                 "AudioDeviceControl(%i, %i, %i)", par1, par2, par3);
    if (!_shared->statistics().Initialized())
    {
        _shared->SetLastError(VE_NOT_INITED, kTraceError);
        return -1;
    }
    _shared->SetLastError(VE_FUNC_NOT_SUPPORTED, kTraceError,
        "  no support for resetting sound device");
    return -1;
}

int AoEHardwareImpl::SetLoudspeakerStatus(bool enable)
{
    VOIP_TRACE(kTraceApiCall, kTraceVoice, AoEId(_shared->instance_id(), -1),
                 "SetLoudspeakerStatus(enable=%i)", (int) enable);

    if (!_shared->statistics().Initialized())
    {
        _shared->SetLastError(VE_NOT_INITED, kTraceError);
        return -1;
    }
#if defined(VOIP_ANDROID)
    if (_shared->audio_device()->SetLoudspeakerStatus(enable) < 0)
    {
        _shared->SetLastError(VE_IGNORED_FUNCTION, kTraceError,
            "  Failed to set loudspeaker status");
        return -1;
    }

    return 0;
#else
    _shared->SetLastError(VE_FUNC_NOT_SUPPORTED, kTraceError,
        "  no support for setting loudspeaker status");
    return -1;
#endif
}

int AoEHardwareImpl::GetLoudspeakerStatus(bool& enabled)
{
    VOIP_TRACE(kTraceApiCall, kTraceVoice, AoEId(_shared->instance_id(), -1),
                 "GetLoudspeakerStatus()");

#if defined(VOIP_ANDROID)
    if (!_shared->statistics().Initialized())
    {
        _shared->SetLastError(VE_NOT_INITED, kTraceError);
        return -1;
    }

    if (_shared->audio_device()->GetLoudspeakerStatus(&enabled) < 0)
    {
        _shared->SetLastError(VE_IGNORED_FUNCTION, kTraceError,
            "  Failed to get loudspeaker status");
        return -1;
    }

    return 0;
#else
    _shared->SetLastError(VE_FUNC_NOT_SUPPORTED, kTraceError,
      "  no support for setting loudspeaker status");
    return -1;
#endif
}

int AoEHardwareImpl::GetCPULoad(int& loadPercent)
{
    VOIP_TRACE(kTraceApiCall, kTraceVoice, AoEId(_shared->instance_id(), -1),
                 "GetCPULoad()");

    if (!_shared->statistics().Initialized())
    {
        _shared->SetLastError(VE_NOT_INITED, kTraceError);
        return -1;
    }

    // Get CPU load from ADM
    uint16_t load(0);
    if (_shared->audio_device()->CPULoad(&load) != 0)
    {
        _shared->SetLastError(VE_CPU_INFO_ERROR, kTraceError,
            "  error getting system CPU load");
        return -1;
    }

    loadPercent = static_cast<int> (load);

    VOIP_TRACE(kTraceStateInfo, kTraceVoice,
        AoEId(_shared->instance_id(), -1),
        "  Output: loadPercent = %d", loadPercent);

    return 0;
}

int AoEHardwareImpl::EnableBuiltInAEC(bool enable)
{
    VOIP_TRACE(kTraceApiCall, kTraceVoice, AoEId(_shared->instance_id(), -1),
        "%s", __FUNCTION__);
    if (!_shared->statistics().Initialized())
    {
        _shared->SetLastError(VE_NOT_INITED, kTraceError);
        return -1;
    }

    return _shared->audio_device()->EnableBuiltInAEC(enable);
}

bool AoEHardwareImpl::BuiltInAECIsEnabled() const
{
    VOIP_TRACE(kTraceApiCall, kTraceVoice, AoEId(_shared->instance_id(), -1),
        "%s", __FUNCTION__);
    if (!_shared->statistics().Initialized())
    {
        _shared->SetLastError(VE_NOT_INITED, kTraceError);
        return false;
    }

    return _shared->audio_device()->BuiltInAECIsEnabled();
}

int AoEHardwareImpl::SetRecordingSampleRate(unsigned int samples_per_sec) {
  VOIP_TRACE(kTraceApiCall, kTraceVoice, AoEId(_shared->instance_id(), -1),
               "%s", __FUNCTION__);
  if (!_shared->statistics().Initialized()) {
    _shared->SetLastError(VE_NOT_INITED, kTraceError);
    return false;
  }
  return _shared->audio_device()->SetRecordingSampleRate(samples_per_sec);
}

int AoEHardwareImpl::RecordingSampleRate(unsigned int* samples_per_sec) const {
  VOIP_TRACE(kTraceApiCall, kTraceVoice, AoEId(_shared->instance_id(), -1),
               "%s", __FUNCTION__);
  if (!_shared->statistics().Initialized()) {
    _shared->SetLastError(VE_NOT_INITED, kTraceError);
    return false;
  }
  return _shared->audio_device()->RecordingSampleRate(samples_per_sec);
}

int AoEHardwareImpl::SetPlayoutSampleRate(unsigned int samples_per_sec) {
  VOIP_TRACE(kTraceApiCall, kTraceVoice, AoEId(_shared->instance_id(), -1),
               "%s", __FUNCTION__);
  if (!_shared->statistics().Initialized()) {
    _shared->SetLastError(VE_NOT_INITED, kTraceError);
    return false;
  }
  return _shared->audio_device()->SetPlayoutSampleRate(samples_per_sec);
}

int AoEHardwareImpl::PlayoutSampleRate(unsigned int* samples_per_sec) const {
  VOIP_TRACE(kTraceApiCall, kTraceVoice, AoEId(_shared->instance_id(), -1),
               "%s", __FUNCTION__);
  if (!_shared->statistics().Initialized()) {
    _shared->SetLastError(VE_NOT_INITED, kTraceError);
    return false;
  }
  return _shared->audio_device()->PlayoutSampleRate(samples_per_sec);
}

#endif  // VOIP_AUDIO_ENGINE_HARDWARE_API

}  // namespace VoIP
