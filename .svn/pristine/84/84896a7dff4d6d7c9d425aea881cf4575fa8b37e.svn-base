#pragma once

#include "..//audio_engine//include//typedefs.h"
#include "../audio_engine/modules/audio_device/include/audio_device.h"
#include "../audio_engine/modules/audio_device/include/audio_device_defines.h"
#include "../audio_engine/modules/audio_device/audio_device_utility.h"
#include "../Audio_engine/common_audio/resampler/include/resampler.h"
#include "../audio_engine/modules/utility/interface/process_thread.h"
#include "../audio_engine/system_wrappers/interface/file_wrapper.h"
#include "fileutils.h"
#include <list>
#include "../audio_engine/system_wrappers/interface/sleep.h"

using namespace VoIP;
using namespace std;

class AudioTransportImpl;

#define TEST_LOG printf
#define TEST_LOG_ERROR(...) fprintf(stderr, __VA_ARGS__)

#define RESET_TEST
#define PRINT_TEST_RESULTS

#pragma warning(disable:4996)


#if defined(USE_SLEEP_AS_PAUSE)
#define PAUSE(a) SleepMs(a);
#else
#define PAUSE(a) AudioDeviceUtility::WaitForKey();
#endif


enum TestType
{
	TTInvalid = -1,
	TTAll = 0,
	TTAudioLayerSelection = 1,
	TTDeviceEnumeration = 2,
	TTDeviceSelection = 3,
	TTAudioTransport = 4,
	TTSpeakerVolume = 5,
	TTMicrophoneVolume = 6,
	TTSpeakerMute = 7,
	TTMicrophoneMute = 8,
	TTMicrophoneBoost = 9,
	TTMicrophoneAGC = 10,
	TTLoopback = 11,
	TTDeviceRemoval = 13,
	TTMobileAPI = 14,
	TTTest = 66,
};


struct AudioPacket
{
	uint8_t dataBuffer[4 * 960];
	uint16_t nSamples;
	uint16_t nBytesPerSample;
	uint8_t nChannels;
	uint32_t samplesPerSec;
};


class AudioEventObserver: public AudioDeviceObserver
{
public:
	virtual void OnErrorIsReported(const ErrorCode error);
	virtual void OnWarningIsReported(const WarningCode warning);
	AudioEventObserver(AudioDeviceModule* audioDevice);
	~AudioEventObserver();
public:
	ErrorCode _error;
	WarningCode _warning;
};


class Audio_Device_FuncTest
{
public:
	Audio_Device_FuncTest(void);
	~Audio_Device_FuncTest(void);

	int32_t Init();
	int32_t Close();
	int32_t DoTest(const TestType testType);

private:
	int32_t TestAudioLayerSelection();
	int32_t TestDeviceEnumeration();
	int32_t TestDeviceSelection();
	int32_t TestAudioTransport();
	int32_t TestSpeakerVolume();
	int32_t TestMicrophoneVolume();
	int32_t TestSpeakerMute();
	int32_t TestMicrophoneMute();
	int32_t TestMicrophoneBoost();
	int32_t TestLoopback();
	int32_t TestDeviceRemoval();
	int32_t TestExtra();
	int32_t TestMicrophoneAGC();
	int32_t SelectPlayoutDevice();
	int32_t SelectRecordingDevice();
	int32_t TestAdvancedMBAPI();

private:

	// Paths to where the resource files to be used for this test are located.
	std::string _playoutFile48;
	std::string _playoutFile44;
	std::string _playoutFile16;
	std::string _playoutFile8;


	AudioDeviceModule * _audioDevice;
	AudioTransportImpl * _audioTransport;

	ProcessThread* _processThread;
	AudioEventObserver* _audioEventObserver;
};


// ----------------------------------------------------------------------------
//  AudioTransport
// ----------------------------------------------------------------------------

class AudioTransportImpl: public AudioTransport
{
public:
	virtual int32_t
		RecordedDataIsAvailable(const void* audioSamples,
		const uint32_t nSamples,
		const uint8_t nBytesPerSample,
		const uint8_t nChannels,
		const uint32_t samplesPerSec,
		const uint32_t totalDelayMS,
		const int32_t clockDrift,
		const uint32_t currentMicLevel,
		const bool keyPressed,
		uint32_t& newMicLevel);

	virtual int32_t NeedMorePlayData(const uint32_t nSamples,
		const uint8_t nBytesPerSample,
		const uint8_t nChannels,
		const uint32_t samplesPerSec,
		void* audioSamples,
		uint32_t& nSamplesOut);

	AudioTransportImpl(AudioDeviceModule* audioDevice);
	~AudioTransportImpl();

public:
	int32_t SetFilePlayout(bool enable, const char* fileName = NULL);
	void SetFullDuplex(bool enable);
	void SetSpeakerVolume(bool enable)
	{
		_speakerVolume = enable;
	}
	;
	void SetSpeakerMute(bool enable)
	{
		_speakerMute = enable;
	}
	;
	void SetMicrophoneMute(bool enable)
	{
		_microphoneMute = enable;
	}
	;
	void SetMicrophoneVolume(bool enable)
	{
		_microphoneVolume = enable;
	}
	;
	void SetMicrophoneBoost(bool enable)
	{
		_microphoneBoost = enable;
	}
	;
	void SetLoopbackMeasurements(bool enable)
	{
		_loopBackMeasurements = enable;
	}
	;
	void SetMicrophoneAGC(bool enable)
	{
		_microphoneAGC = enable;
	}
	;

private:
	typedef std::list<AudioPacket*> AudioPacketList;
	AudioDeviceModule* _audioDevice;

	bool _playFromFile;
	bool _fullDuplex;
	bool _speakerVolume;
	bool _speakerMute;
	bool _microphoneVolume;
	bool _microphoneMute;
	bool _microphoneBoost;
	bool _microphoneAGC;
	bool _loopBackMeasurements;

	FileWrapper& _playFile;
	uint32_t _recCount;
	uint32_t _playCount;
	AudioPacketList _audioList;

	Resampler _resampler;
};

