#ifndef VOIP_MODULES_AUDIO_PROCESSING_HIGH_PASS_FILTER_IMPL_H_
#define VOIP_MODULES_AUDIO_PROCESSING_HIGH_PASS_FILTER_IMPL_H_

#include "audio_engine/modules/audio_processing/include/audio_processing.h"
#include "audio_engine/modules/audio_processing/processing_component.h"

namespace VoIP {

class AudioBuffer;
class CriticalSectionWrapper;

class HighPassFilterImpl : public HighPassFilter,
                           public ProcessingComponent {
 public:
  HighPassFilterImpl(const AudioProcessing* apm, CriticalSectionWrapper* crit);
  virtual ~HighPassFilterImpl();

  int ProcessCaptureAudio(AudioBuffer* audio);

  // HighPassFilter implementation.
  virtual bool is_enabled() const OVERRIDE;

 private:
  // HighPassFilter implementation.
  virtual int Enable(bool enable) OVERRIDE;

  // ProcessingComponent implementation.
  virtual void* CreateHandle() const OVERRIDE;
  virtual int InitializeHandle(void* handle) const OVERRIDE;
  virtual int ConfigureHandle(void* handle) const OVERRIDE;
  virtual int DestroyHandle(void* handle) const OVERRIDE;
  virtual int num_handles_required() const OVERRIDE;
  virtual int GetHandleError(void* handle) const OVERRIDE;

  const AudioProcessing* apm_;
  CriticalSectionWrapper* crit_;
};
}  // namespace VoIP

#endif  // VOIP_MODULES_AUDIO_PROCESSING_HIGH_PASS_FILTER_IMPL_H_
