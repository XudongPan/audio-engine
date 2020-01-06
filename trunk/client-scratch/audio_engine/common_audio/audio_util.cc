
#include "audio_engine/common_audio/include/audio_util.h"

#include "audio_engine/include/typedefs.h"

namespace VoIP {

void RoundToInt16(const float* src, int size, int16_t* dest) {
  for (int i = 0; i < size; ++i)
    dest[i] = RoundToInt16(src[i]);
}

void ScaleAndRoundToInt16(const float* src, int size, int16_t* dest) {
  for (int i = 0; i < size; ++i)
    dest[i] = ScaleAndRoundToInt16(src[i]);
}

void ScaleToFloat(const int16_t* src, int size, float* dest) {
  for (int i = 0; i < size; ++i)
    dest[i] = ScaleToFloat(src[i]);
}

}  // namespace VoIP
