/*
 *  Copyright (c) 2011 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef WEBRTC_MODULES_RTP_RTCP_SOURCE_DTMF_QUEUE_H_
#define WEBRTC_MODULES_RTP_RTCP_SOURCE_DTMF_QUEUE_H_

#include "audio_engine/modules/audio_package/source/audio_pkg_config.h"
#include "audio_engine/system_wrappers/interface/critical_section_wrapper.h"
#include "audio_engine/include/typedefs.h"

namespace VoIP {
class DTMFqueue {
 public:
  DTMFqueue();
  virtual ~DTMFqueue();

  int32_t AddDTMF(uint8_t dtmf_key, uint16_t len, uint8_t level);
  int8_t NextDTMF(uint8_t* dtmf_key, uint16_t* len, uint8_t* level);
  bool PendingDTMF();
  void ResetDTMF();

 private:
  CriticalSectionWrapper* dtmf_critsect_;
  uint8_t next_empty_index_;
  uint8_t dtmf_key_[DTMF_OUTBAND_MAX];
  uint16_t dtmf_length[DTMF_OUTBAND_MAX];
  uint8_t dtmf_level_[DTMF_OUTBAND_MAX];
};
}  // namespace VoIP

#endif  // WEBRTC_MODULES_RTP_RTCP_SOURCE_DTMF_QUEUE_H_
