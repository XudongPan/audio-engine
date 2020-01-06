#include "stdafx.h"
#include "device_test.h"


AudioTransportImplent::AudioTransportImplent(AudioDeviceModule* audioDevice) :
	_audioDevice(audioDevice),
	_playFromFile(false),
	_fullDuplex(false),
	_speakerVolume(false),
	_speakerMute(false),
	_microphoneVolume(false),
	_microphoneMute(false),
	_microphoneBoost(false),
	_microphoneAGC(false),
	_loopBackMeasurements(false),
	_playFile(*FileWrapper::Create()),
	_recCount(0),
	_playCount(0)
{
	_resampler.Reset(48000, 48000, kResamplerSynchronousStereo);
}

AudioTransportImplent::~AudioTransportImplent()
{
	_playFile.Flush();
	_playFile.CloseFile();
	delete &_playFile;

	for (AudioPacketList::iterator iter = _audioList.begin();
		iter != _audioList.end(); ++iter) {
			delete *iter;
	}
}

// ----------------------------------------------------------------------------
//	AudioTransportImpl::SetFilePlayout
// ----------------------------------------------------------------------------

int32_t AudioTransportImplent::SetFilePlayout(bool enable, const char* fileName)
{
	_playFromFile = enable;
	if (enable)
	{
		return (_playFile.OpenFile(fileName, true, true, false));
	} else
	{
		_playFile.Flush();
		return (_playFile.CloseFile());
	}
}
;

void AudioTransportImplent::SetFullDuplex(bool enable)
{
	_fullDuplex = enable;

	for (AudioPacketList::iterator iter = _audioList.begin();
		iter != _audioList.end(); ++iter) {
			delete *iter;
	}
	_audioList.clear();
}

int32_t AudioTransportImplent::RecordedDataIsAvailable(
	const void* audioSamples,
	const uint32_t nSamples,
	const uint8_t nBytesPerSample,
	const uint8_t nChannels,
	const uint32_t samplesPerSec,
	const uint32_t totalDelayMS,
	const int32_t clockDrift,
	const uint32_t currentMicLevel,
	const bool keyPressed,
	uint32_t& newMicLevel)
{
	if (_fullDuplex && _audioList.size() < 15)
	{
		AudioPacket* packet = new AudioPacket();
		memcpy(packet->dataBuffer, audioSamples, nSamples * nBytesPerSample);
		packet->nSamples = (uint16_t) nSamples;
		packet->nBytesPerSample = nBytesPerSample;
		packet->nChannels = nChannels;
		packet->samplesPerSec = samplesPerSec;
		_audioList.push_back(packet);
	}

	_recCount++;
	if (_recCount % 100 == 0)
	{
		bool addMarker(true);

		if (_loopBackMeasurements)
		{
			addMarker = false;
		}

		if (_microphoneVolume)
		{
			uint32_t maxVolume(0);
			uint32_t minVolume(0);
			uint32_t volume(0);
			uint16_t stepSize(0);
			EXPECT_EQ(0, _audioDevice->MaxMicrophoneVolume(&maxVolume));
			EXPECT_EQ(0, _audioDevice->MinMicrophoneVolume(&minVolume));
			EXPECT_EQ(0, _audioDevice->MicrophoneVolumeStepSize(&stepSize));
			EXPECT_EQ(0, _audioDevice->MicrophoneVolume(&volume));
			if (volume == 0)
			{
				TEST_LOG("[0]");
				addMarker = false;
			}
			int stepScale = (int) ((maxVolume - minVolume) / (stepSize * 10));
			volume += (stepScale * stepSize);
			if (volume > maxVolume)
			{
				TEST_LOG("[MAX]");
				volume = 0;
				addMarker = false;
			}
			EXPECT_EQ(0, _audioDevice->SetMicrophoneVolume(volume));
		}

		if (_microphoneAGC)
		{
			uint32_t maxVolume(0);
			uint32_t minVolume(0);
			uint16_t stepSize(0);
			EXPECT_EQ(0, _audioDevice->MaxMicrophoneVolume(&maxVolume));
			EXPECT_EQ(0, _audioDevice->MinMicrophoneVolume(&minVolume));
			EXPECT_EQ(0, _audioDevice->MicrophoneVolumeStepSize(&stepSize));
			// emulate real AGC (min->max->min->max etc.)
			if (currentMicLevel <= 1)
			{
				TEST_LOG("[MIN]");
				addMarker = false;
			}
			int stepScale = (int) ((maxVolume - minVolume) / (stepSize * 10));
			newMicLevel = currentMicLevel + (stepScale * stepSize);
			if (newMicLevel > maxVolume)
			{
				TEST_LOG("[MAX]");
				newMicLevel = 1; // set lowest (non-zero) AGC level
				addMarker = false;
			}
		}

		if (_microphoneMute && (_recCount % 500 == 0))
		{
			bool muted(false);
			EXPECT_EQ(0, _audioDevice->MicrophoneMute(&muted));
			muted = !muted;
			EXPECT_EQ(0, _audioDevice->SetMicrophoneMute(muted));
			if (muted)
			{
				TEST_LOG("[MUTE ON]");
				addMarker = false;
			} else
			{
				TEST_LOG("[MUTE OFF]");
				addMarker = false;
			}
		}

		if (_microphoneBoost && (_recCount % 500 == 0))
		{
			bool boosted(false);
			EXPECT_EQ(0, _audioDevice->MicrophoneBoost(&boosted));
			boosted = !boosted;
			EXPECT_EQ(0, _audioDevice->SetMicrophoneBoost(boosted));
			if (boosted)
			{
				TEST_LOG("[BOOST ON]");
				addMarker = false;
			} else
			{
				TEST_LOG("[BOOST OFF]");
				addMarker = false;
			}
		}

		if ((nChannels == 1) && addMarker)
		{
			// mono
			TEST_LOG("-");
		} else if ((nChannels == 2) && (nBytesPerSample == 2) && addMarker)
		{
			AudioDeviceModule::ChannelType
				chType(AudioDeviceModule::kChannelLeft);
			EXPECT_EQ(0, _audioDevice->RecordingChannel(&chType));
			if (chType == AudioDeviceModule::kChannelLeft)
				TEST_LOG("-|");
			else
				TEST_LOG("|-");
		} else if (addMarker)
		{
			// stereo
			TEST_LOG("--");
		}

		if (nChannels == 2 && nBytesPerSample == 2)
		{
			TEST_LOG("=> emulated mono (one channel exctracted from stereo input)\n");
		}
	}

	return 0;
}


