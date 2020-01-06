#include "stdafx.h"
#include "audio_proxy.h"


audio_proxy::audio_proxy(void)
{
}


audio_proxy::~audio_proxy(void)
{
}

int32_t audio_proxy::RecordedDataIsAvailable( const void* audioSamples, 
											 const uint32_t nSamples, 
											 const uint8_t nBytesPerSample, 
											 const uint8_t nChannels, 
											 const uint32_t samplesPerSec, 
											 const uint32_t totalDelayMS, 
											 const int32_t clockDrift, 
											 const uint32_t currentMicLevel, 
											 const bool keyPressed, 
											 uint32_t& newMicLevel )
{
// 	uint32_t samplesout = 0;
// 	this->NeedMorePlayData(nSamples, nBytesPerSample, nChannels, samplesPerSec, audioSamples, samplesout);

}

int32_t audio_proxy::NeedMorePlayData( const uint32_t nSamples, 
									  const uint8_t nBytesPerSample, 
									  const uint8_t nChannels, 
									  const uint32_t samplesPerSec, 
									  void* audioSamples, 
									  uint32_t& nSamplesOut )
{
// 	GetPlayoutData(static_cast<int>(samplesPerSec),
// 		static_cast<int>(nChannels),
// 		static_cast<int>(nSamples), true, audioSamples);
// 
// 	nSamplesOut = samples_per_channel_;


}
