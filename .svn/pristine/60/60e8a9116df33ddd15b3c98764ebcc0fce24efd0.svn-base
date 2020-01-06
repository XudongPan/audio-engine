#ifndef VOIP_MODULES_AUDIO_CONFERENCE_MIXER_INTERFACE_AUDIO_CONFERENCE_MIXER_H_
#define VOIP_MODULES_AUDIO_CONFERENCE_MIXER_INTERFACE_AUDIO_CONFERENCE_MIXER_H_

#include "audio_engine/modules/audio_mixer/interface/audio_mixer_defines.h"
#include "audio_engine/modules/interface/module.h"
#include "audio_engine/modules/interface/module_common_types.h"

namespace VoIP {
class AudioMixerOutputReceiver;
class AudioMixerStatusReceiver;
class MixerParticipant;
class Trace;

class AudioConferenceMixer : public Module
{
public:
    enum {kMaximumAmountOfMixedParticipants = 3};
    enum Frequency
    {
        kNbInHz           = 8000,
        kWbInHz           = 16000,
        kSwbInHz          = 32000,
        kFbInHz           = 48000,
        kLowestPossible   = -1,
        kDefaultFrequency = kWbInHz
    };

    // Factory method. Constructor disabled.
    static AudioConferenceMixer* Create(int id);
    virtual ~AudioConferenceMixer() {}

    // Module functions
    virtual int32_t ChangeUniqueId(const int32_t id) = 0;
    virtual int32_t TimeUntilNextProcess() = 0 ;
    virtual int32_t Process() = 0;

    // Register/unregister a callback class for receiving the mixed audio.
    virtual int32_t RegisterMixedStreamCallback(
        AudioMixerOutputReceiver& receiver) = 0;
    virtual int32_t UnRegisterMixedStreamCallback() = 0;

    // Register/unregister a callback class for receiving status information.
    virtual int32_t RegisterMixerStatusCallback(
        AudioMixerStatusReceiver& mixerStatusCallback,
        const uint32_t amountOf10MsBetweenCallbacks) = 0;
    virtual int32_t UnRegisterMixerStatusCallback() = 0;

    // Add/remove participants as candidates for mixing.
    virtual int32_t SetMixabilityStatus(MixerParticipant& participant,
                                        bool mixable) = 0;
    // mixable is set to true if a participant is a candidate for mixing.
    virtual int32_t MixabilityStatus(MixerParticipant& participant,
                                     bool& mixable) = 0;

    // Inform the mixer that the participant should always be mixed and not
    // count toward the number of mixed participants. Note that a participant
    // must have been added to the mixer (by calling SetMixabilityStatus())
    // before this function can be successfully called.
    virtual int32_t SetAnonymousMixabilityStatus(MixerParticipant& participant,
                                                 const bool mixable) = 0;
    // mixable is set to true if the participant is mixed anonymously.
    virtual int32_t AnonymousMixabilityStatus(MixerParticipant& participant,
                                              bool& mixable) = 0;

    // Set the minimum sampling frequency at which to mix. The mixing algorithm
    // may still choose to mix at a higher samling frequency to avoid
    // downsampling of audio contributing to the mixed audio.
    virtual int32_t SetMinimumMixingFrequency(Frequency freq) = 0;

protected:
    AudioConferenceMixer() {}
};
}  // namespace VoIP

#endif // VOIP_MODULES_AUDIO_CONFERENCE_MIXER_INTERFACE_AUDIO_CONFERENCE_MIXER_H_
