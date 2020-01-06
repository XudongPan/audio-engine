// This sub-API supports the following functionalities:
//
//  - External protocol support.
//  - Packet timeout notification.
//  - Dead-or-Alive connection observations.
//
// Usage example, omitting error checking:
//
//  using namespace VoIP;
//  AudioEngine* aoe = AudioEngine::Create();
//  AoEBase* base = AoEBase::GetInterface(aoe);
//  AoEIO* io  = AoEIO::GetInterface(aoe);
//  base->Init();
//  int ch = base->CreateChannel();
//  ...
//  io->SetPeriodicDeadOrAliveStatus(ch, true);
//  ...
//  base->DeleteChannel(ch);
//  base->Terminate();
//  base->Release();
//  io->Release();
//  AudioEngine::Delete(aoe);
//
#ifndef VOIP_AUDIO_ENGINE_IO_H
#define VOIP_AUDIO_ENGINE_IO_H

#include "audio_engine/include/common_types.h"
#include "audio_engine/include/common.h"

namespace VoIP {

class AudioEngine;

// AoEConnectionObserver
class VOIP_DLLEXPORT AoEConnectionObserver
{
public:
    // This method will be called peridically and deliver dead-or-alive
    // notifications for a specified |channel| when the observer interface
    // has been installed and activated.
    virtual void OnPeriodicDeadOrAlive(int channel, bool alive) = 0;

protected:
    virtual ~AoEConnectionObserver() {}
};

// External transport callback interface
class Transport
{
public:
	//virtual int SendPacket(int channel, const void *data, int len) = 0;
	virtual int SendRecPacket(int channel, AudioFrame & recFrame) = 0;
	//virtual int SendCtrlPacket(int channel, const void *data, int len) = 0;

protected:
	virtual ~Transport() {}
	Transport() {}
};


// AoEIO
class VOIP_DLLEXPORT AoEIO
{
public:
    // Factory for the AoEIO sub-API. Increases an internal
    // reference counter if successful. Returns NULL if the API is not
    // supported or if construction fails.
    static AoEIO* GetInterface(AudioEngine* audioEngine);

    // Releases the AoEIO sub-API and decreases an internal
    // reference counter. Returns the new reference count. This value should
    // be zero for all sub-API:s before the AudioEngine object can be safely
    // deleted.
    virtual int Release() = 0;

    // Installs and enables a user-defined external transport protocol for a
    // specified |channel|.
    virtual int RegisterExternalTransport(
        int channel, Transport& transport) = 0;

    // Removes and disables a user-defined external transport protocol for a
    // specified |channel|.
    virtual int DeRegisterExternalTransport(int channel) = 0;


	// The packets received from far-end client should be passed to this
	// function when external transport is enabled. Note that the data
	// must be given to the AudioEngine.
	virtual int ReceivedPlayoutData(int channel, AudioFrame &frame) = 0;
protected:
    AoEIO() {}
    virtual ~AoEIO() {}
};

}  // namespace VoIP

#endif  //  VOIP_AUDIO_ENGINE_IO_H
