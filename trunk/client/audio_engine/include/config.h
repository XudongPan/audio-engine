#ifndef VOIP_CONFIG_H_
#define VOIP_CONFIG_H_

#include <string>
#include <vector>

#include "common_types.h"
#include "typedefs.h"

namespace VoIP {

struct AudioPackageStatistics {
  AudioPackageStatistics()
      : ssrc(0),
        fraction_loss(0),
        cumulative_loss(0),
        extended_max_sequence_number(0) {}
  uint32_t ssrc;
  int fraction_loss;
  int cumulative_loss;
  int extended_max_sequence_number;
  std::string c_name;
};

struct StreamStats {
  StreamStats() : key_frames(0), delta_frames(0), bitrate_bps(0) {}
  uint32_t key_frames;
  uint32_t delta_frames;
  int32_t bitrate_bps;
  StreamDataCounters rtp_stats;
  RtcpStatistics rtcp_stats;
};

// Settings for NACK, see RFC 4585 for details.
struct NackConfig {
  NackConfig() : rtp_history_ms(0) {}
  // Send side: the time RTP packets are stored for retransmissions.
  // Receive side: the time the receiver is prepared to wait for
  // retransmissions.
  // Set to '0' to disable.
  int rtp_history_ms;
};

// Settings for forward error correction, see RFC 5109 for details. Set the
// payload types to '-1' to disable.
struct FecConfig {
  FecConfig() : ulpfec_payload_type(-1), red_payload_type(-1) {}
  // Payload type used for ULPFEC packets.
  int ulpfec_payload_type;

  // Payload type used for RED packets.
  int red_payload_type;
};

//header extension to use for the video stream, see RFC 5285.
struct AudioPackageExtension {
  static const char* kTOffset;
  static const char* kAbsSendTime;
  AudioPackageExtension(const char* name, int id) : name(name), id(id) {}
  std::string name;
  int id;
};
}  // namespace VoIP

#endif  // VOIP_CONFIG_H_
