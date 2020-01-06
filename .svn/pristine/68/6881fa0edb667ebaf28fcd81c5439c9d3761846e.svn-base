#ifndef VOIP_MODULES_AUDIO_PROCESSING_LEVEL_ESTIMATOR_IMPL_H_
#define VOIP_MODULES_AUDIO_PROCESSING_LEVEL_ESTIMATOR_IMPL_H_

#include "audio_engine/modules/audio_processing/include/audio_processing.h"
#include "audio_engine/modules/audio_processing/processing_component.h"

namespace VoIP {

class AudioBuffer;
class CriticalSectionWrapper;

class LevelEstimatorImpl : public LevelEstimator,
                           public ProcessingComponent {
 public:
  LevelEstimatorImpl(const AudioProcessing* apm,
                     CriticalSectionWrapper* crit);
  virtual ~LevelEstimatorImpl();

  int ProcessStream(AudioBuffer* audio);

  // LevelEstimator implementation.
  virtual bool is_enabled() const OVERRIDE;

 private:
  // LevelEstimator implementation.
  virtual int Enable(bool enable) OVERRIDE;
  virtual int RMS() OVERRIDE;

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

#endif  // VOIP_MODULES_AUDIO_PROCESSING_LEVEL_ESTIMATOR_IMPL_H_
