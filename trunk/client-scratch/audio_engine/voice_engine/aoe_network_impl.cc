
#include "audio_engine/voice_engine/aoe_network_impl.h"

#include "audio_engine/system_wrappers/interface/critical_section_wrapper.h"
#include "audio_engine/system_wrappers/interface/logging.h"
#include "audio_engine/system_wrappers/interface/trace.h"
#include "audio_engine/voice_engine/channel.h"
#include "audio_engine/voice_engine/include/aoe_errors.h"
#include "audio_engine/voice_engine/audio_engine_impl.h"

namespace VoIP
{

AoEIO* AoEIO::GetInterface(AudioEngine* audioEngine)
{
    if (NULL == audioEngine)
    {
        return NULL;
    }
    AudioEngineImpl* s = static_cast<AudioEngineImpl*>(audioEngine);
    s->AddRef();
    return s;
}

AoENetworkImpl::AoENetworkImpl(aoe::SharedData* shared) : _shared(shared)
{
    VOIP_TRACE(kTraceMemory, kTraceVoice, AoEId(_shared->instance_id(), -1),
                 "AoENetworkImpl() - ctor");
}

AoENetworkImpl::~AoENetworkImpl()
{
    VOIP_TRACE(kTraceMemory, kTraceVoice, AoEId(_shared->instance_id(), -1),
                 "~AoENetworkImpl() - dtor");
}

int AoENetworkImpl::RegisterExternalTransport(int channel,
                                              Transport& transport)
{
    VOIP_TRACE(kTraceApiCall, kTraceVoice, AoEId(_shared->instance_id(), -1),
                 "SetExternalTransport(channel=%d, transport=0x%x)",
                 channel, &transport);
    if (!_shared->statistics().Initialized())
    {
        _shared->SetLastError(VE_NOT_INITED, kTraceError);
        return -1;
    }
    aoe::ChannelOwner ch = _shared->channel_manager().GetChannel(channel);
    aoe::Channel* channelPtr = ch.channel();
    if (channelPtr == NULL)
    {
        _shared->SetLastError(VE_CHANNEL_NOT_VALID, kTraceError,
            "SetExternalTransport() failed to locate channel");
        return -1;
    }
    return channelPtr->RegisterExternalTransport(transport);
}

int AoENetworkImpl::DeRegisterExternalTransport(int channel)
{
    VOIP_TRACE(kTraceApiCall, kTraceVoice, AoEId(_shared->instance_id(), -1),
                 "DeRegisterExternalTransport(channel=%d)", channel);
    if (!_shared->statistics().Initialized())
    {
        VOIP_TRACE(kTraceError, kTraceVoice,
                     AoEId(_shared->instance_id(), -1),
                     "DeRegisterExternalTransport() - invalid state");
    }
    aoe::ChannelOwner ch = _shared->channel_manager().GetChannel(channel);
    aoe::Channel* channelPtr = ch.channel();
    if (channelPtr == NULL)
    {
        _shared->SetLastError(VE_CHANNEL_NOT_VALID, kTraceError,
            "DeRegisterExternalTransport() failed to locate channel");
        return -1;
    }
    return channelPtr->DeRegisterExternalTransport();
}

int AoENetworkImpl::ReceivedPlayoutData(int channel, AudioFrame &frame)
{
	VOIP_TRACE(kTraceStream, kTraceVoice, AoEId(_shared->instance_id(), -1),
		"ReceivedPlayoutData(channel=%d)", channel);


	aoe::ChannelOwner ch = _shared->channel_manager().GetChannel(channel);
	aoe::Channel* channelPtr = ch.channel();
	if (channelPtr == NULL)
	{
		_shared->SetLastError(VE_CHANNEL_NOT_VALID, kTraceError,
			"ReceivedPlayoutData() failed to locate channel");
		return -1;
	}

	return channelPtr->ReceivedPlayoutData(frame);
}

}  // namespace VoIP
