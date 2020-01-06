// Modified from the Chromium original here:
// src/media/base/sinc_resampler_unittest.cc

#ifndef VOIP_COMMON_AUDIO_RESAMPLER_SINUSOIDAL_LINEAR_CHIRP_SOURCE_H_
#define VOIP_COMMON_AUDIO_RESAMPLER_SINUSOIDAL_LINEAR_CHIRP_SOURCE_H_

#include "audio_engine/common_audio/resampler/sinc_resampler.h"
#include "audio_engine/system_wrappers/interface/constructor_magic.h"

namespace webrtc {

// Fake audio source for testing the resampler.  Generates a sinusoidal linear
// chirp (http://en.wikipedia.org/wiki/Chirp) which can be tuned to stress the
// resampler for the specific sample rate conversion being used.
class SinusoidalLinearChirpSource : public SincResamplerCallback {
 public:
  // |delay_samples| can be used to insert a fractional sample delay into the
  // source.  It will produce zeros until non-negative time is reached.
  SinusoidalLinearChirpSource(int sample_rate, int samples,
                              double max_frequency, double delay_samples);

  virtual ~SinusoidalLinearChirpSource() {}

  virtual void Run(int frames, float* destination) OVERRIDE;

  double Frequency(int position);

 private:
  enum {
    kMinFrequency = 5
  };

  double sample_rate_;
  int total_samples_;
  double max_frequency_;
  double k_;
  int current_index_;
  double delay_samples_;

  DISALLOW_COPY_AND_ASSIGN(SinusoidalLinearChirpSource);
};

}  // namespace voip

#endif  // VOIP_COMMON_AUDIO_RESAMPLER_SINUSOIDAL_LINEAR_CHIRP_SOURCE_H_
