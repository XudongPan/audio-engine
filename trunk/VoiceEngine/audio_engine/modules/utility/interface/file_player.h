#ifndef VOIP_MODULES_UTILITY_INTERFACE_FILE_PLAYER_H_
#define VOIP_MODULES_UTILITY_INTERFACE_FILE_PLAYER_H_

#include "audio_engine/include/common_types.h"
#include "audio_engine/include/engine_configurations.h"
#include "audio_engine/modules/interface/module_common_types.h"
#include "audio_engine/include/typedefs.h"

namespace VoIP {
class FileCallback;

class FilePlayer
{
public:
    // The largest decoded frame size in samples (60ms with 32kHz sample rate).
    enum {MAX_AUDIO_BUFFER_IN_SAMPLES = 60*32};
    enum {MAX_AUDIO_BUFFER_IN_BYTES = MAX_AUDIO_BUFFER_IN_SAMPLES*2};

    static FilePlayer* CreateFilePlayer(const uint32_t instanceID,
                                        const FileFormats fileFormat);

    static void DestroyFilePlayer(FilePlayer* player);

    // Read 10 ms of audio at |frequencyInHz| to |outBuffer|. |lengthInSamples|
    // will be set to the number of samples read (not the number of samples per
    // channel).
    virtual int Get10msAudioFromFile(
        int16_t* outBuffer,
        int& lengthInSamples,
        int frequencyInHz) = 0;

    // Register callback for receiving file playing notifications.
    virtual int32_t RegisterModuleFileCallback(
        FileCallback* callback) = 0;

    // API for playing audio from fileName to channel.
    // Note: codecInst is used for pre-encoded files.
    virtual int32_t StartPlayingFile(
        const char* fileName,
        bool loop,
        uint32_t startPosition,
        float volumeScaling,
        uint32_t notification,
        uint32_t stopPosition = 0,
        const CodecInst* codecInst = NULL) = 0;

    // Note: codecInst is used for pre-encoded files.
    virtual int32_t StartPlayingFile(
        InStream& sourceStream,
        uint32_t startPosition,
        float volumeScaling,
        uint32_t notification,
        uint32_t stopPosition = 0,
        const CodecInst* codecInst = NULL) = 0;

    virtual int32_t StopPlayingFile() = 0;

    virtual bool IsPlayingFile() const = 0;

    virtual int32_t GetPlayoutPosition(uint32_t& durationMs) = 0;

    virtual int32_t Frequency() const = 0;

    // Note: scaleFactor is in the range [0.0 - 2.0]
    virtual int32_t SetAudioScaling(float scaleFactor) = 0;

   
protected:
    virtual ~FilePlayer() {}

};
}  // namespace VoIP
#endif // VOIP_MODULES_UTILITY_INTERFACE_FILE_PLAYER_H_
