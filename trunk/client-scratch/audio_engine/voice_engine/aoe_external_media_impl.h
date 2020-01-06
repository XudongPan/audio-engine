#ifndef VOIP_AUDIO_ENGINE_AOE_EXTERNAL_MEDIA_IMPL_H
#define VOIP_AUDIO_ENGINE_AOE_EXTERNAL_MEDIA_IMPL_H

#include "audio_engine/voice_engine/include/aoe_external_media.h"

#include "audio_engine/voice_engine/shared_data.h"

namespace VoIP {

class AoEExternalMediaImpl : public AoEExternalMedia
{
public:
    virtual int RegisterExternalMediaProcessing(
        int channel,
        ProcessingTypes type,
        AoEMediaProcess& processObject);

    virtual int DeRegisterExternalMediaProcessing(
        int channel,
        ProcessingTypes type);

    virtual int SetExternalRecordingStatus(bool enable);

    virtual int SetExternalPlayoutStatus(bool enable);

    virtual int ExternalRecordingInsertData(
        const int16_t speechData10ms[],
        int lengthSamples,
        int samplingFreqHz,
        int current_delay_ms);

    virtual int ExternalPlayoutGetData(int16_t speechData10ms[],
                                       int samplingFreqHz,
                                       int current_delay_ms,
                                       int& lengthSamples);

    virtual int GetAudioFrame(int channel, int desired_sample_rate_hz,
                              AudioFrame* frame);

    virtual int SetExternalMixing(int channel, bool enable);

protected:
    AoEExternalMediaImpl(aoe::SharedData* shared);
    virtual ~AoEExternalMediaImpl();

private:
#ifdef VOIP_AOE_EXTERNAL_REC_AND_PLAYOUT
    int playout_delay_ms_;
#endif
    aoe::SharedData* shared_;
};

}  // namespace VoIP

#endif  // VOIP_AUDIO_ENGINE_AOE_EXTERNAL_MEDIA_IMPL_H
