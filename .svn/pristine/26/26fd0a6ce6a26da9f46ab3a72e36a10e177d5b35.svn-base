#pragma once

#include "..//audio_engine//include//typedefs.h"
#include "../audio_engine/modules/audio_device/include/audio_device.h"
#include "../audio_engine/modules/audio_device/include/audio_device_defines.h"
#include "../audio_engine/modules/audio_device/audio_device_utility.h"
#include "../Audio_engine/common_audio/resampler/include/resampler.h"
#include "../audio_engine/modules/utility/interface/process_thread.h"
#include "../audio_engine/system_wrappers/interface/file_wrapper.h"
#include <list>
// ----------------------------------------------------------------------------
//  AudioTransport
// ----------------------------------------------------------------------------
using namespace VoIP;
using namespace std;


#define TEST_LOG printf
#define TEST_LOG_ERROR(...) fprintf(stderr, __VA_ARGS__)

#define RESET_TEST
#define PRINT_TEST_RESULTS

struct AudioPacket
{
	uint8_t dataBuffer[4 * 960];
	uint16_t nSamples;
	uint16_t nBytesPerSample;
	uint8_t nChannels;
	uint32_t samplesPerSec;
};


class AudioTransportImplent: public AudioTransport
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

	AudioTransportImplent(AudioDeviceModule* audioDevice);
	~AudioTransportImplent();

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

