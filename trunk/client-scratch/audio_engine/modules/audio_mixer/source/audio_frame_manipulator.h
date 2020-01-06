#ifndef VOIP_MODULES_AUDIO_CONFERENCE_MIXER_SOURCE_AUDIO_FRAME_MANIPULATOR_H_
#define VOIP_MODULES_AUDIO_CONFERENCE_MIXER_SOURCE_AUDIO_FRAME_MANIPULATOR_H_

namespace VoIP {
class AudioFrame;

// Updates the audioFrame's energy (based on its samples).
void CalculateEnergy(AudioFrame& audioFrame);

// Apply linear step function that ramps in/out the audio samples in audioFrame
void RampIn(AudioFrame& audioFrame);
void RampOut(AudioFrame& audioFrame);

}  // namespace VoIP

#endif // VOIP_MODULES_AUDIO_CONFERENCE_MIXER_SOURCE_AUDIO_FRAME_MANIPULATOR_H_
