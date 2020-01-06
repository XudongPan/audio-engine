#ifndef VOIP_MODULES_AUDIO_PROCESSING_NOISE_SUPPRESSION_IMPL_H_
#define VOIP_MODULES_AUDIO_PROCESSING_NOISE_SUPPRESSION_IMPL_H_

#include "audio_engine/modules/audio_processing/include/audio_processing.h"
#include "audio_engine/modules/audio_processing/processing_component.h"

namespace VoIP {

class AudioBuffer;
class CriticalSectionWrapper;

class NoiseSuppressionImpl : public NoiseSuppression,
                             public ProcessingComponent {
 public:
  NoiseSuppressionImpl(const AudioProcessing* apm,
                       CriticalSectionWrapper* crit);
  virtual ~NoiseSuppressionImpl();

  int ProcessCaptureAudio(AudioBuffer* audio);

  // NoiseSuppression implementation.
  virtual bool is_enabled() const OVERRIDE;
  virtual float speech_probability() const OVERRIDE;

 private:
  // NoiseSuppression implementation.
  virtual int Enable(bool enable) OVERRIDE;
  virtual int set_level(Level level) OVERRIDE;
  virtual Level level() const OVERRIDE;

  // ProcessingComponent implementation.
  virtual void* CreateHandle() const OVERRIDE;
  virtual int InitializeHandle(void* handle) const OVERRIDE;
  virtual int ConfigureHandle(void* handle) const OVERRIDE;
  virtual int DestroyHandle(void* handle) const OVERRIDE;
  virtual int num_handles_required() const OVERRIDE;
  virtual int GetHandleError(void* handle) const OVERRIDE;

  const AudioProcessing* apm_;
  CriticalSectionWrapper* crit_;
  Level level_;
};

}  // namespace VoIP

#endif  // VOIP_MODULES_AUDIO_PROCESSING_NOISE_SUPPRESSION_IMPL_H_
