#ifndef VOIP_AUDIO_ENGINE_AOE_NETWORK_IMPL_H
#define VOIP_AUDIO_ENGINE_AOE_NETWORK_IMPL_H

#include "audio_engine/voice_engine/include/aoe_network.h"

#include "audio_engine/voice_engine/shared_data.h"


namespace VoIP
{

class AoENetworkImpl: public AoEIO
{
public:
    virtual int RegisterExternalTransport(int channel, Transport& transport);

    virtual int DeRegisterExternalTransport(int channel);

	virtual int ReceivedPlayoutData(int channel, AudioFrame &frame);
protected:
    AoENetworkImpl(aoe::SharedData* shared);
    virtual ~AoENetworkImpl();
private:
    aoe::SharedData* _shared;
};

}  // namespace VoIP

#endif  // VOIP_AUDIO_ENGINE_AOE_NETWORK_IMPL_H
