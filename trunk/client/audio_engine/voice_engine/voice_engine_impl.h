/*
 *  Copyright (c) 2012 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef WEBRTC_VOICE_ENGINE_VOICE_ENGINE_IMPL_H
#define WEBRTC_VOICE_ENGINE_VOICE_ENGINE_IMPL_H

#include "audio_engine/include/engine_configurations.h"
#include "audio_engine/system_wrappers/interface/atomic32.h"
#include "audio_engine/voice_engine/voe_base_impl.h"

#ifdef VOIP_AUDIO_ENGINE_AUDIO_PROCESSING_API
#include "audio_engine/voice_engine/voe_audio_processing_impl.h"
#endif
#ifdef VOIP_AUDIO_ENGINE_CODEC_API
#include "audio_engine/voice_engine/voe_codec_impl.h"
#endif
#ifdef WEBRTC_VOICE_ENGINE_DTMF_API
#include "audio_engine/voice_engine/voe_dtmf_impl.h"
#endif
#ifdef WEBRTC_VOICE_ENGINE_EXTERNAL_MEDIA_API
#include "audio_engine/voice_engine/voe_external_media_impl.h"
#endif
#ifdef WEBRTC_VOICE_ENGINE_FILE_API
#include "audio_engine/voice_engine/voe_file_impl.h"
#endif
#ifdef VOIP_AUDIO_ENGINE_HARDWARE_API
#include "audio_engine/voice_engine/voe_hardware_impl.h"
#endif
#ifdef WEBRTC_VOICE_ENGINE_NETEQ_STATS_API
#include "audio_engine/voice_engine/voe_neteq_stats_impl.h"
#endif
#include "audio_engine/voice_engine/voe_network_impl.h"
#ifdef WEBRTC_VOICE_ENGINE_RTP_RTCP_API
#include "audio_engine/voice_engine/voe_audio_pkg_impl.h"
#endif
#ifdef VOIP_AUDIO_ENGINE_VOLUME_CONTROL_API
#include "audio_engine/voice_engine/voe_volume_control_impl.h"
#endif

namespace VoIP
{

class VoiceEngineImpl : public voe::SharedData,  // Must be the first base class
                        public AudioEngine,
#ifdef VOIP_AUDIO_ENGINE_AUDIO_PROCESSING_API
                        public VoEAudioProcessingImpl,
#endif
#ifdef VOIP_AUDIO_ENGINE_CODEC_API
                        public VoECodecImpl,
#endif
#ifdef WEBRTC_VOICE_ENGINE_DTMF_API
                        public VoEDtmfImpl,
#endif
#ifdef WEBRTC_VOICE_ENGINE_EXTERNAL_MEDIA_API
                        public VoEExternalMediaImpl,
#endif
#ifdef WEBRTC_VOICE_ENGINE_FILE_API
                        public VoEFileImpl,
#endif
#ifdef VOIP_AUDIO_ENGINE_HARDWARE_API
                        public VoEHardwareImpl,
#endif
#ifdef WEBRTC_VOICE_ENGINE_NETEQ_STATS_API
                        public VoENetEqStatsImpl,
#endif
                        public VoENetworkImpl,
#ifdef WEBRTC_VOICE_ENGINE_RTP_RTCP_API
                        public VoERTP_RTCPImpl,
#endif
#ifdef VOIP_AUDIO_ENGINE_VOLUME_CONTROL_API
                        public VoEVolumeControlImpl,
#endif
                        public VoEBaseImpl
{
public:
    VoiceEngineImpl(const Config* config, bool owns_config) :
        SharedData(*config),
#ifdef VOIP_AUDIO_ENGINE_AUDIO_PROCESSING_API
        VoEAudioProcessingImpl(this),
#endif
#ifdef VOIP_AUDIO_ENGINE_CODEC_API
        VoECodecImpl(this),
#endif
#ifdef WEBRTC_VOICE_ENGINE_DTMF_API
        VoEDtmfImpl(this),
#endif
#ifdef WEBRTC_VOICE_ENGINE_EXTERNAL_MEDIA_API
        VoEExternalMediaImpl(this),
#endif
#ifdef WEBRTC_VOICE_ENGINE_FILE_API
        VoEFileImpl(this),
#endif
#ifdef VOIP_AUDIO_ENGINE_HARDWARE_API
        VoEHardwareImpl(this),
#endif
#ifdef WEBRTC_VOICE_ENGINE_NETEQ_STATS_API
        VoENetEqStatsImpl(this),
#endif
        VoENetworkImpl(this),
#ifdef WEBRTC_VOICE_ENGINE_RTP_RTCP_API
        VoERTP_RTCPImpl(this),
#endif
#ifdef VOIP_AUDIO_ENGINE_VOLUME_CONTROL_API
        VoEVolumeControlImpl(this),
#endif
        VoEBaseImpl(this),
        _ref_count(0),
        own_config_(owns_config ? config : NULL)
    {
    }
    virtual ~VoiceEngineImpl()
    {
        assert(_ref_count.Value() == 0);
    }

    int AddRef();

    // This implements the Release() method for all the inherited interfaces.
    virtual int Release();

private:
    Atomic32 _ref_count;
    scoped_ptr<const Config> own_config_;
};

}  // namespace VoIP

#endif // WEBRTC_VOICE_ENGINE_VOICE_ENGINE_IMPL_H
