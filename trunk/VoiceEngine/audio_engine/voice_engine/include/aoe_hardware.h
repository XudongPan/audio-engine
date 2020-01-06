// This sub-API supports the following functionalities:
//
//  - Audio device handling.
//  - Device information.
//  - CPU load monitoring.
//
// Usage example, omitting error checking:
//
//  using namespace voip;
//  AudioEngine* voe = AudioEngine::Create();
//  AoEBase* base = AoEBase::GetInterface(voe);
//  AoEHardware* hardware  = AoEHardware::GetInterface(voe);
//  base->Init();
//  ...
//  int n_devices = hardware->GetNumOfPlayoutDevices();
//  ...
//  base->Terminate();
//  base->Release();
//  hardware->Release();
//  AudioEngine::Delete(voe);
//
#ifndef VOIP_AUDIO_ENGINE_AOE_HARDWARE_H
#define VOIP_AUDIO_ENGINE_AOE_HARDWARE_H

#include "audio_engine/include/aoe_hardware.h"

#endif  //  VOIP_AUDIO_ENGINE_AOE_HARDWARE_H
