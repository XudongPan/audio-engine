#include "stdafx.h"


#include "audio_device_test.h"

#include "audio_engine/modules/audio_device/audio_device_impl.h"

using namespace VoIP;

FILE * pFile = NULL;
void OpenWave(const char* lpFileName);
void CloseWave();
audio_device_test::audio_device_test(void)
	:_playFile(*FileWrapper::Create())
{
	_resampler.Reset(48000, 48000, kResamplerSynchronousStereo);
	
	device = NULL;
}


audio_device_test::~audio_device_test(void)
{
	device->Terminate();
	
	_playFile.Flush();
	_playFile.CloseFile();
	delete &_playFile;

	if (device)
	{
		device->Release(); 
		device = NULL;
	}
}

int audio_device_test::create_device()
{
	device = AudioDeviceModuleImpl::Create(1, VoIP::AudioDeviceModule::AudioLayer::kWindowsWaveAudio);

	device->RegisterAudioCallback(this);
	return device? 0: -1;
}

int audio_device_test::init()
{
	int  ret = 0; 
	ret = device->Init();
	ret &= device->InitRecording();
	ret &= device->InitPlayout();
	ret &= device->InitSpeaker();
	ret &= device->InitMicrophone();

	return ret;
}

int audio_device_test::set_playout()
{
	int ret = 0;
	int index = 0;
	bool isStereoAvailable = false;
	unsigned int  num_of_devices = 0;


	//get playout device
	num_of_devices = device->PlayoutDevices();

	printf("\r\n we have %d playout devices r\n", num_of_devices);

	for (index = 0 ; index < num_of_devices; index ++)
	{
		char name[128] = {0};
		char guid[128] = {0};
		device->PlayoutDeviceName(index, name, guid);
		printf("\r\n=====================================\r\n");
		printf(" %d : device name  = %s\r\n", index, name);
		printf("      guid  = %s\r\n", guid);
		printf("\r\n=====================================\r\n");
	}
	//set playout device
	if (device->Playing())
	{
		device->StopPlayout();
	}

	ret = device->SetPlayoutDevice(index-1);
	
	ret &= device->InitSpeaker();
	
	device->StereoPlayoutIsAvailable(&isStereoAvailable);
	ret &= device->SetStereoPlayout(isStereoAvailable);
	
	return ret;
	

}

int audio_device_test::init_playout()
{
	bool available = false;

// 	device->PlayoutIsAvailable(&available);
// 	if (!available)
// 	{
// 		return -1;
// 	}

	return device->InitPlayout();
}

int audio_device_test::init_record()
{
	bool available = false;

// 	device->RecordingIsAvailable(&available);
// 	if (!available)
// 	{
// 		return -1;
// 	}

	return device->InitRecording();
}

int audio_device_test::set_record()
{
	int ret = 0;
	int index = 0;
	bool isStereoAvailable = false;
	unsigned int  num_of_devices = 0;


	//get record device
	num_of_devices = device->RecordingDevices();

	printf("\r\n we have %d recording devices r\n", num_of_devices);

	for (index = 0 ; index < num_of_devices; index ++)
	{
		char name[128] = {0};
		char guid[128] = {0};
		device->RecordingDeviceName(index, name, guid);
		printf("\r\n=====================================\r\n");
		printf(" %d : device name  = %s\r\n", index, name);
		printf("      guid  = %s\r\n", guid);
		printf("\r\n=====================================\r\n");
	}
	//set record device
	if (device->Recording())
	{
		device->StopRecording();
	}

	ret = device->SetRecordingDevice(index-1);

	ret &= device->InitMicrophone();

	device->StereoRecordingIsAvailable(&isStereoAvailable);
	ret &= device->SetStereoRecording(isStereoAvailable);


	return ret;
}

int audio_device_test::start()
{
	int ret = 0;

	ret = device->SetRecordingSampleRate(8000);
	device->SetPlayoutSampleRate(8000);
	ret = device->StartRecording();
	ret &= device->StartPlayout();

/*	OpenWave("c:\\work\\123.wav");*/
	_playFile.Flush();
	_playFile.CloseFile();

	return (_playFile.OpenFile("c:\\work\\123.wav", false, false, false));

	device->StartRawInputFileRecording("c:\\work\\rawinput.pcm");
	device->StartRawOutputFileRecording("c:\\work\\rawoutput.pcm");
	return ret;
}

int audio_device_test::stop()
{
	int ret = 0;

	device->StopRawInputFileRecording();
	device->StopRawOutputFileRecording();

	//CloseWave();
	_playFile.Flush();
	_playFile.CloseFile();

	ret = device->StopRecording();
	ret &= device->StopPlayout();

	return ret;
}

int audio_device_test::set_param()
{
	//buffer settings
	//sample rates settings
	//volume settings
	return 0;

}

int32_t audio_device_test::RecordedDataIsAvailable( const void* audioSamples, const uint32_t nSamples, const uint8_t nBytesPerSample, const uint8_t nChannels, const uint32_t samplesPerSec, const uint32_t totalDelayMS, const int32_t clockDrift, const uint32_t currentMicLevel, const bool keyPressed, uint32_t& newMicLevel )
{
	AudioDataPacket* packet = new AudioDataPacket();
	memcpy(packet->dataBuffer, audioSamples, nSamples * nBytesPerSample);
	packet->nSamples = (uint16_t) nSamples;
	packet->nBytesPerSample = nBytesPerSample;
	packet->nChannels = nChannels;
	packet->samplesPerSec = samplesPerSec;
	_audioList.push_back(packet);

	
	return 0;
}

