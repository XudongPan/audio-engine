
/*
 *  Contains functions often used by different parts of AudioEngine.
 */

#ifndef VOIP_AUDIO_ENGINE_UTILITY_H_
#define VOIP_AUDIO_ENGINE_UTILITY_H_

#include "audio_engine/common_audio/resampler/include/push_resampler.h"
#include "audio_engine/include/typedefs.h"

namespace VoIP {

class AudioFrame;

namespace aoe {

// Upmix or downmix and resample the audio in |src_frame| to |dst_frame|.
// Expects |dst_frame| to have its |num_channels_| and |sample_rate_hz_| set to
// the desired values. Updates |samples_per_channel_| accordingly.
//
// On failure, returns -1 and copies |src_frame| to |dst_frame|.
void RemixAndResample(const AudioFrame& src_frame,
                      PushResampler<int16_t>* resampler,
                      AudioFrame* dst_frame);

// Downmix and downsample the audio in |src_data| to |dst_af| as necessary,
// specified by |codec_num_channels| and |codec_rate_hz|. |mono_buffer| is
// temporary space and must be of sufficient size to hold the downmixed source
// audio (recommend using a size of kMaxMonoDataSizeSamples).
void DownConvertToCodecFormat(const int16_t* src_data,
                              int samples_per_channel,
                              int num_channels,
                              int sample_rate_hz,
                              int codec_num_channels,
                              int codec_rate_hz,
                              int16_t* mono_buffer,
                              PushResampler<int16_t>* resampler,
                              AudioFrame* dst_af);

void MixWithSat(int16_t target[],
                int target_channel,
                const int16_t source[],
                int source_channel,
                int source_len);

}  // namespace aoe
}  // namespace VoIP

#endif  // VOIP_AUDIO_ENGINE_UTILITY_H_
