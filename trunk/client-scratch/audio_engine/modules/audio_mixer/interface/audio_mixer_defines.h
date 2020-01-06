#ifndef VOIP_MODULES_AUDIO_CONFERENCE_MIXER_INTERFACE_AUDIO_CONFERENCE_MIXER_DEFINES_H_
#define VOIP_MODULES_AUDIO_CONFERENCE_MIXER_INTERFACE_AUDIO_CONFERENCE_MIXER_DEFINES_H_

#include "audio_engine/modules/interface/module_common_types.h"
#include "audio_engine/include/typedefs.h"

namespace VoIP {
class MixHistory;

// A callback class that all mixer participants must inherit from/implement.
class MixerParticipant
{
public:
    // The implementation of this function should update audioFrame with new
    // audio every time it's called.
    //
    // If it returns -1, the frame will not be added to the mix.
    virtual int32_t GetAudioFrame(const int32_t id, AudioFrame& audioFrame) = 0;

    // mixed will be set to true if the participant was mixed this mix iteration
    int32_t IsMixed(bool& mixed) const;

    // This function specifies the sampling frequency needed for the AudioFrame
    // for future GetAudioFrame(..) calls.
    virtual int32_t NeededFrequency(const int32_t id) = 0;

    MixHistory* _mixHistory;
protected:
    MixerParticipant();
    virtual ~MixerParticipant();
};

// Container struct for participant statistics.
struct ParticipantStatistics
{
    int32_t participant;
    int32_t level;
};

class AudioMixerStatusReceiver
{
public:
    // Callback function that provides an array of ParticipantStatistics for the
    // participants that were mixed last mix iteration.
    virtual void MixedParticipants(
        const int32_t id,
        const ParticipantStatistics* participantStatistics,
        const uint32_t size) = 0;
    // Callback function that provides an array of the ParticipantStatistics for
    // the participants that had a positiv VAD last mix iteration.
    virtual void VADPositiveParticipants(
        const int32_t id,
        const ParticipantStatistics* participantStatistics,
        const uint32_t size) = 0;
    // Callback function that provides the audio level of the mixed audio frame
    // from the last mix iteration.
    virtual void MixedAudioLevel(
        const int32_t  id,
        const uint32_t level) = 0;
protected:
    AudioMixerStatusReceiver() {}
    virtual ~AudioMixerStatusReceiver() {}
};

class AudioMixerOutputReceiver
{
public:
    // This callback function provides the mixed audio for this mix iteration.
    // Note that uniqueAudioFrames is an array of AudioFrame pointers with the
    // size according to the size parameter.
    virtual void NewMixedAudio(const int32_t id,
                               const AudioFrame& generalAudioFrame,
                               const AudioFrame** uniqueAudioFrames,
                               const uint32_t size) = 0;
protected:
    AudioMixerOutputReceiver() {}
    virtual ~AudioMixerOutputReceiver() {}
};
}  // namespace VoIP

#endif // VOIP_MODULES_AUDIO_CONFERENCE_MIXER_INTERFACE_AUDIO_CONFERENCE_MIXER_DEFINES_H_