int32_t audio_device_test::NeedMorePlayData( const uint32_t nSamples, const uint8_t nBytesPerSample, const uint8_t nChannels, 
											const uint32_t samplesPerSec, void* audioSamples, uint32_t& nSamplesOut )
{

	printf(" nSamples = %d , nBytesPerSample = %d , nChannels = %d, samplesPerSec = %d \r\n",
				nSamples, nBytesPerSample, nChannels, samplesPerSec);


	if (_audioList.empty())
	{
		// use zero stuffing when not enough data
		memset(audioSamples, 0, nBytesPerSample * nSamples);
	} else
	{
		AudioDataPacket* packet = _audioList.front();
		_audioList.pop_front();
		if (packet)
		{
			int ret(0);
			int lenOut(0);
			int16_t tmpBuf_96kHz[80 * 12];
			int16_t* ptr16In = NULL;
			int16_t* ptr16Out = NULL;

			const uint16_t nSamplesIn = packet->nSamples;
			const uint8_t nChannelsIn = packet->nChannels;
			const uint32_t samplesPerSecIn = packet->samplesPerSec;
			const uint16_t nBytesPerSampleIn = packet->nBytesPerSample;

			int32_t fsInHz(samplesPerSecIn);
			int32_t fsOutHz(samplesPerSec);

			if (fsInHz == 44100)
				fsInHz = 44000;

			if (fsOutHz == 44100)
				fsOutHz = 44000;

			if (nChannelsIn == 2 && nBytesPerSampleIn == 4)
			{
				// input is stereo => we will resample in stereo
				ret = _resampler.ResetIfNeeded(fsInHz, fsOutHz,
					kResamplerSynchronousStereo);
				if (ret == 0)
				{
					if (nChannels == 2)
					{
						_resampler.Push(
							(const int16_t*) packet->dataBuffer,
							2 * nSamplesIn,
							(int16_t*) audioSamples, 2
							* nSamples, lenOut);
					} else
					{
						_resampler.Push(
							(const int16_t*) packet->dataBuffer,
							2 * nSamplesIn, tmpBuf_96kHz, 2
							* nSamples, lenOut);

						ptr16In = &tmpBuf_96kHz[0];
						ptr16Out = (int16_t*) audioSamples;

						// do stereo -> mono
						for (unsigned int i = 0; i < nSamples; i++)
						{
							*ptr16Out = *ptr16In; // use left channel
							ptr16Out++;
							ptr16In++;
							ptr16In++;
						}
					}
					assert(2*nSamples == (uint32_t)lenOut);
				} 
			} else
			{
				// input is mono (can be "reduced from stereo" as well) =>
				// we will resample in mono
				ret = _resampler.ResetIfNeeded(fsInHz, fsOutHz,
					kResamplerSynchronous);
				if (ret == 0)
				{
					if (nChannels == 1)
					{
						_resampler.Push(
							(const int16_t*) packet->dataBuffer,
							nSamplesIn,
							(int16_t*) audioSamples,
							nSamples, lenOut);
					} else
					{
						_resampler.Push(
							(const int16_t*) packet->dataBuffer,
							nSamplesIn, tmpBuf_96kHz, nSamples,
							lenOut);

						ptr16In = &tmpBuf_96kHz[0];
						ptr16Out = (int16_t*) audioSamples;

						// do mono -> stereo
						for (unsigned int i = 0; i < nSamples; i++)
						{
							*ptr16Out = *ptr16In; // left
							ptr16Out++;
							*ptr16Out = *ptr16In; // right (same as left sample)
							ptr16Out++;
							ptr16In++;
						}
					}
					assert(nSamples == (uint32_t)lenOut);
				} 
			}
			nSamplesOut = nSamples;
			delete packet;
		}
	}
// 	fwrite(audioSamples, 1 , nSamples * nBytesPerSample, pFile);
// 	fflush(pFile);
	if (_playFile.Open())
	{
		// write to binary file in mono or stereo (interleaved)
		_playFile.Write(audioSamples, nSamples * nBytesPerSample);
	}

	
	printf(" nSamplesOut = %d \r\n", nSamplesOut);
	return 0 ;
}

void OpenWave (const char* lpFileName)
{
	typedef struct _wav_header_
	{
		unsigned char RIFF[4];
		unsigned int dwFilesize;
		unsigned char WAVE[4];
		unsigned char fmt[4];
		unsigned int dwChunksize;
		unsigned short wCompress;
		unsigned short wChannelnum;
		unsigned int dwSamplerate;
		unsigned int dwBytepersec;
		unsigned short wBlockAlign;
		unsigned short wBitsPerSample;
		unsigned char data[4];
		unsigned int dwDataSize;
	} wav_header;

	fopen_s(&pFile, lpFileName, "w+");
	wav_header wavhead = { 0 };
	memcpy(wavhead.RIFF, "RIFF", 4);
	wavhead.dwFilesize = 160000 + sizeof(wav_header) - 8;
	memcpy(wavhead.WAVE, "WAVE", 4);
	char fmt[4] = { 'f', 'm', 't', 0x20 };
	memcpy(wavhead.fmt, fmt, 4);
	wavhead.dwChunksize = 0x10;
	wavhead.wCompress = 1;
	wavhead.wChannelnum = 1;
	wavhead.dwSamplerate = 8000;
	wavhead.dwBytepersec = 16000;
	wavhead.wBlockAlign = 2;
	wavhead.wBitsPerSample = 16;
	memcpy(wavhead.data, "data", 4);
	wavhead.dwDataSize = wavhead.dwFilesize - 42;
	fwrite(&wavhead, sizeof(wav_header), 1, pFile);
}

void CloseWave ()
{
	fclose(pFile);
}

