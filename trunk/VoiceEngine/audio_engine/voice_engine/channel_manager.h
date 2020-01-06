
#ifndef VOIP_AUDIO_ENGINE_CHANNEL_MANAGER_H
#define VOIP_AUDIO_ENGINE_CHANNEL_MANAGER_H

#include <vector>

#include "audio_engine/system_wrappers/interface/atomic32.h"
#include "audio_engine/system_wrappers/interface/constructor_magic.h"
#include "audio_engine/system_wrappers/interface/critical_section_wrapper.h"
#include "audio_engine/system_wrappers/interface/scoped_ptr.h"
#include "audio_engine/include/typedefs.h"

namespace VoIP {

class Config;

namespace aoe {

class Channel;

// Shared-pointer implementation for keeping track of Channels. The underlying
// shared instance will be dropped when no more ChannelOwners point to it.
//
// One common source of ChannelOwner instances are
// ChannelManager::CreateChannel() and ChannelManager::GetChannel(...).
// It has a similar use case to shared_ptr in C++11. Should this move to C++11
// in the future, this class should be replaced by exactly that.
//
// To access the underlying Channel, use .channel().
// IsValid() implements a convenience method as an alternative for checking
// whether the underlying pointer is NULL or not.
//
// Channel channel_owner = channel_manager.GetChannel(channel_id);
// if (channel_owner.IsValid())
//   channel_owner.channel()->...;
//
class ChannelOwner {
 public:
  explicit ChannelOwner(Channel* channel);
  ChannelOwner(const ChannelOwner& channel_owner);

  ~ChannelOwner();

  ChannelOwner& operator=(const ChannelOwner& other);

  Channel* channel() { return channel_ref_->channel.get(); }
  bool IsValid() { return channel_ref_->channel.get() != NULL; }
 private:
  // Shared instance of a Channel. Copying ChannelOwners increase the reference
  // count and destroying ChannelOwners decrease references. Channels are
  // deleted when no references to them are held.
  struct ChannelRef {
    ChannelRef(Channel* channel);
    const scoped_ptr<Channel> channel;
    Atomic32 ref_count;
  };

  ChannelRef* channel_ref_;
};

class ChannelManager {
 public:
  ChannelManager(uint32_t instance_id, const Config& config);

  // Upon construction of an Iterator it will grab a copy of the channel list of
  // the ChannelManager. The iteration will then occur over this state, not the
  // current one of the ChannelManager. As the Iterator holds its own references
  // to the Channels, they will remain valid even if they are removed from the
  // ChannelManager.
  class Iterator {
   public:
    explicit Iterator(ChannelManager* channel_manager);

    Channel* GetChannel();
    bool IsValid();

    void Increment();

   private:
    size_t iterator_pos_;
    std::vector<ChannelOwner> channels_;

    DISALLOW_COPY_AND_ASSIGN(Iterator);
  };

  // CreateChannel will always return a valid ChannelOwner instance. The channel
  // is created either based on internal configuration, i.e. |config_|, by
  // calling CreateChannel(), or using and external configuration
  // |external_config| if the overloaded method
  // CreateChannel(const Config& external_config) is called.
  ChannelOwner CreateChannel();
  ChannelOwner CreateChannel(const Config& external_config);

  // ChannelOwner.channel() will be NULL if channel_id is invalid or no longer
  // exists. This should be checked with ChannelOwner::IsValid().
  ChannelOwner GetChannel(int32_t channel_id);
  void GetAllChannels(std::vector<ChannelOwner>* channels);

  void DestroyChannel(int32_t channel_id);
  void DestroyAllChannels();

  size_t NumOfChannels() const;

 private:
  // Create a channel given a configuration, |config|.
  ChannelOwner CreateChannelInternal(const Config& config);

  uint32_t instance_id_;

  Atomic32 last_channel_id_;

  scoped_ptr<CriticalSectionWrapper> lock_;
  std::vector<ChannelOwner> channels_;

  const Config& config_;

  DISALLOW_COPY_AND_ASSIGN(ChannelManager);
};
}  // namespace aoe
}  // namespace VoIP

#endif  // VOIP_AUDIO_ENGINE_CHANNEL_MANAGER_H
