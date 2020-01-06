#ifndef VOIP_MODULES_MEDIA_FILE_INTERFACE_MEDIA_FILE_DEFINES_H_
#define VOIP_MODULES_MEDIA_FILE_INTERFACE_MEDIA_FILE_DEFINES_H_

#include "audio_engine/include/engine_configurations.h"
#include "audio_engine/modules/interface/module_common_types.h"
#include "audio_engine/include/typedefs.h"

namespace VoIP {
// Callback class for the MediaFile class.
class FileCallback
{
public:
    virtual ~FileCallback(){}

    // This function is called by MediaFile when a file has been playing for
    // durationMs ms. id is the identifier for the MediaFile instance calling
    // the callback.
    virtual void PlayNotification(const int32_t id,
                                  const uint32_t durationMs) = 0;

    // This function is called by MediaFile when a file has been recording for
    // durationMs ms. id is the identifier for the MediaFile instance calling
    // the callback.
    virtual void RecordNotification(const int32_t id,
                                    const uint32_t durationMs) = 0;

    // This function is called by MediaFile when a file has been stopped
    // playing. id is the identifier for the MediaFile instance calling the
    // callback.
    virtual void PlayFileEnded(const int32_t id) = 0;

    // This function is called by MediaFile when a file has been stopped
    // recording. id is the identifier for the MediaFile instance calling the
    // callback.
    virtual void RecordFileEnded(const int32_t id) = 0;

protected:
    FileCallback() {}
};
}  // namespace VoIP
#endif // VOIP_MODULES_MEDIA_FILE_INTERFACE_MEDIA_FILE_DEFINES_H_
