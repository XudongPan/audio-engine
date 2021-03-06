/*
 *  Copyright (c) 2012 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include "audio_engine/modules/audio_coding/main/acm2/acm_resampler.h"

#include <string.h>

#include "audio_engine/common_audio/resampler/include/resampler.h"
#include "audio_engine/system_wrappers/interface/logging.h"

namespace VoIP {
namespace acm2 {

ACMResampler::ACMResampler() {
}

ACMResampler::~ACMResampler() {
}

int ACMResampler::Resample10Msec(const int16_t* in_audio,
                                 int in_freq_hz,
                                 int out_freq_hz,
                                 int num_audio_channels,
                                 int16_t* out_audio) {
  int in_length = in_freq_hz * num_audio_channels / 100;
  int out_length = out_freq_hz * num_audio_channels / 100;
  if (in_freq_hz == out_freq_hz) {
    memcpy(out_audio, in_audio, in_length * sizeof(int16_t));
    return in_length / num_audio_channels;
  }

  if (resampler_.InitializeIfNeeded(in_freq_hz, out_freq_hz,
                                    num_audio_channels) != 0) {
    LOG_FERR3(LS_ERROR, InitializeIfNeeded, in_freq_hz, out_freq_hz,
              num_audio_channels);
    return -1;
  }

  out_length = resampler_.Resample(in_audio, in_length, out_audio, out_length);
  if (out_length == -1) {
    LOG_FERR4(LS_ERROR, Resample, in_audio, in_length, out_audio, out_length);
    return -1;
  }

  return out_length / num_audio_channels;
}

}  // namespace acm2
}  // namespace VoIP
