#ifndef DT5720DATAFORMATS_H_
#define DT5720DATAFORMATS_H_

#include <stdint.h>
#include <vector>

class DT5720ChannelData {
public:
  DT5720ChannelData();
  // Tell the object where the data is, and it will go through
  // and fill its members. The supplied data does not get referenced
  // after returning so can be safely deleted.
  // Returns the number of 32-bit words processed upon success
  // and -1 on failure.
  int Process(uint32_t* data);
  int num_events() const {
    return waveforms_.size();
  }
  int waveform_length() const {
    return waveform_length_;
  }
  uint32_t time_tag(int evt) const {
    return time_tags_[evt];
  }
  const std::vector<int>& waveform(int evt) const {
    return waveforms_[evt];
  }
  bool processed() const {
    return processed_;
  }

private:
  struct Header;
  Header ProcessHeader(uint32_t* data);

  bool processed_;
  int waveform_length_;
  std::vector<uint32_t> time_tags_;
  std::vector< std::vector<int> > waveforms_;
};

class DT5720BoardData {
public:
  const static int kNChan = 4;
  DT5720BoardData();
  // Tell the object where the data is, and it will go through
  // and fill its members. The supplied data does not get referenced
  // after returning so can safely be deleted.
  // Returns the number of 32-bit words processed upon success.
  // Return -1 on failure.
  int Process(uint32_t* data);
  bool channel_enabled(int ch) const {
      return channel_enableds_[ch];
    }
  const DT5720ChannelData& channel_data(int ch) const {
      return channel_data_[ch];
  }
  bool processed() {
    return processed_;
  }

private:
  struct Header;
  Header ProcessHeader(uint32_t* data);

  bool processed_;
  bool boardfail_;
  bool channel_enableds_[kNChan];
  DT5720ChannelData channel_data_[kNChan];
};

#endif // Include guard
