// This sub-API supports the following functionalities:
//
//  - Support of non-default codecs (e.g. iLBC, iSAC, etc.).
//  - Voice Activity Detection (VAD) on a per channel basis.
//  - Possibility to specify how to map received payload types to codecs.
//
// Usage example, omitting error checking:
//
//  using namespace VoIP;
//  AudioEngine* aoe = AudioEngine::Create();
//  VoEBase* base = VoEBase::GetInterface(aoe);
//  AoECodec* codec = AoECodec::GetInterface(aoe);
//  base->Init();
//  int num_of_codecs = codec->NumOfCodecs()
//  ...
//  base->Terminate();
//  base->Release();
//  codec->Release();
//  AudioEngine::Delete(aoe);
//
#ifndef VOIP_AUDIO_ENGINE_CODEC_H
#define VOIP_AUDIO_ENGINE_CODEC_H

#include "audio_engine/include/common_types.h"

namespace VoIP {

class AudioEngine;

class VOIP_DLLEXPORT AoECodec
{
public:
    // Factory for the AoECodec sub-API. Increases an internal
    // reference counter if successful. Returns NULL if the API is not
    // supported or if construction fails.
    static AoECodec* GetInterface(AudioEngine* voiceEngine);

    // Releases the AoECodec sub-API and decreases an internal
    // reference counter. Returns the new reference count. This value should
    // be zero for all sub-API:s before the AudioEngine object can be safely
    // deleted.
    virtual int Release() = 0;

    // Gets the number of supported codecs.
    virtual int NumOfCodecs() = 0;

    // Get the |codec| information for a specified list |index|.
    virtual int GetCodec(int index, CodecInst& codec) = 0;

    // Sets the |codec| for the |channel| to be used for sending.
    virtual int SetSendCodec(int channel, const CodecInst& codec) = 0;

    // Gets the |codec| parameters for the sending codec on a specified
    // |channel|.
    virtual int GetSendCodec(int channel, CodecInst& codec) = 0;

    // Sets the |codec| as secondary codec for |channel|. Registering a
    // secondary send codec enables dual-streaming. In dual-streaming mode,
    // payloads of the primary and the secondary codecs are packed in RED
    // payloads with |red_payload_type| as payload type. The Secondary codec
    // MUST have the same sampling rate as the primary codec, otherwise the
    // codec cannot be registered and -1 is returned. This method fails if a
    // primary codec is not yet set.
    virtual int SetSecondarySendCodec(int channel, const CodecInst& codec,
                                      int red_payload_type) = 0;

    // Removes the secondary codec from |channel|. This will terminate
    // dual-streaming.
    virtual int RemoveSecondarySendCodec(int channel) = 0;

    // Gets |codec| which is used as secondary codec in |channel|.
    virtual int GetSecondarySendCodec(int channel, CodecInst& codec) = 0;

    // Gets the currently received |codec| for a specific |channel|.
    virtual int GetRecCodec(int channel, CodecInst& codec) = 0;

    // Sets the dynamic payload type number for a particular |codec| or
    // disables (ignores) a codec for receiving. For instance, when receiving
    // an invite from a SIP-based client, this function can be used to change
    // the dynamic payload type number to match that in the INVITE SDP-
    // message. The utilized parameters in the |codec| structure are:
    // plname, plfreq, pltype and channels.
    virtual int SetRecPayloadType(int channel, const CodecInst& codec) = 0;

    // Gets the actual payload type that is set for receiving a |codec| on a
    // |channel|. The value it retrieves will either be the default payload
    // type, or a value earlier set with SetRecPayloadType().
    virtual int GetRecPayloadType(int channel, CodecInst& codec) = 0;

    // Sets the payload |type| for the sending of SID-frames with background
    // noise estimation during silence periods detected by the VAD.
    virtual int SetSendCNPayloadType(
        int channel, int type, PayloadFrequencies frequency = kFreq16000Hz) = 0;


    // Sets the VAD/DTX (silence suppression) status and |mode| for a
    // specified |channel|. Disabling VAD (through |enable|) will also disable
    // DTX; it is not necessary to explictly set |disableDTX| in this case.
    virtual int SetVADStatus(int channel, bool enable,
                             VadModes mode = kVadConventional,
                             bool disableDTX = false) = 0;

    // Gets the VAD/DTX status and |mode| for a specified |channel|.
    virtual int GetVADStatus(int channel, bool& enabled, VadModes& mode,
                             bool& disabledDTX) = 0;
protected:
    AoECodec() {}
    virtual ~AoECodec() {}
};

}  // namespace VoIP

#endif  //  VOIP_AUDIO_ENGINE_CODEC_H
