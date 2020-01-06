#ifndef VOIP_MODULES_AUDIO_DEVICE_MOCK_AUDIO_DEVICE_BUFFER_H_
#define VOIP_MODULES_AUDIO_DEVICE_MOCK_AUDIO_DEVICE_BUFFER_H_

#include "testing/gmock/include/gmock/gmock.h"
#include "audio_engine/modules/audio_device/audio_device_buffer.h"

namespace webrtc {

class MockAudioDeviceBuffer : public AudioDeviceBuffer {
 public:
  MockAudioDeviceBuffer() {}
  virtual ~MockAudioDeviceBuffer() {}

  MOCK_METHOD1(RequestPlayoutData, int32_t(uint32_t nSamples));
  MOCK_METHOD1(GetPlayoutData, int32_t(void* audioBuffer));
};

}  // namespace webrtc

#endif  // VOIP_MODULES_AUDIO_DEVICE_MOCK_AUDIO_DEVICE_BUFFER_H_
