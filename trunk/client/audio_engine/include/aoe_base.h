// This sub-API supports the following functionalities:
//
//  - Initialization and termination.
//  - Trace information on text files or via callbacks.
//  - Multi-channel support (mixing, sending to multiple destinations etc.).
//
// Usage example, omitting error checking:
//
//  using namespace VoIP;
//  AudioEngine* aoe = AudioEngine::Create();
//  AoEBase* base = AoEBase::GetInterface(aoe);
//  base->Init();
//  int ch = base->CreateChannel();
//  base->StartPlayout(ch);
//  ...
//  base->DeleteChannel(ch);
//  base->Terminate();
//  base->Release();
//  AudioEngine::Delete(voe);
//
#ifndef VOIP_AUDIO_ENGINE_BASE_H
#define VOIP_AUDIO_ENGINE_BASE_H

#include "audio_engine/include/common_types.h"

namespace VoIP {

class AudioDeviceModule;
class AudioProcessing;
class AudioTransport;
class Config;

const int kAoEDefault = -1;

// AudioEngineObserver
class VOIP_DLLEXPORT AudioEngineObserver
{
public:
    // This method will be called after the occurrence of any runtime error
    // code, or warning notification, when the observer interface has been
    // installed using AoEBase::RegisterAudioEngineObserver().
    virtual void CallbackOnError(int channel, int errCode) = 0;

protected:
    virtual ~AudioEngineObserver() {}
};

// AudioEngine
class VOIP_DLLEXPORT AudioEngine
{
public:
    // Creates a AudioEngine object, which can then be used to acquire
    // sub-APIs. Returns NULL on failure.
    static AudioEngine* Create();
    static AudioEngine* Create(const Config& config);

    // Deletes a created AudioEngine object and releases the utilized resources.
    // Note that if there are outstanding references held via other interfaces,
    // the voice engine instance will not actually be deleted until those
    // references have been released.
    static bool Delete(AudioEngine*& audioEngine);

    // Specifies the amount and type of trace information which will be
    // created by the AudioEngine.
    static int SetTraceFilter(unsigned int filter);

    // Sets the name of the trace file and enables trace messages.
    static int SetTraceFile(const char* fileNameUTF8,
                            bool addFileCounter = false);

    // Installs the TraceCallback implementation to ensure that the user
    // receives callbacks for generated trace messages.
    static int SetTraceCallback(TraceCallback* callback);

protected:
    AudioEngine() {}
    ~AudioEngine() {}
};

// AoEBase
class VOIP_DLLEXPORT AoEBase
{
public:
    // Factory for the AoEBase sub-API. Increases an internal reference
    // counter if successful. Returns NULL if the API is not supported or if
    // construction fails.
    static AoEBase* GetInterface(AudioEngine* audioEngine);

    // Releases the AoEBase sub-API and decreases an internal reference
    // counter. Returns the new reference count. This value should be zero
    // for all sub-APIs before the AudioEngine object can be safely deleted.
    virtual int Release() = 0;

    // Installs the observer class to enable runtime error control and
    // warning notifications.
    virtual int RegisterAudioEngineObserver(AudioEngineObserver& observer) = 0;

    // Removes and disables the observer class for runtime error control
    // and warning notifications.
    virtual int DeRegisterAudioEngineObserver() = 0;

    // Initializes all common parts of the AudioEngine; e.g. all
	// the sound card and core receiving components.
    // This method also makes it possible to install some user-defined external
    // modules:
    // - The Audio Device Module (ADM) which implements all the audio layer
    // functionality in a separate (reference counted) module.
    // - The AudioProcessing module handles capture-side processing. AudioEngine
    // takes ownership of this object.
    // If NULL is passed for any of these, AudioEngine will create its own.
    virtual int Init(AudioDeviceModule* external_adm = NULL,
                     AudioProcessing* audioproc = NULL) = 0;

    // Returns NULL before Init() is called.
    virtual AudioProcessing* audio_processing() = 0;

    // Terminates all AudioEngine functions and releses allocated resources.
    virtual int Terminate() = 0;

    // Creates a new channel and allocates the required resources for it.
    // One can use |config| to configure the channel. Currently that is used for
    // choosing between ACM1 and ACM2, when creating Audio Coding Module.
    virtual int CreateChannel() = 0;
    virtual int CreateChannel(const Config& config) = 0;

    // Deletes an existing channel and releases the utilized resources.
    virtual int DeleteChannel(int channel) = 0;

    // Prepares and initiates the AudioEngine for reception of
    virtual int StartReceive(int channel) = 0;

    // Stops receiving incoming PLAYOUT packets on the specified |channel|.
    virtual int StopReceive(int channel) = 0;

    // Starts forwarding the packets to the mixer/soundcard for a
    // specified |channel|.
    virtual int StartPlayout(int channel) = 0;

    // Stops forwarding the packets to the mixer/soundcard for a
    // specified |channel|.
    virtual int StopPlayout(int channel) = 0;

    // Starts sending packets to an already specified IP address and
    // port number for a specified |channel|.
    virtual int StartSend(int channel) = 0;

    // Stops sending packets from a specified |channel|.
    virtual int StopSend(int channel) = 0;

    // Gets the version information for AudioEngine and its components.
    virtual int GetVersion(char version[1024]) = 0;

    // Gets the last AudioEngine error code.
    virtual int LastError() = 0;

    virtual AudioTransport* audio_transport() { return NULL; }

protected:
    AoEBase() {}
    virtual ~AoEBase() {}
};

}  // namespace VoIP

#endif  //  VOIP_AUDIO_ENGINE_BASE_H
