#include "DT5730DataFormats.h"

#include <stdint.h>
#include <vector>

struct DT5730ChannelData::Header {
  const static int header_size = 2;
  int nwords;
  bool format_en, dualtrace_en, charge_en;
  bool timetag_en, extras_en, waveform_en;
};

int DT5730ChannelData::Process(uint32_t* data, bool odd) {
  Header header = ProcessHeader(data);
  if (!header.format_en) return -1; // Sanity check, always enabled
  if (!header.waveform_en || !header.timetag_en) return -1;

  const int n_samps_per_word = 2;
  const int evt_size = header.waveform_en*(waveform_length_/n_samps_per_word) +
                       header.charge_en + header.timetag_en + header.extras_en;
  const int nevt = (header.nwords - Header::header_size)/evt_size;
  waveforms_.reserve(nevt);

  data += Header::header_size;
  int nwords_processed = Header::header_size;
  for (int ievt = 0; ievt < nevt; ++ievt) {
    // Since the DT5730 groups every two channels together, here we ignore
    // the even channel if "odd" is true, and vice-versa.
    if (odd != *data>>31) {
      data += evt_size;
      continue;
    }
    if (header.timetag_en) {
      time_tags_.push_back(*data & 0x7FFFFFFF);
      ++data; ++nwords_processed;
    }
    if (header.waveform_en) {
      std::vector<int> adcs;
      adcs.reserve(waveform_length_);
      for (int iword = 0; iword < waveform_length_/n_samps_per_word;
           ++iword, ++data)
        for (int isamp = 0; isamp < n_samps_per_word; ++isamp)
          adcs.push_back((*data >> (16*isamp)) & 0x3FFF);
      waveforms_.push_back(adcs);
      nwords_processed += waveform_length_/n_samps_per_word;
    }
    if (header.extras_en) ++data;
    if (header.charge_en) ++data;
  }
  return nwords_processed;
}

DT5730ChannelData::Header DT5730ChannelData::ProcessHeader(uint32_t* data) {
  const uint32_t SIZE_MASK         = 0x00007FFF;
  const uint32_t FORMAT_EN_MASK    = 0x80000000;
  const uint32_t NSAMPLES_MASK     = 0x00000FFF;
  const uint32_t WAVEFORM_EN_MASK  = 0x08000000;
  const uint32_t EXTRAS_EN_MASK    = 0x10000000;
  const uint32_t TIMETAG_EN_MASK   = 0x20000000;
  const uint32_t CHARGE_EN_MASK    = 0x40000000;
  const uint32_t DUALTRACE_EN_MASK = 0x80000000;

  Header header = {0};
  header.format_en    = data[0] & FORMAT_EN_MASK;
  header.nwords       = data[0] & SIZE_MASK;
  header.dualtrace_en = data[1] & DUALTRACE_EN_MASK;
  header.charge_en    = data[1] & CHARGE_EN_MASK;
  header.timetag_en   = data[1] & TIMETAG_EN_MASK;
  header.extras_en    = data[1] & EXTRAS_EN_MASK;
  header.waveform_en  = data[1] & WAVEFORM_EN_MASK;
  waveform_length_    = 8*(data[1] & NSAMPLES_MASK);
  return header;
}

struct DT5730BoardData::Header {
  const static int header_size = 4;
  int nwords;
  int n_channel_data;
};

int DT5730BoardData::Process(uint32_t* data) {
  const Header header = ProcessHeader(data);
  data += Header::header_size;
  int nwords_processed = Header::header_size;
  int ich = 0;
  for (int i_channel_data = 0; i_channel_data < header.n_channel_data;
       ++i_channel_data) {
    while (channel_enableds_[ich] == false) ++ich;
    bool is_odd_channel = ich % 2;
    if (!is_odd_channel) {
      const int n = channel_data_[ich].Process(data, is_odd_channel);
      if (n < 0) return -1;
      nwords_processed += n;
      data += n;
      if (!channel_enableds_[++ich]) continue;
      is_odd_channel = !is_odd_channel;
    }
    const int n = channel_data_[ich].Process(data, is_odd_channel);
    if (n < 0) return -1;
    nwords_processed += n;
    data += n;
    ++ich;
  }
  if (header.nwords != nwords_processed) return -1;
  return nwords_processed;
}

DT5730BoardData::Header DT5730BoardData::ProcessHeader(uint32_t* data) {
  Header header = {0};
  header.nwords = data[0] & 0x0FFFFFFF;
  const int channel_mask = data[1] & 0xFF;
  for (int i = 0; i < kNChan; ++i)
    channel_enableds_[i] = channel_mask & 1 << i;
  header.n_channel_data = 0;
  for (int i = 0; i < kNChan; i += 2)
    if ( channel_enableds_[i] || channel_enableds_[i+1])
      ++header.n_channel_data;
  return header;
}
