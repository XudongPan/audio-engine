
#ifndef VOIP_AUDIO_ENGINE_AUDIO_ENGINE_DEFINES_H
#define VOIP_AUDIO_ENGINE_AUDIO_ENGINE_DEFINES_H

#include "audio_engine/include/common_types.h"
#include "audio_engine/include/engine_configurations.h"
#include "audio_engine/modules/audio_processing/include/audio_processing.h"
#include "audio_engine/system_wrappers/interface/logging.h"

// ----------------------------------------------------------------------------
//  Enumerators
// ----------------------------------------------------------------------------

namespace VoIP {

// Internal buffer size required for mono audio, based on the highest sample
// rate voice engine supports (10 ms of audio at 192 kHz).
static const int kMaxMonoDataSizeSamples = 1920;

// VolumeControl
enum { kMinVolumeLevel = 0 };
enum { kMaxVolumeLevel = 255 };
// Min scale factor for per-channel volume scaling
const float kMinOutputVolumeScaling = 0.0f;
// Max scale factor for per-channel volume scaling
const float kMaxOutputVolumeScaling = 10.0f;
// Min scale factor for output volume panning
const float kMinOutputVolumePanning = 0.0f;
// Max scale factor for output volume panning
const float kMaxOutputVolumePanning = 1.0f;

enum { kAudioEngineMaxModuleVersionSize = 960 };

// Base
enum { kAudioEngineVersionMaxMessageSize = 1024 };

// Audio processing
const NoiseSuppression::Level kDefaultNsMode = NoiseSuppression::kModerate;
const GainControl::Mode kDefaultAgcMode =
#if defined(VOIP_ANDROID) || defined(VOIP_IOS)
  GainControl::kAdaptiveDigital;
#else
  GainControl::kAdaptiveAnalog;
#endif
const bool kDefaultAgcState =
#if defined(VOIP_ANDROID) || defined(VOIP_IOS)
  false;
#else
  true;
#endif
const GainControl::Mode kDefaultRxAgcMode = GainControl::kAdaptiveDigital;

}  // namespace VoIP

// ----------------------------------------------------------------------------
//  Build information macros
// ----------------------------------------------------------------------------

#if defined(_DEBUG)
#define BUILDMODE "d"
#elif defined(DEBUG)
#define BUILDMODE "d"
#elif defined(NDEBUG)
#define BUILDMODE "r"
#else
#define BUILDMODE "?"
#endif

#define BUILDTIME __TIME__
#define BUILDDATE __DATE__

// Example: "Oct 10 2002 12:05:30 r"
#define BUILDINFO BUILDDATE " " BUILDTIME " " BUILDMODE

// ----------------------------------------------------------------------------
//  Macros
// ----------------------------------------------------------------------------

#define NOT_SUPPORTED(stat)                  \
  LOG_F(LS_ERROR) << "not supported";        \
  stat.SetLastError(VE_FUNC_NOT_SUPPORTED);  \
  return -1;

#if (defined(_DEBUG) && defined(_WIN32) && (_MSC_VER >= 1400))
  #include <windows.h>
  #include <stdio.h>
  #define DEBUG_PRINT(...)      \
  {                             \
    char msg[256];              \
    sprintf(msg, __VA_ARGS__);  \
    OutputDebugStringA(msg);    \
  }
#else
  // special fix for visual 2003
  #define DEBUG_PRINT(exp)      ((void)0)
#endif  // defined(_DEBUG) && defined(_WIN32)

#define CHECK_CHANNEL(channel)  if (CheckChannel(channel) == -1) return -1;

// ----------------------------------------------------------------------------
//  Inline functions
// ----------------------------------------------------------------------------

namespace VoIP
{

inline int AoEId(int veId, int chId)
{
    if (chId == -1)
    {
        const int dummyChannel(99);
        return (int) ((veId << 16) + dummyChannel);
    }
    return (int) ((veId << 16) + chId);
}

inline int AoEModuleId(int veId, int chId)
{
    return (int) ((veId << 16) + chId);
}

// Convert module ID to internal AoE channel ID
inline int AoEChannelId(int moduleId)
{
    return (int) (moduleId & 0xffff);
}

}  // namespace VoIP

// ----------------------------------------------------------------------------
//  Platform settings
// ----------------------------------------------------------------------------

// *** WINDOWS ***

#if defined(_WIN32)

  #include <windows.h>

  #pragma comment( lib, "winmm.lib" )

  #ifndef VOIP_EXTERNAL_TRANSPORT
    #pragma comment( lib, "ws2_32.lib" )
  #endif

// ----------------------------------------------------------------------------
//  Defines
// ----------------------------------------------------------------------------

// Default device for Windows PC
  #define VOIP_AUDIO_ENGINE_DEFAULT_DEVICE \
    AudioDeviceModule::kDefaultCommunicationDevice

#endif  // #if (defined(_WIN32)

// *** LINUX ***

#ifdef VOIP_LINUX

#include <arpa/inet.h>
#include <netinet/in.h>
#include <pthread.h>
#include <sys/socket.h>
#include <sys/types.h>
#ifndef QNX
  #include <linux/net.h>
#ifndef ANDROID
  #include <sys/soundcard.h>
#endif // ANDROID
#endif // QNX
#include <errno.h>
#include <fcntl.h>
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>

#define DWORD unsigned long int
#define WINAPI
#define LPVOID void *
#define FALSE 0
#define TRUE 1
#define UINT unsigned int
#define UCHAR unsigned char
#define TCHAR char
#ifdef QNX
#define _stricmp stricmp
#else
#define _stricmp strcasecmp
#endif
#define GetLastError() errno
#define WSAGetLastError() errno
#define LPCTSTR const char*
#define LPCSTR const char*
#define wsprintf sprintf
#define TEXT(a) a
#define _ftprintf fprintf
#define _tcslen strlen
#define FAR
#define __cdecl
#define LPSOCKADDR struct sockaddr *

// Default device for Linux and Android
#define VOIP_AUDIO_ENGINE_DEFAULT_DEVICE 0

#endif  // #ifdef VOIP_LINUX

// *** VOIP_MAC ***
// including iPhone

#ifdef VOIP_MAC

#include <AudioUnit/AudioUnit.h>
#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <pthread.h>
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>
#if !defined(VOIP_IOS)
  #include <CoreServices/CoreServices.h>
  #include <CoreAudio/CoreAudio.h>
  #include <AudioToolbox/DefaultAudioOutput.h>
  #include <AudioToolbox/AudioConverter.h>
  #include <CoreAudio/HostTime.h>
#endif

#define DWORD unsigned long int
#define WINAPI
#define LPVOID void *
#define FALSE 0
#define TRUE 1
#define SOCKADDR_IN struct sockaddr_in
#define UINT unsigned int
#define UCHAR unsigned char
#define TCHAR char
#define _stricmp strcasecmp
#define GetLastError() errno
#define WSAGetLastError() errno
#define LPCTSTR const char*
#define wsprintf sprintf
#define TEXT(a) a
#define _ftprintf fprintf
#define _tcslen strlen
#define FAR
#define __cdecl
#define LPSOCKADDR struct sockaddr *
#define LPCSTR const char*
#define ULONG unsigned long

// Default device for Mac and iPhone
#define VOIP_AUDIO_ENGINE_DEFAULT_DEVICE 0
#endif  // #ifdef VOIP_MAC

#endif // VOIP_AUDIO_ENGINE_AUDIO_ENGINE_DEFINES_H
