#ifndef VOIP_AUDIO_ENGINE_AOE_BASE_IMPL_H
#define VOIP_AUDIO_ENGINE_AOE_BASE_IMPL_H

#include "audio_engine/voice_engine/include/aoe_base.h"

#include "audio_engine/modules/interface/module_common_types.h"
#include "audio_engine/voice_engine/shared_data.h"

namespace VoIP
{

class ProcessThread;

class AoEBaseImpl: public AoEBase,
                   public AudioTransport,
                   public AudioDeviceObserver
{
public:
    virtual int RegisterAudioEngineObserver(AudioEngineObserver& observer);

    virtual int DeRegisterAudioEngineObserver();

    virtual int Init(AudioDeviceModule* external_adm = NULL,
                     AudioProcessing* audioproc = NULL);
    virtual AudioProcessing* audio_processing() {
      return _shared->audio_processing();
    }

    virtual int Terminate();

    virtual int CreateChannel();
    virtual int CreateChannel(const Config& config);

    virtual int DeleteChannel(int channel);

    virtual int StartReceive(int channel);

    virtual int StartPlayout(int channel);

    virtual int StartSend(int channel);

	virtual int Start(int channel);

	virtual int Stop(int channel);

    virtual int StopReceive(int channel);

    virtual int StopPlayout(int channel);

    virtual int StopSend(int channel);

    virtual int GetVersion(char version[1024]);

    virtual int LastError();

    virtual AudioTransport* audio_transport() { return this; }

    // AudioTransport
    virtual int32_t
        RecordedDataIsAvailable(const void* audioSamples,
                                uint32_t nSamples,
                                uint8_t nBytesPerSample,
                                uint8_t nChannels,
                                uint32_t samplesPerSec,
                                uint32_t totalDelayMS,
                                int32_t clockDrift,
                                uint32_t micLevel,
                                bool keyPressed,
                                uint32_t& newMicLevel);

    virtual int32_t NeedMorePlayData(uint32_t nSamples,
                                     uint8_t nBytesPerSample,
                                     uint8_t nChannels,
                                     uint32_t samplesPerSec,
                                     void* audioSamples,
                                     uint32_t& nSamplesOut);

    virtual int OnDataAvailable(const int voe_channels[],
                                int number_of_voe_channels,
                                const int16_t* audio_data,
                                int sample_rate,
                                int number_of_channels,
                                int number_of_frames,
                                int audio_delay_milliseconds,
                                int volume,
                                bool key_pressed,
                                bool need_audio_processing);

    virtual void OnData(int voe_channel, const void* audio_data,
                        int bits_per_sample, int sample_rate,
                        int number_of_channels, int number_of_frames);

    virtual void PushCaptureData(int voe_channel, const void* audio_data,
                                 int bits_per_sample, int sample_rate,
                                 int number_of_channels, int number_of_frames);

    virtual void PullRenderData(int bits_per_sample, int sample_rate,
                                int number_of_channels, int number_of_frames,
                                void* audio_data);

    // AudioDeviceObserver
    virtual void OnErrorIsReported(ErrorCode error);
    virtual void OnWarningIsReported(WarningCode warning);

protected:
    AoEBaseImpl(aoe::SharedData* shared);
    virtual ~AoEBaseImpl();

private:
    int32_t StartPlayout();
    int32_t StopPlayout();
    int32_t StartSend();
    int32_t StopSend();
    int32_t TerminateInternal();

    // Helper function to process the recorded data with AudioProcessing Module,
    // demultiplex the data to specific aoe channels, encode and send to the
    // network. When |number_of_AoE_channels| is 0, it will demultiplex the
    // data to all the existing AoE channels.
    // It returns new AGC microphone volume or 0 if no volume changes
    // should be done.
    int ProcessRecordedDataWithAPM(const int voe_channels[],
                                   int number_of_voe_channels,
                                   const void* audio_data,
                                   uint32_t sample_rate,
                                   uint8_t number_of_channels,
                                   uint32_t number_of_frames,
                                   uint32_t audio_delay_milliseconds,
                                   int32_t clock_drift,
                                   uint32_t volume,
                                   bool key_pressed);

    void GetPlayoutData(int sample_rate, int number_of_channels,
                        int number_of_frames, bool feed_data_to_apm,
                        void* audio_data);

    int32_t AddBuildInfo(char* str) const;
    int32_t AddAoEVersion(char* str) const;

    // Initialize channel by setting Engine Information then initializing
    // channel.
    int InitializeChannel(aoe::ChannelOwner* channel_owner);
#ifdef VOIP_EXTERNAL_TRANSPORT
    int32_t AddExternalTransportBuild(char* str) const;
#endif
#ifdef VOIP_AOE_EXTERNAL_REC_AND_PLAYOUT
    int32_t AddExternalRecAndPlayoutBuild(char* str) const;
#endif
    AudioEngineObserver* _voiceEngineObserverPtr;
    CriticalSectionWrapper& _callbackCritSect;

    bool _voiceEngineObserver;
    AudioFrame _audioFrame;
    aoe::SharedData* _shared;
};

}  // namespace VoIP

#endif  // VOIP_AUDIO_ENGINE_AOE_BASE_IMPL_H
