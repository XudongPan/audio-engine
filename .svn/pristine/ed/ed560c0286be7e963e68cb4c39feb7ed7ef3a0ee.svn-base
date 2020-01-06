#ifndef VOIP_MODULES_UTILITY_SOURCE_FILE_PLAYER_IMPL_H_
#define VOIP_MODULES_UTILITY_SOURCE_FILE_PLAYER_IMPL_H_

#include "audio_engine/common_audio/resampler/include/resampler.h"
#include "audio_engine/include/common_types.h"
#include "audio_engine/include/engine_configurations.h"
#include "audio_engine/modules/media_file/interface/media_file.h"
#include "audio_engine/modules/media_file/interface/media_file_defines.h"
#include "audio_engine/modules/utility/interface/file_player.h"
#include "audio_engine/system_wrappers/interface/critical_section_wrapper.h"
#include "audio_engine/system_wrappers/interface/tick_util.h"
#include "audio_engine/include/typedefs.h"

namespace VoIP {
class VideoCoder;
class FrameScaler;

class FilePlayerImpl : public FilePlayer
{
public:
    FilePlayerImpl(uint32_t instanceID, FileFormats fileFormat);
    ~FilePlayerImpl();

    virtual int Get10msAudioFromFile(
        int16_t* outBuffer,
        int& lengthInSamples,
        int frequencyInHz);
    virtual int32_t RegisterModuleFileCallback(FileCallback* callback);
    virtual int32_t StartPlayingFile(
        const char* fileName,
        bool loop,
        uint32_t startPosition,
        float volumeScaling,
        uint32_t notification,
        uint32_t stopPosition = 0,
        const CodecInst* codecInst = NULL);
    virtual int32_t StartPlayingFile(
        InStream& sourceStream,
        uint32_t startPosition,
        float volumeScaling,
        uint32_t notification,
        uint32_t stopPosition = 0,
        const CodecInst* codecInst = NULL);
    virtual int32_t StopPlayingFile();
    virtual bool IsPlayingFile() const;
    virtual int32_t GetPlayoutPosition(uint32_t& durationMs);
    virtual int32_t Frequency() const;
    virtual int32_t SetAudioScaling(float scaleFactor);

protected:
    int32_t SetUpAudioDecoder();

    uint32_t _instanceID;
    const FileFormats _fileFormat;
    MediaFile& _fileModule;

    uint32_t _decodedLengthInMS;

private:
    int32_t _numberOf10MsPerFrame;
    int32_t _numberOf10MsInDecoder;

    Resampler _resampler;
    float _scaling;
};

}  // namespace VoIP
#endif // VOIP_MODULES_UTILITY_SOURCE_FILE_PLAYER_IMPL_H_
