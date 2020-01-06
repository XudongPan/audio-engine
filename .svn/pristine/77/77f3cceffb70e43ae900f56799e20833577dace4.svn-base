
#include <assert.h>

#include "audio_engine/modules/media_file/source/media_file_impl.h"
#include "audio_engine/system_wrappers/interface/critical_section_wrapper.h"
#include "audio_engine/system_wrappers/interface/file_wrapper.h"
#include "audio_engine/system_wrappers/interface/tick_util.h"
#include "audio_engine/system_wrappers/interface/trace.h"

namespace VoIP {
MediaFile* MediaFile::CreateMediaFile(const int32_t id)
{
    return new MediaFileImpl(id);
}

void MediaFile::DestroyMediaFile(MediaFile* module)
{
    delete static_cast<MediaFileImpl*>(module);
}

MediaFileImpl::MediaFileImpl(const int32_t id)
    : _id(id),
      _crit(CriticalSectionWrapper::CreateCriticalSection()),
      _callbackCrit(CriticalSectionWrapper::CreateCriticalSection()),
      _ptrFileUtilityObj(NULL),
      codec_info_(),
      _ptrInStream(NULL),
      _ptrOutStream(NULL),
      _fileFormat((FileFormats)-1),
      _recordDurationMs(0),
      _playoutPositionMs(0),
      _notificationMs(0),
      _playingActive(false),
      _recordingActive(false),
      _isStereo(false),
      _openFile(false),
      _fileName(),
      _ptrCallback(NULL)
{
    VOIP_TRACE(kTraceMemory, kTraceFile, id, "Created");

    codec_info_.plname[0] = '\0';
    _fileName[0] = '\0';
}


MediaFileImpl::~MediaFileImpl()
{
    VOIP_TRACE(kTraceMemory, kTraceFile, _id, "~MediaFileImpl()");
    {
        CriticalSectionScoped lock(_crit);

        if(_playingActive)
        {
            StopPlaying();
        }

        if(_recordingActive)
        {
            StopRecording();
        }

        delete _ptrFileUtilityObj;

        if(_openFile)
        {
            delete _ptrInStream;
            _ptrInStream = NULL;
            delete _ptrOutStream;
            _ptrOutStream = NULL;
        }
    }

    delete _crit;
    delete _callbackCrit;
}

int32_t MediaFileImpl::ChangeUniqueId(const int32_t id)
{
    _id = id;
    return 0;
}

int32_t MediaFileImpl::TimeUntilNextProcess()
{
    VOIP_TRACE(
        kTraceWarning,
        kTraceFile,
        _id,
        "TimeUntilNextProcess: This method is not used by MediaFile class.");
    return -1;
}

int32_t MediaFileImpl::Process()
{
    VOIP_TRACE(kTraceWarning, kTraceFile, _id,
                 "Process: This method is not used by MediaFile class.");
    return -1;
}


int32_t MediaFileImpl::PlayoutAudioData(int8_t* buffer,
                                        uint32_t& dataLengthInBytes)
{
    return PlayoutData( buffer, dataLengthInBytes, false);
}

int32_t MediaFileImpl::PlayoutData(int8_t* buffer, uint32_t& dataLengthInBytes,
                                   bool video)
{
    VOIP_TRACE(kTraceStream, kTraceFile, _id,
               "MediaFileImpl::PlayoutData(buffer= 0x%x, bufLen= %ld)",
                 buffer, dataLengthInBytes);

    const uint32_t bufferLengthInBytes = dataLengthInBytes;
    dataLengthInBytes = 0;

    if(buffer == NULL || bufferLengthInBytes == 0)
    {
        VOIP_TRACE(kTraceError, kTraceFile, _id,
                     "Buffer pointer or length is NULL!");
        return -1;
    }

    int32_t bytesRead = 0;
    {
        CriticalSectionScoped lock(_crit);

        if(!_playingActive)
        {
            VOIP_TRACE(kTraceWarning, kTraceFile, _id,
                         "Not currently playing!");
            return -1;
        }

        if(!_ptrFileUtilityObj)
        {
            VOIP_TRACE(kTraceError, kTraceFile, _id,
                         "Playing, but no FileUtility object!");
            StopPlaying();
            return -1;
        }

        switch(_fileFormat)
        {
            case kFileFormatPcm32kHzFile:
            case kFileFormatPcm16kHzFile:
            case kFileFormatPcm8kHzFile:
                bytesRead = _ptrFileUtilityObj->ReadPCMData(
                    *_ptrInStream,
                    buffer,
                    bufferLengthInBytes);
                break;
            case kFileFormatCompressedFile:
                bytesRead = _ptrFileUtilityObj->ReadCompressedData(
                    *_ptrInStream,
                    buffer,
                    bufferLengthInBytes);
                break;
            case kFileFormatWavFile:
                bytesRead = _ptrFileUtilityObj->ReadWavDataAsMono(
                    *_ptrInStream,
                    buffer,
                    bufferLengthInBytes);
                break;
            case kFileFormatPreencodedFile:
                bytesRead = _ptrFileUtilityObj->ReadPreEncodedData(
                    *_ptrInStream,
                    buffer,
                    bufferLengthInBytes);
                if(bytesRead > 0)
                {
                    dataLengthInBytes = bytesRead;
                    return 0;
                }
                break;
         }

        if( bytesRead > 0)
        {
            dataLengthInBytes =(uint32_t) bytesRead;
        }
    }
    HandlePlayCallbacks(bytesRead);
    return 0;
}

void MediaFileImpl::HandlePlayCallbacks(int32_t bytesRead)
{
    bool playEnded = false;
    uint32_t callbackNotifyMs = 0;

    if(bytesRead > 0)
    {
        // Check if it's time for PlayNotification(..).
        _playoutPositionMs = _ptrFileUtilityObj->PlayoutPositionMs();
        if(_notificationMs)
        {
            if(_playoutPositionMs >= _notificationMs)
            {
                _notificationMs = 0;
                callbackNotifyMs = _playoutPositionMs;
            }
        }
    }
    else
    {
        // If no bytes were read assume end of file.
        StopPlaying();
        playEnded = true;
    }

    // Only _callbackCrit may and should be taken when making callbacks.
    CriticalSectionScoped lock(_callbackCrit);
    if(_ptrCallback)
    {
        if(callbackNotifyMs)
        {
            _ptrCallback->PlayNotification(_id, callbackNotifyMs);
        }
        if(playEnded)
        {
            _ptrCallback->PlayFileEnded(_id);
        }
    }
}

int32_t MediaFileImpl::PlayoutStereoData(
    int8_t* bufferLeft,
    int8_t* bufferRight,
    uint32_t& dataLengthInBytes)
{
    VOIP_TRACE(kTraceStream, kTraceFile, _id,
                 "MediaFileImpl::PlayoutStereoData(Left = 0x%x, Right = 0x%x,\
 Len= %ld)",
                 bufferLeft,
                 bufferRight,
                 dataLengthInBytes);

    const uint32_t bufferLengthInBytes = dataLengthInBytes;
    dataLengthInBytes = 0;

    if(bufferLeft == NULL || bufferRight == NULL || bufferLengthInBytes == 0)
    {
        VOIP_TRACE(kTraceError, kTraceFile, _id,
                     "A buffer pointer or the length is NULL!");
        return -1;
    }

    bool playEnded = false;
    uint32_t callbackNotifyMs = 0;
    {
        CriticalSectionScoped lock(_crit);

        if(!_playingActive || !_isStereo)
        {
            VOIP_TRACE(kTraceWarning, kTraceFile, _id,
                         "Not currently playing stereo!");
            return -1;
        }

        if(!_ptrFileUtilityObj)
        {
            VOIP_TRACE(
                kTraceError,
                kTraceFile,
                _id,
                "Playing stereo, but the FileUtility objects is NULL!");
            StopPlaying();
            return -1;
        }

        // Stereo playout only supported for WAV files.
        int32_t bytesRead = 0;
        switch(_fileFormat)
        {
            case kFileFormatWavFile:
                    bytesRead = _ptrFileUtilityObj->ReadWavDataAsStereo(
                        *_ptrInStream,
                        bufferLeft,
                        bufferRight,
                        bufferLengthInBytes);
                    break;
            default:
                VOIP_TRACE(kTraceError, kTraceFile, _id,
                             "Trying to read non-WAV as stereo audio\
 (not supported)");
                break;
        }

        if(bytesRead > 0)
        {
            dataLengthInBytes = bytesRead;

            // Check if it's time for PlayNotification(..).
            _playoutPositionMs = _ptrFileUtilityObj->PlayoutPositionMs();
            if(_notificationMs)
            {
                if(_playoutPositionMs >= _notificationMs)
                {
                    _notificationMs = 0;
                    callbackNotifyMs = _playoutPositionMs;
                }
            }
        }
        else
        {
            // If no bytes were read assume end of file.
            StopPlaying();
            playEnded = true;
        }
    }

    CriticalSectionScoped lock(_callbackCrit);
    if(_ptrCallback)
    {
        if(callbackNotifyMs)
        {
            _ptrCallback->PlayNotification(_id, callbackNotifyMs);
        }
        if(playEnded)
        {
            _ptrCallback->PlayFileEnded(_id);
        }
    }
    return 0;
}

int32_t MediaFileImpl::StartPlayingAudioFile(
    const char* fileName,
    const uint32_t notificationTimeMs,
    const bool loop,
    const FileFormats format,
    const CodecInst* codecInst,
    const uint32_t startPointMs,
    const uint32_t stopPointMs)
{
    const bool videoOnly = false;
    return StartPlayingFile(fileName, notificationTimeMs, loop, videoOnly,
                            format, codecInst, startPointMs, stopPointMs);
}

int32_t MediaFileImpl::StartPlayingFile(
    const char* fileName,
    const uint32_t notificationTimeMs,
    const bool loop,
    bool videoOnly,
    const FileFormats format,
    const CodecInst* codecInst,
    const uint32_t startPointMs,
    const uint32_t stopPointMs)
{

    if(!ValidFileName(fileName))
    {
        return -1;
    }
    if(!ValidFileFormat(format,codecInst))
    {
        return -1;
    }
    if(!ValidFilePositions(startPointMs,stopPointMs))
    {
        return -1;
    }

    // Check that the file will play longer than notificationTimeMs ms.
    if((startPointMs && stopPointMs && !loop) &&
       (notificationTimeMs > (stopPointMs - startPointMs)))
    {
        VOIP_TRACE(
            kTraceError,
            kTraceFile,
            _id,
            "specified notification time is longer than amount of ms that will\
 be played");
        return -1;
    }

    FileWrapper* inputStream = FileWrapper::Create();
    if(inputStream == NULL)
    {
       VOIP_TRACE(kTraceMemory, kTraceFile, _id,
                    "Failed to allocate input stream for file %s", fileName);
        return -1;
    }

    bool useStream = true;
    if( useStream)
    {
        if(inputStream->OpenFile(fileName, true, loop) != 0)
        {
            delete inputStream;
            VOIP_TRACE(kTraceError, kTraceFile, _id,
                         "Could not open input file %s", fileName);
            return -1;
        }
    }

    if(StartPlayingStream(*inputStream, fileName, loop, notificationTimeMs,
                          format, codecInst, startPointMs, stopPointMs,
                          videoOnly) == -1)
    {
        if( useStream)
        {
            inputStream->CloseFile();
        }
        delete inputStream;
        return -1;
    }

    CriticalSectionScoped lock(_crit);
    _openFile = true;
    strncpy(_fileName, fileName, sizeof(_fileName));
    _fileName[sizeof(_fileName) - 1] = '\0';
    return 0;
}

int32_t MediaFileImpl::StartPlayingAudioStream(
    InStream& stream,
    const uint32_t notificationTimeMs,
    const FileFormats format,
    const CodecInst* codecInst,
    const uint32_t startPointMs,
    const uint32_t stopPointMs)
{
    return StartPlayingStream(stream, 0, false, notificationTimeMs, format,
                              codecInst, startPointMs, stopPointMs);
}

int32_t MediaFileImpl::StartPlayingStream(
    InStream& stream,
    const char* filename,
    bool loop,
    const uint32_t notificationTimeMs,
    const FileFormats format,
    const CodecInst*  codecInst,
    const uint32_t startPointMs,
    const uint32_t stopPointMs,
    bool videoOnly)
{
    if(!ValidFileFormat(format,codecInst))
    {
        return -1;
    }

    if(!ValidFilePositions(startPointMs,stopPointMs))
    {
        return -1;
    }

    CriticalSectionScoped lock(_crit);
    if(_playingActive || _recordingActive)
    {
        VOIP_TRACE(
            kTraceError,
            kTraceFile,
            _id,
            "StartPlaying called, but already playing or recording file %s",
            (_fileName[0] == '\0') ? "(name not set)" : _fileName);
        return -1;
    }

    if(_ptrFileUtilityObj != NULL)
    {
        VOIP_TRACE(kTraceError,
                     kTraceFile,
                     _id,
                     "StartPlaying called, but FileUtilityObj already exists!");
        StopPlaying();
        return -1;
    }

    _ptrFileUtilityObj = new ModuleFileUtility(_id);
    if(_ptrFileUtilityObj == NULL)
    {
        VOIP_TRACE(kTraceMemory, kTraceFile, _id,
                     "Failed to create FileUtilityObj!");
        return -1;
    }

    switch(format)
    {
        case kFileFormatWavFile:
        {
            if(_ptrFileUtilityObj->InitWavReading(stream, startPointMs,
                                                  stopPointMs) == -1)
            {
                VOIP_TRACE(kTraceError, kTraceFile, _id,
                             "Not a valid WAV file!");
                StopPlaying();
                return -1;
            }
            _fileFormat = kFileFormatWavFile;
            break;
        }
        case kFileFormatCompressedFile:
        {
            if(_ptrFileUtilityObj->InitCompressedReading(stream, startPointMs,
                                                         stopPointMs) == -1)
            {
                VOIP_TRACE(kTraceError, kTraceFile, _id,
                             "Not a valid Compressed file!");
                StopPlaying();
                return -1;
            }
            _fileFormat = kFileFormatCompressedFile;
            break;
        }
        case kFileFormatPcm8kHzFile:
        case kFileFormatPcm16kHzFile:
        case kFileFormatPcm32kHzFile:
        {
            // ValidFileFormat() called in the beginneing of this function
            // prevents codecInst from being NULL here.
            assert(codecInst != NULL);
            if(!ValidFrequency(codecInst->plfreq) ||
               _ptrFileUtilityObj->InitPCMReading(stream, startPointMs,
                                                  stopPointMs,
                                                  codecInst->plfreq) == -1)
            {
                VOIP_TRACE(kTraceError, kTraceFile, _id,
                             "Not a valid raw 8 or 16 KHz PCM file!");
                StopPlaying();
                return -1;
            }

            _fileFormat = format;
            break;
        }
        case kFileFormatPreencodedFile:
        {
            // ValidFileFormat() called in the beginneing of this function
            // prevents codecInst from being NULL here.
            assert(codecInst != NULL);
            if(_ptrFileUtilityObj->InitPreEncodedReading(stream, *codecInst) ==
               -1)
            {
                VOIP_TRACE(kTraceError, kTraceFile, _id,
                             "Not a valid PreEncoded file!");
                StopPlaying();
                return -1;
            }

            _fileFormat = kFileFormatPreencodedFile;
            break;
        }
    }
    if(_ptrFileUtilityObj->codec_info(codec_info_) == -1)
    {
        VOIP_TRACE(kTraceError, kTraceFile, _id,
                     "Failed to retrieve codec info!");
        StopPlaying();
        return -1;
    }

    _isStereo = (codec_info_.channels == 2);
    if(_isStereo && (_fileFormat != kFileFormatWavFile))
    {
        VOIP_TRACE(kTraceWarning, kTraceFile, _id,
                     "Stereo is only allowed for WAV files");
        StopPlaying();
        return -1;
    }
    _playingActive = true;
    _playoutPositionMs = _ptrFileUtilityObj->PlayoutPositionMs();
    _ptrInStream = &stream;
    _notificationMs = notificationTimeMs;

    return 0;
}

int32_t MediaFileImpl::StopPlaying()
{

    CriticalSectionScoped lock(_crit);
    _isStereo = false;
    if(_ptrFileUtilityObj)
    {
        delete _ptrFileUtilityObj;
        _ptrFileUtilityObj = NULL;
    }
    if(_ptrInStream)
    {
        // If MediaFileImpl opened the InStream it must be reclaimed here.
        if(_openFile)
        {
            delete _ptrInStream;
            _openFile = false;
        }
        _ptrInStream = NULL;
    }

    codec_info_.pltype = 0;
    codec_info_.plname[0] = '\0';

    if(!_playingActive)
    {
        VOIP_TRACE(kTraceWarning, kTraceFile, _id,
                     "playing is not active!");
        return -1;
    }

    _playingActive = false;
    return 0;
}

bool MediaFileImpl::IsPlaying()
{
    VOIP_TRACE(kTraceStream, kTraceFile, _id, "MediaFileImpl::IsPlaying()");
    CriticalSectionScoped lock(_crit);
    return _playingActive;
}

int32_t MediaFileImpl::IncomingAudioData(
    const int8_t*  buffer,
    const uint32_t bufferLengthInBytes)
{
    return IncomingAudioVideoData( buffer, bufferLengthInBytes, false);
}

int32_t MediaFileImpl::IncomingAudioVideoData(
    const int8_t*  buffer,
    const uint32_t bufferLengthInBytes,
    const bool video)
{
    VOIP_TRACE(kTraceStream, kTraceFile, _id,
                 "MediaFile::IncomingData(buffer= 0x%x, bufLen= %hd",
                 buffer, bufferLengthInBytes);

    if(buffer == NULL || bufferLengthInBytes == 0)
    {
        VOIP_TRACE(kTraceError, kTraceFile, _id,
                     "Buffer pointer or length is NULL!");
        return -1;
    }

    bool recordingEnded = false;
    uint32_t callbackNotifyMs = 0;
    {
        CriticalSectionScoped lock(_crit);

        if(!_recordingActive)
        {
            VOIP_TRACE(kTraceWarning, kTraceFile, _id,
                         "Not currently recording!");
            return -1;
        }
        if(_ptrOutStream == NULL)
        {
            VOIP_TRACE(kTraceError, kTraceFile, _id,
                         "Recording is active, but output stream is NULL!");
            assert(false);
            return -1;
        }

        int32_t bytesWritten = 0;
        uint32_t samplesWritten = codec_info_.packsize;
        if(_ptrFileUtilityObj)
        {
            switch(_fileFormat)
            {
                case kFileFormatPcm8kHzFile:
                case kFileFormatPcm16kHzFile:
                case kFileFormatPcm32kHzFile:
                    bytesWritten = _ptrFileUtilityObj->WritePCMData(
                        *_ptrOutStream,
                        buffer,
                        bufferLengthInBytes);

                    // Sample size is 2 bytes.
                    if(bytesWritten > 0)
                    {
                        samplesWritten = bytesWritten/sizeof(int16_t);
                    }
                    break;
                case kFileFormatCompressedFile:
                    bytesWritten = _ptrFileUtilityObj->WriteCompressedData(
                        *_ptrOutStream, buffer, bufferLengthInBytes);
                    break;
                case kFileFormatWavFile:
                    bytesWritten = _ptrFileUtilityObj->WriteWavData(
                        *_ptrOutStream,
                        buffer,
                        bufferLengthInBytes);
                    if(bytesWritten > 0 && STR_NCASE_CMP(codec_info_.plname,
                                                         "L16", 4) == 0)
                    {
                        // Sample size is 2 bytes.
                        samplesWritten = bytesWritten/sizeof(int16_t);
                    }
                    break;
                case kFileFormatPreencodedFile:
                    bytesWritten = _ptrFileUtilityObj->WritePreEncodedData(
                        *_ptrOutStream, buffer, bufferLengthInBytes);
                    break;
                
            }
        } else {
            // TODO (hellner): quick look at the code makes me think that this
            //                 code is never executed. Remove?
            if(_ptrOutStream)
            {
                if(_ptrOutStream->Write(buffer, bufferLengthInBytes))
                {
                    bytesWritten = bufferLengthInBytes;
                }
            }
        }

        if(!video)
        {
            _recordDurationMs += samplesWritten / (codec_info_.plfreq / 1000);
        }

        // Check if it's time for RecordNotification(..).
        if(_notificationMs)
        {
            if(_recordDurationMs  >= _notificationMs)
            {
                _notificationMs = 0;
                callbackNotifyMs = _recordDurationMs;
            }
        }
        if(bytesWritten < (int32_t)bufferLengthInBytes)
        {
            VOIP_TRACE(kTraceWarning, kTraceFile, _id,
                         "Failed to write all requested bytes!");
            StopRecording();
            recordingEnded = true;
        }
    }

    // Only _callbackCrit may and should be taken when making callbacks.
    CriticalSectionScoped lock(_callbackCrit);
    if(_ptrCallback)
    {
        if(callbackNotifyMs)
        {
            _ptrCallback->RecordNotification(_id, callbackNotifyMs);
        }
        if(recordingEnded)
        {
            _ptrCallback->RecordFileEnded(_id);
            return -1;
        }
    }
    return 0;
}


int32_t MediaFileImpl::StopRecording()
{

    CriticalSectionScoped lock(_crit);
    if(!_recordingActive)
    {
        VOIP_TRACE(kTraceWarning, kTraceFile, _id,
                     "recording is not active!");
        return -1;
    }

    _isStereo = false;

    if(_ptrFileUtilityObj != NULL)
    {
        // Both AVI and WAV header has to be updated before closing the stream
        // because they contain size information.
        if((_fileFormat == kFileFormatWavFile) &&
            (_ptrOutStream != NULL))
        {
            _ptrFileUtilityObj->UpdateWavHeader(*_ptrOutStream);
        }
        delete _ptrFileUtilityObj;
        _ptrFileUtilityObj = NULL;
    }

    if(_ptrOutStream != NULL)
    {
        // If MediaFileImpl opened the OutStream it must be reclaimed here.
        if(_openFile)
        {
            delete _ptrOutStream;
            _openFile = false;
        }
        _ptrOutStream = NULL;
    }

    _recordingActive = false;
    codec_info_.pltype = 0;
    codec_info_.plname[0] = '\0';

    return 0;
}

bool MediaFileImpl::IsRecording()
{
    VOIP_TRACE(kTraceStream, kTraceFile, _id, "MediaFileImpl::IsRecording()");
    CriticalSectionScoped lock(_crit);
    return _recordingActive;
}

int32_t MediaFileImpl::RecordDurationMs(uint32_t& durationMs)
{

    CriticalSectionScoped lock(_crit);
    if(!_recordingActive)
    {
        durationMs = 0;
        return -1;
    }
    durationMs = _recordDurationMs;
    return 0;
}

bool MediaFileImpl::IsStereo()
{
    VOIP_TRACE(kTraceStream, kTraceFile, _id, "MediaFileImpl::IsStereo()");
    CriticalSectionScoped lock(_crit);
    return _isStereo;
}

int32_t MediaFileImpl::SetModuleFileCallback(FileCallback* callback)
{

    CriticalSectionScoped lock(_callbackCrit);

    _ptrCallback = callback;
    return 0;
}

int32_t MediaFileImpl::FileDurationMs(const char* fileName,
                                      uint32_t& durationMs,
                                      const FileFormats format,
                                      const uint32_t freqInHz)
{

    if(!ValidFileName(fileName))
    {
        return -1;
    }
    if(!ValidFrequency(freqInHz))
    {
        return -1;
    }

    ModuleFileUtility* utilityObj = new ModuleFileUtility(_id);
    if(utilityObj == NULL)
    {
        VOIP_TRACE(kTraceError, kTraceFile, _id,
                     "failed to allocate utility object!");
        return -1;
    }

    const int32_t duration = utilityObj->FileDurationMs(fileName, format,
                                                        freqInHz);
    delete utilityObj;
    if(duration == -1)
    {
        durationMs = 0;
        return -1;
    }

    durationMs = duration;
    return 0;
}

int32_t MediaFileImpl::PlayoutPositionMs(uint32_t& positionMs) const
{
    CriticalSectionScoped lock(_crit);
    if(!_playingActive)
    {
        positionMs = 0;
        return -1;
    }
    positionMs = _playoutPositionMs;
    return 0;
}

bool MediaFileImpl::ValidFileFormat(const FileFormats format,
                                    const CodecInst*  codecInst)
{
    if(codecInst == NULL)
    {
        if(format == kFileFormatPreencodedFile ||
           format == kFileFormatPcm8kHzFile    ||
           format == kFileFormatPcm16kHzFile   ||
           format == kFileFormatPcm32kHzFile)
        {
            VOIP_TRACE(kTraceError, kTraceFile, -1,
                         "Codec info required for file format specified!");
            return false;
        }
    }
    return true;
}

bool MediaFileImpl::ValidFileName(const char* fileName)
{
    if((fileName == NULL) ||(fileName[0] == '\0'))
    {
        VOIP_TRACE(kTraceError, kTraceFile, -1, "FileName not specified!");
        return false;
    }
    return true;
}


bool MediaFileImpl::ValidFilePositions(const uint32_t startPointMs,
                                       const uint32_t stopPointMs)
{
    if(startPointMs == 0 && stopPointMs == 0) // Default values
    {
        return true;
    }
    if(stopPointMs &&(startPointMs >= stopPointMs))
    {
        VOIP_TRACE(kTraceError, kTraceFile, -1,
                     "startPointMs must be less than stopPointMs!");
        return false;
    }
    if(stopPointMs &&((stopPointMs - startPointMs) < 20))
    {
        VOIP_TRACE(kTraceError, kTraceFile, -1,
                     "minimum play duration for files is 20 ms!");
        return false;
    }
    return true;
}

bool MediaFileImpl::ValidFrequency(const uint32_t frequency)
{
    if((frequency == 8000) || (frequency == 16000)|| (frequency == 32000))
    {
        return true;
    }
    VOIP_TRACE(kTraceError, kTraceFile, -1,
                 "Frequency should be 8000, 16000 or 32000 (Hz)");
    return false;
}
}  // namespace VoIP