int32_t AudioTransportImplent::NeedMorePlayData(
	const uint32_t nSamples,
	const uint8_t nBytesPerSample,
	const uint8_t nChannels,
	const uint32_t samplesPerSec,
	void* audioSamples,
	uint32_t& nSamplesOut)
{
	if (_fullDuplex)
	{
		if (_audioList.empty())
		{
			// use zero stuffing when not enough data
			memset(audioSamples, 0, nBytesPerSample * nSamples);
		} else
		{
			AudioPacket* packet = _audioList.front();
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
				const uint16_t nBytesPerSampleIn =
					packet->nBytesPerSample;

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
					} else
					{
						if (_playCount % 100 == 0)
							TEST_LOG(
							"ERROR: unable to resample from %d to %d\n",
							samplesPerSecIn, samplesPerSec);
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
					} else
					{
						if (_playCount % 100 == 0)
							TEST_LOG("ERROR: unable to resample from %d to %d\n",
							samplesPerSecIn, samplesPerSec);
					}
				}
				nSamplesOut = nSamples;
				delete packet;
			}
		}
	}  // if (_fullDuplex)

	if (_playFromFile && _playFile.Open())
	{
		int16_t fileBuf[480];

		// read mono-file
		int32_t len = _playFile.Read((int8_t*) fileBuf, 2
			* nSamples);
		if (len != 2 * (int32_t) nSamples)
		{
			_playFile.Rewind();
			_playFile.Read((int8_t*) fileBuf, 2 * nSamples);
		}

		// convert to stero if required
		if (nChannels == 1)
		{
			memcpy(audioSamples, fileBuf, 2 * nSamples);
		} else
		{
			// mono sample from file is duplicated and sent to left and right
			// channels
			int16_t* audio16 = (int16_t*) audioSamples;
			for (unsigned int i = 0; i < nSamples; i++)
			{
				(*audio16) = fileBuf[i]; // left
				audio16++;
				(*audio16) = fileBuf[i]; // right
				audio16++;
			}
		}
	}  // if (_playFromFile && _playFile.Open())

	_playCount++;

	if (_playCount % 100 == 0)
	{
		bool addMarker(true);

		if (_speakerVolume)
		{
			uint32_t maxVolume(0);
			uint32_t minVolume(0);
			uint32_t volume(0);
			uint16_t stepSize(0);
			EXPECT_EQ(0, _audioDevice->MaxSpeakerVolume(&maxVolume));
			EXPECT_EQ(0, _audioDevice->MinSpeakerVolume(&minVolume));
			EXPECT_EQ(0, _audioDevice->SpeakerVolumeStepSize(&stepSize));
			EXPECT_EQ(0, _audioDevice->SpeakerVolume(&volume));
			if (volume == 0)
			{
				TEST_LOG("[0]");
				addMarker = false;
			}
			uint32_t step = (maxVolume - minVolume) / 10;
			step = (step < stepSize ? stepSize : step);
			volume += step;
			if (volume > maxVolume)
			{
				TEST_LOG("[MAX]");
				volume = 0;
				addMarker = false;
			}
			EXPECT_EQ(0, _audioDevice->SetSpeakerVolume(volume));
		}

		if (_speakerMute && (_playCount % 500 == 0))
		{
			bool muted(false);
			EXPECT_EQ(0, _audioDevice->SpeakerMute(&muted));
			muted = !muted;
			EXPECT_EQ(0, _audioDevice->SetSpeakerMute(muted));
			if (muted)
			{
				TEST_LOG("[MUTE ON]");
				addMarker = false;
			} else
			{
				TEST_LOG("[MUTE OFF]");
				addMarker = false;
			}
		}

		if (_loopBackMeasurements)
		{
			uint16_t recDelayMS(0);
			uint16_t playDelayMS(0);
			size_t nItemsInList(0);

			nItemsInList = _audioList.size();
			EXPECT_EQ(0, _audioDevice->RecordingDelay(&recDelayMS));
			EXPECT_EQ(0, _audioDevice->PlayoutDelay(&playDelayMS));
			TEST_LOG("Delay (rec+play)+buf: %3zu (%3u+%3u)+%3zu [ms]\n",
				recDelayMS + playDelayMS + 10 * (nItemsInList + 1),
				recDelayMS, playDelayMS, 10 * (nItemsInList + 1));

			addMarker = false;
		}

		if ((nChannels == 1) && addMarker)
		{
			TEST_LOG("+");
		} else if ((nChannels == 2) && addMarker)
		{
			TEST_LOG("++");
		}
	}  // if (_playCount % 100 == 0)

	nSamplesOut = nSamples;

	return 0;
}

