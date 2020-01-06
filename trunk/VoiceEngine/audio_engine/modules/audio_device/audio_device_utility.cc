#include <assert.h>

#include "audio_engine/modules/audio_device/audio_device_utility.h"

#if defined(_WIN32)

// ============================================================================
//                                     Windows
// ============================================================================

#include <windows.h>
#include <conio.h>
#include <ctype.h>
#include <stdio.h>
#include <mmsystem.h>

namespace VoIP
{

void AudioDeviceUtility::WaitForKey()
{
	_getch();
}

uint32_t AudioDeviceUtility::GetTimeInMS()
{
	return timeGetTime();
}

bool AudioDeviceUtility::StringCompare(
    const char* str1 , const char* str2,
    const uint32_t length)
{
	return ((_strnicmp(str1, str2, length) == 0) ? true : false);
}

}  // namespace VoIP

#elif defined(VOIP_LINUX) || defined(VOIP_MAC)

// ============================================================================
//                                 Linux & Mac
// ============================================================================

#include <stdio.h>      // getchar
#include <string.h>     // strncasecmp
#include <sys/time.h>   // gettimeofday
#include <termios.h>    // tcgetattr
#include <time.h>       // gettimeofday

#include <unistd.h>

namespace VoIP
{

void AudioDeviceUtility::WaitForKey()
{

    struct termios oldt, newt;

    tcgetattr( STDIN_FILENO, &oldt );

    // we don't want getchar to echo!

    newt = oldt;
    newt.c_lflag &= ~( ICANON | ECHO );
    tcsetattr( STDIN_FILENO, TCSANOW, &newt );

    // catch any newline that's hanging around...

    // you'll have to hit enter twice if you

    // choose enter out of all available keys

    if (getchar() == '\n')
    {
        getchar();
    }

    tcsetattr( STDIN_FILENO, TCSANOW, &oldt );
}

uint32_t AudioDeviceUtility::GetTimeInMS()
{
    struct timeval tv;
    struct timezone tz;
    uint32_t val;

    gettimeofday(&tv, &tz);
    val = (uint32_t)(tv.tv_sec*1000 + tv.tv_usec/1000);
    return val;
}

bool AudioDeviceUtility::StringCompare(
    const char* str1 , const char* str2, const uint32_t length)
{
    return (strncasecmp(str1, str2, length) == 0)?true: false;
}

}  // namespace VoIP

#endif  // defined(VOIP_LINUX) || defined(VOIP_MAC)
