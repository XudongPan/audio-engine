
#include "audio_engine/modules/audio_processing/voice_detection_impl.h"

#include <assert.h>

#include "audio_engine/common_audio/vad/include/voip_vad.h"
#include "audio_engine/modules/audio_processing/audio_buffer.h"
#include "audio_engine/system_wrappers/interface/critical_section_wrapper.h"

namespace VoIP {

typedef VadInst Handle;

namespace {
int MapSetting(VoiceDetection::Likelihood likelihood) {
  switch (likelihood) {
    case VoiceDetection::kVeryLowLikelihood:
      return 3;
    case VoiceDetection::kLowLikelihood:
      return 2;
    case VoiceDetection::kModerateLikelihood:
      return 1;
    case VoiceDetection::kHighLikelihood:
      return 0;
  }
  assert(false);
  return -1;
}
}  // namespace

VoiceDetectionImpl::VoiceDetectionImpl(const AudioProcessing* apm,
                                       CriticalSectionWrapper* crit)
  : ProcessingComponent(),
    apm_(apm),
    crit_(crit),
    stream_has_voice_(false),
    using_external_vad_(false),
    likelihood_(kLowLikelihood),
    frame_size_ms_(10),
    frame_size_samples_(0) {}

VoiceDetectionImpl::~VoiceDetectionImpl() {}

int VoiceDetectionImpl::ProcessCaptureAudio(AudioBuffer* audio) {
  if (!is_component_enabled()) {
    return apm_->kNoError;
  }

  if (using_external_vad_) {
    using_external_vad_ = false;
    return apm_->kNoError;
  }
  assert(audio->samples_per_split_channel() <= 160);

  int16_t* mixed_data = audio->low_pass_split_data(0);
  if (audio->num_channels() > 1) {
    audio->CopyAndMixLowPass(1);
    mixed_data = audio->mixed_low_pass_data(0);
  }

  // TODO(ajm): concatenate data in frame buffer here.

  int vad_ret = VoipVad_Process(static_cast<Handle*>(handle(0)),
                                  apm_->split_sample_rate_hz(),
                                  mixed_data,
                                  frame_size_samples_);
  if (vad_ret == 0) {
    stream_has_voice_ = false;
    audio->set_activity(AudioFrame::kVadPassive);
  } else if (vad_ret == 1) {
    stream_has_voice_ = true;
    audio->set_activity(AudioFrame::kVadActive);
  } else {
    return apm_->kUnspecifiedError;
  }

  return apm_->kNoError;
}

int VoiceDetectionImpl::Enable(bool enable) {
  CriticalSectionScoped crit_scoped(crit_);
  return EnableComponent(enable);
}

bool VoiceDetectionImpl::is_enabled() const {
  return is_component_enabled();
}

int VoiceDetectionImpl::set_stream_has_voice(bool has_voice) {
  using_external_vad_ = true;
  stream_has_voice_ = has_voice;
  return apm_->kNoError;
}

bool VoiceDetectionImpl::stream_has_voice() const {
  // TODO(ajm): enable this assertion?
  //assert(using_external_vad_ || is_component_enabled());
  return stream_has_voice_;
}

int VoiceDetectionImpl::set_likelihood(VoiceDetection::Likelihood likelihood) {
  CriticalSectionScoped crit_scoped(crit_);
  if (MapSetting(likelihood) == -1) {
    return apm_->kBadParameterError;
  }

  likelihood_ = likelihood;
  return Configure();
}

VoiceDetection::Likelihood VoiceDetectionImpl::likelihood() const {
  return likelihood_;
}

int VoiceDetectionImpl::set_frame_size_ms(int size) {
  CriticalSectionScoped crit_scoped(crit_);
  assert(size == 10); // TODO(ajm): remove when supported.
  if (size != 10 &&
      size != 20 &&
      size != 30) {
    return apm_->kBadParameterError;
  }

  frame_size_ms_ = size;

  return Initialize();
}

int VoiceDetectionImpl::frame_size_ms() const {
  return frame_size_ms_;
}

int VoiceDetectionImpl::Initialize() {
  int err = ProcessingComponent::Initialize();
  if (err != apm_->kNoError || !is_component_enabled()) {
    return err;
  }

  using_external_vad_ = false;
  frame_size_samples_ = frame_size_ms_ * (apm_->split_sample_rate_hz() / 1000);
  // TODO(ajm): intialize frame buffer here.

  return apm_->kNoError;
}

void* VoiceDetectionImpl::CreateHandle() const {
  Handle* handle = NULL;
  if (VoipVad_Create(&handle) != apm_->kNoError) {
    handle = NULL;
  } else {
    assert(handle != NULL);
  }

  return handle;
}

int VoiceDetectionImpl::DestroyHandle(void* handle) const {
  return VoipVad_Free(static_cast<Handle*>(handle));
}

int VoiceDetectionImpl::InitializeHandle(void* handle) const {
  return VoipVad_Init(static_cast<Handle*>(handle));
}

int VoiceDetectionImpl::ConfigureHandle(void* handle) const {
  return VoipVad_set_mode(static_cast<Handle*>(handle),
                            MapSetting(likelihood_));
}

int VoiceDetectionImpl::num_handles_required() const {
  return 1;
}

int VoiceDetectionImpl::GetHandleError(void* handle) const {
  // The VAD has no get_error() function.
  assert(handle != NULL);
  return apm_->kUnspecifiedError;
}
}  // namespace VoIP
