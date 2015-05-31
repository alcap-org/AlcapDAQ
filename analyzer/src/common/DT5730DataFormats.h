#ifndef DT5730DATAFORMATS_H_
#define DT5730DATAFORMATS_H_

#include <stdint.h>
#include <vector>

class DT5730ChannelData {
public:
  // Tell the object where the data is, and it will go through
  // and fill its members. The supplied data does not get referenced
  // after returning so can be safely deleted.
  // Second input says whether to process the even or odd (true) pair
  // as the DT5730 bunches every 2 channels together.
  // Returns the number of 32-bit words processed upon success
  // and -1 on failure.
  int Process(uint32_t* data, bool odd);
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

private:
  struct Header;
  Header ProcessHeader(uint32_t* data);

  int waveform_length_;
  std::vector<uint32_t> time_tags_;
  std::vector< std::vector<int> > waveforms_;
};

class DT5730BoardData {
public:
  // Tell the object where the data is, and it will go through
  // and fill its members. The supplied data does not get referenced
  // after returning so can safely be deleted.
  // Returns the number of 32-bit words processed upon success.
  // Return -1 on failure.
  int Process(uint32_t* data);
  bool channel_enabled(int ch) const {
      return channel_enableds_[ch];
    }
  const DT5730ChannelData& channel_data(int ch) const {
      return channel_data_[ch];
  }

private:
  struct Header;
  Header ProcessHeader(uint32_t* data);

  const static int kNChan = 8;
  bool error_;
  bool channel_enableds_[kNChan];
  DT5730ChannelData channel_data_[kNChan];
};

#endif // Include guard
