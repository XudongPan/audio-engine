/*
 *  Copyright (c) 2012 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef WEBRTC_MODULES_RTP_RTCP_SOURCE_FEC_RECEIVER_IMPL_H_
#define WEBRTC_MODULES_RTP_RTCP_SOURCE_FEC_RECEIVER_IMPL_H_

// This header is included to get the nested declaration of Packet structure.

#include "audio_engine/modules/audio_package/interface/fec_receiver.h"
#include "audio_engine/modules/audio_package/interface/audio_pkg_defines.h"
#include "audio_engine/modules/audio_package/source/forward_error_correction.h"
#include "audio_engine/system_wrappers/interface/scoped_ptr.h"
#include "audio_engine/include/typedefs.h"

namespace VoIP {

class CriticalSectionWrapper;

class FecReceiverImpl : public FecReceiver {
 public:
  FecReceiverImpl(RtpData* callback);
  virtual ~FecReceiverImpl();

  virtual int32_t AddReceivedRedPacket(const AudioPacketHeader& rtp_header,
                                       const uint8_t* incoming_rtp_packet,
                                       int packet_length,
                                       uint8_t ulpfec_payload_type) OVERRIDE;

  virtual int32_t ProcessReceivedFec() OVERRIDE;

 private:
  scoped_ptr<CriticalSectionWrapper> crit_sect_;
  RtpData* recovered_packet_callback_;
  ForwardErrorCorrection* fec_;
  // TODO(holmer): In the current version received_packet_list_ is never more
  // than one packet, since we process FEC every time a new packet
  // arrives. We should remove the list.
  ForwardErrorCorrection::ReceivedPacketList received_packet_list_;
  ForwardErrorCorrection::RecoveredPacketList recovered_packet_list_;
};
}  // namespace VoIP

#endif  // WEBRTC_MODULES_RTP_RTCP_SOURCE_FEC_RECEIVER_IMPL_H_
