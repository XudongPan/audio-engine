
#ifndef VOIP_AUDIO_ENGINE_SHARED_DATA_H
#define VOIP_AUDIO_ENGINE_SHARED_DATA_H

#include "audio_engine/modules/audio_device/include/audio_device.h"
#include "audio_engine/modules/audio_processing/include/audio_processing.h"
#include "audio_engine/modules/utility/interface/process_thread.h"
#include "audio_engine/system_wrappers/interface/scoped_ptr.h"
#include "audio_engine/voice_engine/channel_manager.h"
#include "audio_engine/voice_engine/statistics.h"
#include "audio_engine/voice_engine/audio_engine_defines.h"

class ProcessThread;

namespace VoIP {
class Config;
class CriticalSectionWrapper;

namespace aoe {

class TransmitMixer;
class OutputMixer;

class SharedData
{
public:
    // Public accessors.
    uint32_t instance_id() const { return _instanceId; }
    Statistics& statistics() { return _engineStatistics; }
    ChannelManager& channel_manager() { return _channelManager; }
    AudioDeviceModule* audio_device() { return _audioDevicePtr; }
    void set_audio_device(AudioDeviceModule* audio_device);
    AudioProcessing* audio_processing() { return audioproc_.get(); }
    void set_audio_processing(AudioProcessing* audio_processing);
    TransmitMixer* transmit_mixer() { return _transmitMixerPtr; }
    OutputMixer* output_mixer() { return _outputMixerPtr; }
    CriticalSectionWrapper* crit_sec() { return _apiCritPtr; }
    bool ext_recording() const { return _externalRecording; }
    void set_ext_recording(bool value) { _externalRecording = value; }
    bool ext_playout() const { return _externalPlayout; }
    void set_ext_playout(bool value) { _externalPlayout = value; }
    ProcessThread* process_thread() { return _moduleProcessThreadPtr; }
    AudioDeviceModule::AudioLayer audio_device_layer() const {
      return _audioDeviceLayer;
    }
    void set_audio_device_layer(AudioDeviceModule::AudioLayer layer) {
      _audioDeviceLayer = layer;
    }

    int NumOfSendingChannels();
    int NumOfPlayingChannels();

    // Convenience methods for calling statistics().SetLastError().
    void SetLastError(int32_t error) const;
    void SetLastError(int32_t error, TraceLevel level) const;
    void SetLastError(int32_t error, TraceLevel level,
                      const char* msg) const;

protected:
    const uint32_t _instanceId;
    CriticalSectionWrapper* _apiCritPtr;
    ChannelManager _channelManager;
    Statistics _engineStatistics;
    AudioDeviceModule* _audioDevicePtr;
    OutputMixer* _outputMixerPtr;
    TransmitMixer* _transmitMixerPtr;
    scoped_ptr<AudioProcessing> audioproc_;
    ProcessThread* _moduleProcessThreadPtr;

    bool _externalRecording;
    bool _externalPlayout;

    AudioDeviceModule::AudioLayer _audioDeviceLayer;

    SharedData(const Config& config);
    virtual ~SharedData();
};

}  // namespace aoe

}  // namespace VoIP
#endif // VOIP_AUDIO_ENGINE_SHARED_DATA_H
