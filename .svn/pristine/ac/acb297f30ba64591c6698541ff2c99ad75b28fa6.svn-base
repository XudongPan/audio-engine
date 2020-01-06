#ifndef VOIP_AUDIO_ENGINE_AUDIO_ENGINE_IMPL_H
#define VOIP_AUDIO_ENGINE_AUDIO_ENGINE_IMPL_H

#include "audio_engine/include/engine_configurations.h"
#include "audio_engine/system_wrappers/interface/atomic32.h"
#include "audio_engine/voice_engine/aoe_base_impl.h"

#ifdef VOIP_AUDIO_ENGINE_AUDIO_PROCESSING_API
#include "audio_engine/voice_engine/aoe_audio_processing_impl.h"
#endif
#ifdef VOIP_AUDIO_ENGINE_HARDWARE_API
#include "audio_engine/voice_engine/aoe_hardware_impl.h"
#endif
#include "audio_engine/voice_engine/aoe_network_impl.h"
#ifdef VOIP_AUDIO_ENGINE_VOLUME_CONTROL_API
#include "audio_engine/voice_engine/aoe_volume_control_impl.h"
#endif

namespace VoIP
{

class AudioEngineImpl : public aoe::SharedData,  // Must be the first base class
                        public AudioEngine,
#ifdef VOIP_AUDIO_ENGINE_AUDIO_PROCESSING_API
                        public AoEAudioProcessingImpl,
#endif
#ifdef VOIP_AUDIO_ENGINE_HARDWARE_API
                        public AoEHardwareImpl,
#endif
                        public AoENetworkImpl,
#ifdef VOIP_AUDIO_ENGINE_VOLUME_CONTROL_API
                        public AoEVolumeControlImpl,
#endif
                        public AoEBaseImpl
{
public:
    AudioEngineImpl(const Config* config, bool owns_config) :
        SharedData(*config),
#ifdef VOIP_AUDIO_ENGINE_AUDIO_PROCESSING_API
        AoEAudioProcessingImpl(this),
#endif
#ifdef VOIP_AUDIO_ENGINE_HARDWARE_API
		AoEHardwareImpl(this),
#endif
        AoENetworkImpl(this),
#ifdef VOIP_AUDIO_ENGINE_VOLUME_CONTROL_API
        AoEVolumeControlImpl(this),
#endif
        AoEBaseImpl(this),
        _ref_count(0),
        own_config_(owns_config ? config : NULL)
    {
    }
    virtual ~AudioEngineImpl()
    {
        assert(_ref_count.Value() == 0);
    }

    int AddRef();

    // This implements the Release() method for all the inherited interfaces.
    virtual int Release();

private:
    Atomic32 _ref_count;
    scoped_ptr<const Config> own_config_;
};

}  // namespace VoIP

#endif // VOIP_AUDIO_ENGINE_AUDIO_ENGINE_IMPL_H
