#pragma once

#include "audio_engine/modules/audio_device/include/audio_device.h"
#include "audio_engine/common_audio/resampler/include/resampler.h"
#include <list>
#include "audio_engine/system_wrappers/interface/file_wrapper.h"

using namespace VoIP;

struct AudioDataPacket
{
	uint8_t dataBuffer[4 * 960];
	uint16_t nSamples;
	uint16_t nBytesPerSample;
	uint8_t nChannels;
	uint32_t samplesPerSec;
};

class audio_device_test :public AudioTransport
{
public:
	audio_device_test(void);
	virtual ~audio_device_test(void);

	int create_device();
	int init();

	int set_playout();
	int init_playout();
	int init_record();
	int set_record();
	
	int start();
	int stop();

	int set_param();

	virtual int32_t RecordedDataIsAvailable(const void* audioSamples,
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
private:

	AudioDeviceModule * device;
	typedef std::list<AudioDataPacket*> AudioPacketList;
	AudioPacketList _audioList;
	Resampler _resampler;

	FileWrapper&		_playFile;
};

