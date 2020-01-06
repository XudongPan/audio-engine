#include "audio_engine/modules/utility/source/file_player_impl.h"
#include "audio_engine/system_wrappers/interface/logging.h"


namespace VoIP {
FilePlayer* FilePlayer::CreateFilePlayer(uint32_t instanceID,
                                         FileFormats fileFormat)
{
    switch(fileFormat)
    {
    case kFileFormatWavFile:
    case kFileFormatCompressedFile:
    case kFileFormatPreencodedFile:
    case kFileFormatPcm16kHzFile:
    case kFileFormatPcm8kHzFile:
    case kFileFormatPcm32kHzFile:
        // audio formats
        return new FilePlayerImpl(instanceID, fileFormat);
 
    }
    assert(false);
    return NULL;
}

void FilePlayer::DestroyFilePlayer(FilePlayer* player)
{
    delete player;
}

FilePlayerImpl::FilePlayerImpl(const uint32_t instanceID,
                               const FileFormats fileFormat)
    : _instanceID(instanceID),
      _fileFormat(fileFormat),
      _fileModule(*MediaFile::CreateMediaFile(instanceID)),
      _decodedLengthInMS(0),
      _numberOf10MsPerFrame(0),
      _numberOf10MsInDecoder(0),
      _resampler(),
      _scaling(1.0)
{
    
}

FilePlayerImpl::~FilePlayerImpl()
{
    MediaFile::DestroyMediaFile(&_fileModule);
}

int32_t FilePlayerImpl::Frequency() const
{
	return 8000;
}


int32_t FilePlayerImpl::Get10msAudioFromFile(
    int16_t* outBuffer,
    int& lengthInSamples,
    int frequencyInHz)
{


    AudioFrame unresampledAudioFrame;
   
    int outLen = 0;
    if(_resampler.ResetIfNeeded(unresampledAudioFrame.sample_rate_hz_,
                                frequencyInHz, kResamplerSynchronous))
    {
        LOG(LS_WARNING) << "Get10msAudioFromFile() unexpected codec.";

        // New sampling frequency. Update state.
        outLen = frequencyInHz / 100;
        memset(outBuffer, 0, outLen * sizeof(int16_t));
        return 0;
    }
    _resampler.Push(unresampledAudioFrame.data_,
                    unresampledAudioFrame.samples_per_channel_,
                    outBuffer,
                    MAX_AUDIO_BUFFER_IN_SAMPLES,
                    outLen);

    lengthInSamples = outLen;

    if(_scaling != 1.0)
    {
        for (int i = 0;i < outLen; i++)
        {
            outBuffer[i] = (int16_t)(outBuffer[i] * _scaling);
        }
    }
    _decodedLengthInMS += 10;
    return 0;
}

int32_t FilePlayerImpl::RegisterModuleFileCallback(FileCallback* callback)
{
    return _fileModule.SetModuleFileCallback(callback);
}

int32_t FilePlayerImpl::SetAudioScaling(float scaleFactor)
{
    if((scaleFactor >= 0)&&(scaleFactor <= 2.0))
    {
        _scaling = scaleFactor;
        return 0;
    }
    LOG(LS_WARNING) << "SetAudioScaling() non-allowed scale factor.";
    return -1;
}

int32_t FilePlayerImpl::StartPlayingFile(const char* fileName,
                                         bool loop,
                                         uint32_t startPosition,
                                         float volumeScaling,
                                         uint32_t notification,
                                         uint32_t stopPosition,
                                         const CodecInst* codecInst)
{
    if (_fileFormat == kFileFormatPcm16kHzFile ||
        _fileFormat == kFileFormatPcm8kHzFile||
        _fileFormat == kFileFormatPcm32kHzFile )
    {
        CodecInst codecInstL16;
        strncpy(codecInstL16.plname,"L16",32);
        codecInstL16.pltype   = 93;
        codecInstL16.channels = 1;

        if (_fileFormat == kFileFormatPcm8kHzFile)
        {
            codecInstL16.rate     = 128000;
            codecInstL16.plfreq   = 8000;
            codecInstL16.packsize  = 80;

        } else if(_fileFormat == kFileFormatPcm16kHzFile)
        {
            codecInstL16.rate     = 256000;
            codecInstL16.plfreq   = 16000;
            codecInstL16.packsize  = 160;

        }else if(_fileFormat == kFileFormatPcm32kHzFile)
        {
            codecInstL16.rate     = 512000;
            codecInstL16.plfreq   = 32000;
            codecInstL16.packsize  = 160;
        } else
        {
            LOG(LS_ERROR) << "StartPlayingFile() sample frequency not "
                          << "supported for PCM format.";
            return -1;
        }

        if (_fileModule.StartPlayingAudioFile(fileName, notification, loop,
                                              _fileFormat, &codecInstL16,
                                              startPosition,
                                              stopPosition) == -1)
        {
            LOG(LS_WARNING) << "StartPlayingFile() failed to initialize "
                            << "pcm file " << fileName;
            return -1;
        }
        SetAudioScaling(volumeScaling);
    }else if(_fileFormat == kFileFormatPreencodedFile)
    {
        if (_fileModule.StartPlayingAudioFile(fileName, notification, loop,
                                              _fileFormat, codecInst) == -1)
        {
            LOG(LS_WARNING) << "StartPlayingFile() failed to initialize "
                            << "pre-encoded file " << fileName;
            return -1;
        }
    } else
    {
        CodecInst* no_inst = NULL;
        if (_fileModule.StartPlayingAudioFile(fileName, notification, loop,
                                              _fileFormat, no_inst,
                                              startPosition,
                                              stopPosition) == -1)
        {
            LOG(LS_WARNING) << "StartPlayingFile() failed to initialize file "
                            << fileName;
            return -1;
        }
        SetAudioScaling(volumeScaling);
    }
    if (SetUpAudioDecoder() == -1)
    {
        StopPlayingFile();
        return -1;
    }
    return 0;
}

int32_t FilePlayerImpl::StartPlayingFile(InStream& sourceStream,
                                         uint32_t startPosition,
                                         float volumeScaling,
                                         uint32_t notification,
                                         uint32_t stopPosition,
                                         const CodecInst* codecInst)
{
    if (_fileFormat == kFileFormatPcm16kHzFile ||
        _fileFormat == kFileFormatPcm32kHzFile ||
        _fileFormat == kFileFormatPcm8kHzFile)
    {
        CodecInst codecInstL16;
        strncpy(codecInstL16.plname,"L16",32);
        codecInstL16.pltype   = 93;
        codecInstL16.channels = 1;

        if (_fileFormat == kFileFormatPcm8kHzFile)
        {
            codecInstL16.rate     = 128000;
            codecInstL16.plfreq   = 8000;
            codecInstL16.packsize  = 80;

        }else if (_fileFormat == kFileFormatPcm16kHzFile)
        {
            codecInstL16.rate     = 256000;
            codecInstL16.plfreq   = 16000;
            codecInstL16.packsize  = 160;

        }else if (_fileFormat == kFileFormatPcm32kHzFile)
        {
            codecInstL16.rate     = 512000;
            codecInstL16.plfreq   = 32000;
            codecInstL16.packsize  = 160;
        }else
        {
            LOG(LS_ERROR) << "StartPlayingFile() sample frequency not "
                          << "supported for PCM format.";
            return -1;
        }
        if (_fileModule.StartPlayingAudioStream(sourceStream, notification,
                                                _fileFormat, &codecInstL16,
                                                startPosition,
                                                stopPosition) == -1)
        {
            LOG(LS_ERROR) << "StartPlayingFile() failed to initialize stream "
                          << "playout.";
            return -1;
        }

    }else if(_fileFormat == kFileFormatPreencodedFile)
    {
        if (_fileModule.StartPlayingAudioStream(sourceStream, notification,
                                                _fileFormat, codecInst) == -1)
        {
            LOG(LS_ERROR) << "StartPlayingFile() failed to initialize stream "
                          << "playout.";
            return -1;
        }
    } else {
        CodecInst* no_inst = NULL;
        if (_fileModule.StartPlayingAudioStream(sourceStream, notification,
                                                _fileFormat, no_inst,
                                                startPosition,
                                                stopPosition) == -1)
        {
            LOG(LS_ERROR) << "StartPlayingFile() failed to initialize stream "
                          << "playout.";
            return -1;
        }
    }
    SetAudioScaling(volumeScaling);

    if (SetUpAudioDecoder() == -1)
    {
        StopPlayingFile();
        return -1;
    }
    return 0;
}

int32_t FilePlayerImpl::StopPlayingFile()
{
    
    _numberOf10MsPerFrame  = 0;
    _numberOf10MsInDecoder = 0;
    return _fileModule.StopPlaying();
}

bool FilePlayerImpl::IsPlayingFile() const
{
    return _fileModule.IsPlaying();
}

int32_t FilePlayerImpl::GetPlayoutPosition(uint32_t& durationMs)
{
    return _fileModule.PlayoutPositionMs(durationMs);
}

int32_t FilePlayerImpl::SetUpAudioDecoder()
{
    _numberOf10MsInDecoder = 0;
    return 0;
}

}  // namespace VoIP
