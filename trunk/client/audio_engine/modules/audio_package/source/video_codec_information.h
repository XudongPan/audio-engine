/*
 *  Copyright (c) 2011 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef WEBRTC_MODULES_RTP_RTCP_SOURCE_VIDEO_CODEC_INFORMATION_H_
#define WEBRTC_MODULES_RTP_RTCP_SOURCE_VIDEO_CODEC_INFORMATION_H_

#include "audio_engine/modules/audio_package/source/audio_pkg_config.h"
#include "audio_engine/modules/audio_package/source/audio_pkg_utility.h"

namespace VoIP {
class VideoCodecInformation
{
public:
    virtual void Reset() = 0;

    virtual RtpVideoCodecTypes Type() = 0;
    virtual ~VideoCodecInformation(){};
};
}  // namespace VoIP

#endif // WEBRTC_MODULES_RTP_RTCP_SOURCE_VIDEO_CODEC_INFORMATION_H_
