// This sub-API supports the following functionalities:
//
//  - Speaker volume controls.
//  - Microphone volume control.
//  - Non-linear speech level control.
//  - Mute functions.
//  - Additional stereo scaling methods.
//
// Usage example, omitting error checking:
//
//  using namespace voip;
//  AudioEngine* voe = AudioEngine::Create();
//  AoEBase* base = AoEBase::GetInterface(voe);
//  AoEVolumeControl* volume  = AoEVolumeControl::GetInterface(voe);
//  base->Init();
//  int ch = base->CreateChannel();
//  ...
//  volume->SetInputMute(ch, true);
//  ...
//  base->DeleteChannel(ch);
//  base->Terminate();
//  base->Release();
//  volume->Release();
//  AudioEngine::Delete(voe);
//
#ifndef VOIP_AUDIO_ENGINE_AOE_VOLUME_CONTROL_H
#define VOIP_AUDIO_ENGINE_AOE_VOLUME_CONTROL_H

#include "audio_engine/include/aoe_volume_control.h"

#endif  // #ifndef VOIP_AUDIO_ENGINE_AOE_VOLUME_CONTROL_H
