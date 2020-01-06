/*
 *  Copyright (c) 2011 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

// This sub-API supports the following functionalities:
//
//  - External protocol support.
//  - Packet timeout notification.
//  - Dead-or-Alive connection observations.
//
// Usage example, omitting error checking:
//
//  using namespace webrtc;
//  VoiceEngine* voe = VoiceEngine::Create();
//  VoEBase* base = VoEBase::GetInterface(voe);
//  VoENetwork* netw  = VoENetwork::GetInterface(voe);
//  base->Init();
//  int ch = base->CreateChannel();
//  ...
//  netw->SetPeriodicDeadOrAliveStatus(ch, true);
//  ...
//  base->DeleteChannel(ch);
//  base->Terminate();
//  base->Release();
//  netw->Release();
//  VoiceEngine::Delete(voe);
//
#ifndef VOIP_VOICE_ENGINE_VOE_NETWORK_H
#define VOIP_VOICE_ENGINE_VOE_NETWORK_H

#include "audio_engine/include/aoe_io.h"
/*
#include "audio_engine/include/common_types.h"

namespace webrtc {

class VoiceEngine;

// VoEConnectionObserver
class WEBRTC_DLLEXPORT VoEConnectionObserver
{
public:
    // This method will be called peridically and deliver dead-or-alive
    // notifications for a specified |channel| when the observer interface
    // has been installed and activated.
    virtual void OnPeriodicDeadOrAlive(int channel, bool alive) = 0;

protected:
    virtual ~VoEConnectionObserver() {}
};

// VoENetwork
class WEBRTC_DLLEXPORT VoENetwork
{
public:
    // Factory for the VoENetwork sub-API. Increases an internal
    // reference counter if successful. Returns NULL if the API is not
    // supported or if construction fails.
    static VoENetwork* GetInterface(VoiceEngine* voiceEngine);

    // Releases the VoENetwork sub-API and decreases an internal
    // reference counter. Returns the new reference count. This value should
    // be zero for all sub-API:s before the VoiceEngine object can be safely
    // deleted.
    virtual int Release() = 0;

    // Installs and enables a user-defined external transport protocol for a
    // specified |channel|.
    virtual int RegisterExternalTransport(
        int channel, Transport& transport) = 0;

    // Removes and disables a user-defined external transport protocol for a
    // specified |channel|.
    virtual int DeRegisterExternalTransport(int channel) = 0;

    // The packets received from the network should be passed to this
    // function when external transport is enabled. Note that the data
    // including the RTP-header must also be given to the VoiceEngine.
    virtual int ReceivedRTPPacket(int channel,
                                  const void* data,
                                  unsigned int length) = 0;
    virtual int ReceivedRTPPacket(int channel,
                                  const void* data,
                                  unsigned int length,
                                  const PacketTime& packet_time) {
      return 0;
    }

    // The packets received from the network should be passed to this
    // function when external transport is enabled. Note that the data
    // including the RTCP-header must also be given to the VoiceEngine.
    virtual int ReceivedRTCPPacket(
        int channel, const void* data, unsigned int length) = 0;

protected:
    VoENetwork() {}
    virtual ~VoENetwork() {}
};

}  // namespace webrtc
*/
#endif  //  VOIP_VOICE_ENGINE_VOE_NETWORK_H