#ifndef VOIP_COMMON_H_
#define VOIP_COMMON_H_

#include <map>

#include "typedefs.h"

namespace VoIP {


// Class Config is designed to ease passing a set of options across VoIP code.
// Options are identified by typename in order to avoid incorrect casts.
//
// Usage:
// * declaring an option:
//    struct Algo1_CostFunction {
//      virtual float cost(int x) const { return x; }
//      virtual ~Algo1_CostFunction() {}
//    };
//
// * accessing an option:
//    config.Get<Algo1_CostFunction>().cost(value);
//
// * setting an option:
//    struct SqrCost : Algo1_CostFunction {
//      virtual float cost(int x) const { return x*x; }
//    };
//    config.Set<Algo1_CostFunction>(new SqrCost());
//
// Note: This class is thread-compatible (like STL containers).
class Config {
 public:
  // Returns the option if set or a default constructed one.
  // Callers that access options too often are encouraged to cache the result.
  // Returned references are owned by this.
  //
  // Requires std::is_default_constructible<T>
  template<typename T> const T& Get() const;

  // Set the option, deleting any previous instance of the same.
  // This instance gets ownership of the newly set value.
  template<typename T> void Set(T* value);

  Config() {}
  ~Config() {
    // Note: this method is inline so VoIP public API depends only
    // on the headers.
    for (OptionMap::iterator it = options_.begin();
         it != options_.end(); ++it) {
      delete it->second;
    }
  }

 private:
  typedef void* OptionIdentifier;

  struct BaseOption {
    virtual ~BaseOption() {}
  };

  template<typename T>
  struct Option : BaseOption {
    explicit Option(T* v): value(v) {}
    ~Option() {
      delete value;
    }
    T* value;
  };

  // Own implementation of rtti-subset to avoid depending on rtti and its costs.
  template<typename T>
  static OptionIdentifier identifier() {
    static char id_placeholder;
    return &id_placeholder;
  }

  // Used to instantiate a default constructed object that doesn't needs to be
  // owned. This allows Get<T> to be implemented without requiring explicitly
  // locks.
  template<typename T>
  static const T& default_value() {
    static const T def;
    return def;
  }

  typedef std::map<OptionIdentifier, BaseOption*> OptionMap;
  OptionMap options_;

  // DISALLOW_COPY_AND_ASSIGN
  Config(const Config&);
  void operator=(const Config&);
};

template<typename T>
const T& Config::Get() const {
  OptionMap::const_iterator it = options_.find(identifier<T>());
  if (it != options_.end()) {
    const T* t = static_cast<Option<T>*>(it->second)->value;
    if (t) {
      return *t;
    }
  }
  return default_value<T>();
}

template<typename T>
void Config::Set(T* value) {
  BaseOption*& it = options_[identifier<T>()];
  delete it;
  it = new Option<T>(value);
}


/* This class holds up to 60 ms of super-wideband (32 kHz) stereo audio. It
 * allows for adding and subtracting frames while keeping track of the resulting
 * states.
 *
 * Notes
 * - The total number of samples in |data_| is
 *   samples_per_channel_ * num_channels_
 *
 * - Stereo data is interleaved starting with the left channel.
 *
 * - The +operator assume that you would never add exactly opposite frames when
 *   deciding the resulting state. To do this use the -operator.
 */
class AudioFrame {
 public:
  // Stereo, 32 kHz, 60 ms (2 * 32 * 60) = 3840
  //mono 8k 40ms 1*8*40=320
  static const int kMaxDataSizeSamples = 320;

  enum VADActivity {
    kVadActive = 0,
    kVadPassive = 1,
    kVadUnknown = 2
  };
  enum SpeechType {
    kNormalSpeech = 0,
    kPLC = 1,
    kCNG = 2,
    kPLCCNG = 3,
    kUndefined = 4
  };

  AudioFrame();
  virtual ~AudioFrame() {}

  // |interleaved_| is not changed by this method.
  void UpdateFrame(int id, uint32_t timestamp, const int16_t* data,
                   int samples_per_channel, int sample_rate_hz,
                   SpeechType speech_type, VADActivity vad_activity,
                   int num_channels = 1, uint32_t energy = -1);

  AudioFrame& Append(const AudioFrame& rhs);

  void CopyFrom(const AudioFrame& src);

  void Mute();

  AudioFrame& operator>>=(const int rhs);
  AudioFrame& operator+=(const AudioFrame& rhs);
  AudioFrame& operator-=(const AudioFrame& rhs);

  int id_;
  uint32_t timestamp_;
  int16_t data_[kMaxDataSizeSamples];
  int samples_per_channel_;
  int sample_rate_hz_;
  int num_channels_;
  SpeechType speech_type_;
  VADActivity vad_activity_;
  uint32_t energy_;
  bool interleaved_;

