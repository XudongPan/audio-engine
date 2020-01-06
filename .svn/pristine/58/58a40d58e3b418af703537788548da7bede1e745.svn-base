
#if defined(VOIP_ANDROID)
#include "audio_engine/modules/audio_device/android/audio_device_template.h"
#include "audio_engine/modules/audio_device/android/audio_record_jni.h"
#include "audio_engine/modules/audio_device/android/audio_track_jni.h"
#if !defined(VOIP_BUILD)
#include "audio_engine/modules/audio_device/android/opensles_input.h"
#include "audio_engine/modules/audio_device/android/opensles_output.h"
#endif
#endif

#include "audio_engine/system_wrappers/interface/trace.h"
#include "audio_engine/voice_engine/audio_engine_impl.h"

namespace VoIP
{

// Counter to be ensure that we can add a correct ID in all static trace
// methods. It is not the nicest solution, especially not since we already
// have a counter in AoEBaseImpl. In other words, there is room for
// improvement here.
static int32_t gAudioEngineInstanceCounter = 0;

AudioEngine* GetAudioEngine(const Config* config, bool owns_config)
{
#if (defined _WIN32)
  HMODULE hmod = LoadLibrary(TEXT("AudioEngineTestingDynamic.dll"));

  if (hmod) {
    typedef AudioEngine* (*PfnGetAudioEngine)(void);
    PfnGetAudioEngine pfn = (PfnGetAudioEngine)GetProcAddress(
        hmod,"GetAudioEngine");
    if (pfn) {
      AudioEngine* self = pfn();
      if (owns_config) {
        delete config;
      }
      return (self);
    }
  }
#endif

    AudioEngineImpl* self = new AudioEngineImpl(config, owns_config);
    if (self != NULL)
    {
        self->AddRef();  // First reference.  Released in AudioEngine::Delete.
        gAudioEngineInstanceCounter++;
    }
    return self;
}

int AudioEngineImpl::AddRef() {
  return ++_ref_count;
}

// This implements the Release() method for all the inherited interfaces.
int AudioEngineImpl::Release() {
  int new_ref = --_ref_count;
  assert(new_ref >= 0);
  if (new_ref == 0) {
    VOIP_TRACE(kTraceApiCall, kTraceVoice, -1,
                 "AudioEngineImpl self deleting (audioEngine=0x%p)",
                 this);

    // Clear any pointers before starting destruction. Otherwise worker-
    // threads will still have pointers to a partially destructed object.
    // Example: AudioDeviceBuffer::RequestPlayoutData() can access a
    // partially deconstructed |_ptrCbAudioTransport| during destruction
    // if we don't call Terminate here.
    Terminate();
    delete this;
  }

  return new_ref;
}

AudioEngine* AudioEngine::Create() {
  Config* config = new Config();
  
  return GetAudioEngine(config, true);
}

AudioEngine* AudioEngine::Create(const Config& config) {
  return GetAudioEngine(&config, false);
}

int AudioEngine::SetTraceFilter(unsigned int filter)
{
    VOIP_TRACE(kTraceApiCall, kTraceVoice,
                 AoEId(gAudioEngineInstanceCounter, -1),
                 "SetTraceFilter(filter=0x%x)", filter);

    // Remember old filter
    uint32_t oldFilter = Trace::level_filter();
    Trace::set_level_filter(filter);

    // If previous log was ignored, log again after changing filter
    if (kTraceNone == oldFilter)
    {
        VOIP_TRACE(kTraceApiCall, kTraceVoice, -1,
                     "SetTraceFilter(filter=0x%x)", filter);
    }

    return 0;
}

int AudioEngine::SetTraceFile(const char* fileNameUTF8,
                              bool addFileCounter)
{
    int ret = Trace::SetTraceFile(fileNameUTF8, addFileCounter);
    VOIP_TRACE(kTraceApiCall, kTraceVoice,
                 AoEId(gAudioEngineInstanceCounter, -1),
                 "SetTraceFile(fileNameUTF8=%s, addFileCounter=%d)",
                 fileNameUTF8, addFileCounter);
    return (ret);
}

int AudioEngine::SetTraceCallback(TraceCallback* callback)
{
    VOIP_TRACE(kTraceApiCall, kTraceVoice,
                 AoEId(gAudioEngineInstanceCounter, -1),
                 "SetTraceCallback(callback=0x%x)", callback);
    return (Trace::SetTraceCallback(callback));
}

bool AudioEngine::Delete(AudioEngine*& audioEngine)
{
    if (audioEngine == NULL)
        return false;

    AudioEngineImpl* s = static_cast<AudioEngineImpl*>(audioEngine);
    // Release the reference that was added in GetAudioEngine.
    int ref = s->Release();
    audioEngine = NULL;

    if (ref != 0) {
        VOIP_TRACE(kTraceWarning, kTraceVoice, -1,
            "AudioEngine::Delete did not release the very last reference.  "
            "%d references remain.", ref);
    }

    return true;
}

}  // namespace VoIP