 private:
  DISALLOW_COPY_AND_ASSIGN(AudioFrame);
};

inline AudioFrame::AudioFrame()
    : id_(-1),
      timestamp_(0),
      data_(),
      samples_per_channel_(0),
      sample_rate_hz_(0),
      num_channels_(1),
      speech_type_(kUndefined),
      vad_activity_(kVadUnknown),
      energy_(0xffffffff),
      interleaved_(true) {}

inline void AudioFrame::UpdateFrame(int id, uint32_t timestamp,
                                    const int16_t* data,
                                    int samples_per_channel, int sample_rate_hz,
                                    SpeechType speech_type,
                                    VADActivity vad_activity, int num_channels,
                                    uint32_t energy) {
  id_ = id;
  timestamp_ = timestamp;
  samples_per_channel_ = samples_per_channel;
  sample_rate_hz_ = sample_rate_hz;
  speech_type_ = speech_type;
  vad_activity_ = vad_activity;
  num_channels_ = num_channels;
  energy_ = energy;

  const int length = samples_per_channel * num_channels;
  assert(length <= kMaxDataSizeSamples && length >= 0);
  if (data != NULL) {
    memcpy(data_, data, sizeof(int16_t) * length);
  } else {
    memset(data_, 0, sizeof(int16_t) * length);
  }
}

inline void AudioFrame::CopyFrom(const AudioFrame& src) {
  if (this == &src) return;

  id_ = src.id_;
  timestamp_ = src.timestamp_;
  samples_per_channel_ = src.samples_per_channel_;
  sample_rate_hz_ = src.sample_rate_hz_;
  speech_type_ = src.speech_type_;
  vad_activity_ = src.vad_activity_;
  num_channels_ = src.num_channels_;
  energy_ = src.energy_;
  interleaved_ = src.interleaved_;

  const int length = samples_per_channel_ * num_channels_;
  assert(length <= kMaxDataSizeSamples && length >= 0);
  memcpy(data_, src.data_, sizeof(int16_t) * length);
}

inline void AudioFrame::Mute() {
  memset(data_, 0, samples_per_channel_ * num_channels_ * sizeof(int16_t));
}

inline AudioFrame& AudioFrame::operator>>=(const int rhs) {
  assert((num_channels_ > 0) && (num_channels_ < 3));
  if ((num_channels_ > 2) || (num_channels_ < 1)) return *this;

  for (int i = 0; i < samples_per_channel_ * num_channels_; i++) {
    data_[i] = static_cast<int16_t>(data_[i] >> rhs);
  }
  return *this;
}

inline AudioFrame& AudioFrame::Append(const AudioFrame& rhs) {
  // Sanity check
  assert((num_channels_ > 0) && (num_channels_ < 3));
  assert(interleaved_ == rhs.interleaved_);
  if ((num_channels_ > 2) || (num_channels_ < 1)) return *this;
  if (num_channels_ != rhs.num_channels_) return *this;

  if ((vad_activity_ == kVadActive) || rhs.vad_activity_ == kVadActive) {
    vad_activity_ = kVadActive;
  } else if (vad_activity_ == kVadUnknown || rhs.vad_activity_ == kVadUnknown) {
    vad_activity_ = kVadUnknown;
  }
  if (speech_type_ != rhs.speech_type_) {
    speech_type_ = kUndefined;
  }

  int offset = samples_per_channel_ * num_channels_;
  for (int i = 0; i < rhs.samples_per_channel_ * rhs.num_channels_; i++) {
    data_[offset + i] = rhs.data_[i];
  }
  samples_per_channel_ += rhs.samples_per_channel_;
  return *this;
}

inline AudioFrame& AudioFrame::operator+=(const AudioFrame& rhs) {
  // Sanity check
  assert((num_channels_ > 0) && (num_channels_ < 3));
  assert(interleaved_ == rhs.interleaved_);
  if ((num_channels_ > 2) || (num_channels_ < 1)) return *this;
  if (num_channels_ != rhs.num_channels_) return *this;

  bool noPrevData = false;
  if (samples_per_channel_ != rhs.samples_per_channel_) {
    if (samples_per_channel_ == 0) {
      // special case we have no data to start with
      samples_per_channel_ = rhs.samples_per_channel_;
      noPrevData = true;
    } else {
      return *this;
    }
  }

  if ((vad_activity_ == kVadActive) || rhs.vad_activity_ == kVadActive) {
    vad_activity_ = kVadActive;
  } else if (vad_activity_ == kVadUnknown || rhs.vad_activity_ == kVadUnknown) {
    vad_activity_ = kVadUnknown;
  }

  if (speech_type_ != rhs.speech_type_) speech_type_ = kUndefined;

  if (noPrevData) {
    memcpy(data_, rhs.data_,
           sizeof(int16_t) * rhs.samples_per_channel_ * num_channels_);
  } else {
    // IMPROVEMENT this can be done very fast in assembly
    for (int i = 0; i < samples_per_channel_ * num_channels_; i++) {
      int32_t wrapGuard =
          static_cast<int32_t>(data_[i]) + static_cast<int32_t>(rhs.data_[i]);
      if (wrapGuard < -32768) {
        data_[i] = -32768;
      } else if (wrapGuard > 32767) {
        data_[i] = 32767;
      } else {
        data_[i] = (int16_t)wrapGuard;
      }
    }
  }
  energy_ = 0xffffffff;
  return *this;
}

inline AudioFrame& AudioFrame::operator-=(const AudioFrame& rhs) {
  // Sanity check
  assert((num_channels_ > 0) && (num_channels_ < 3));
  assert(interleaved_ == rhs.interleaved_);
  if ((num_channels_ > 2) || (num_channels_ < 1)) return *this;

  if ((samples_per_channel_ != rhs.samples_per_channel_) ||
      (num_channels_ != rhs.num_channels_)) {
    return *this;
  }
  if ((vad_activity_ != kVadPassive) || rhs.vad_activity_ != kVadPassive) {
    vad_activity_ = kVadUnknown;
  }
  speech_type_ = kUndefined;

  for (int i = 0; i < samples_per_channel_ * num_channels_; i++) {
    int32_t wrapGuard =
        static_cast<int32_t>(data_[i]) - static_cast<int32_t>(rhs.data_[i]);
    if (wrapGuard < -32768) {
      data_[i] = -32768;
    } else if (wrapGuard > 32767) {
      data_[i] = 32767;
    } else {
      data_[i] = (int16_t)wrapGuard;
    }
  }
  energy_ = 0xffffffff;
  return *this;
}

}  // namespace VoIP

#endif  // VOIP_COMMON_H_
