#include "DT5730DataFormats.h"

#include <stdint.h>
#include <cstdio>
#include <vector>

DT5730ChannelData::DT5730ChannelData() : processed_(false), waveform_length_(0),
                                        time_tags_(), waveforms_() {
}

struct DT5730ChannelData::Header {
  const static int header_size = 2;
  int nwords;
  bool format_en, dualtrace_en, charge_en;
  bool timetag_en, extras_en, waveform_en;
};

int DT5730ChannelData::Process(uint32_t* data, bool odd,
                               bool& ch_data_present) {
  if (processed_) {
    printf("DT5730 channel already processed!\n");
    return -1;
  }
  Header header = ProcessHeader(data);
  if (!header.format_en) {
    printf("DT5730 dual channel header missing format word!\n");
    return -1;
  }
  if (!header.waveform_en || !header.timetag_en) {
    printf("DT5730 channel missing waveform or time tag (%d, %d)!\n",
           header.waveform_en, header.timetag_en);
    return -1;
  }

  const int n_samps_per_word = 2;
  const int evt_size = header.waveform_en*(waveform_length_/n_samps_per_word) +
                       header.charge_en + header.timetag_en + header.extras_en;
  const int nevt = (header.nwords - Header::header_size)/evt_size;
  waveforms_.reserve(nevt);

  printf("DT5730 channel event size and nevts: %d, %d\n", evt_size, nevt);

  data += Header::header_size;
  int nwords_processed = Header::header_size;
  ch_data_present = false;
  for (int ievt = 0; ievt < nevt; ++ievt) {
    // Since the DT5730 groups every two channels together, here we ignore
    // the even channel if "odd" is true, and vice-versa.
    if (odd != *data>>31) {
      data += evt_size;
      nwords_processed += evt_size;
      continue;
    }
    ch_data_present = true;
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

DT5730BoardData::DT5730BoardData() : processed_(false), error_(false) {
  for (int i = 0; i < kNChan; ++i) {
    channel_enableds_[i] = false;
    channel_data_[i] = DT5730ChannelData();
  }
}

struct DT5730BoardData::Header {
  const static int header_size = 4;
  int nwords;
  int n_channel_data;
  bool chpair_en[kNChan/2];
};

int DT5730BoardData::Process(uint32_t* data) {
  if (processed_) {
    printf("DT5730 board processing error: already processed!\n");
    return -1;
  }
  processed_ = true;

  const Header header = ProcessHeader(data);
  data += Header::header_size;
  int nwords_processed = Header::header_size;
  for (int ichpair = 0; ichpair < kNChan/2; ++ichpair) {
    if (!header.chpair_en[ichpair]) {
      channel_enableds_[2*ichpair] = channel_enableds_[2*ichpair+1] = false;
      continue;
    }
    bool is_data_present;

    bool is_odd_ch = false;
    int ich = 2*ichpair;
    int n = channel_data_[ich].Process(data, is_odd_ch, is_data_present);
    if (n < 0) {
      printf("DT5730 error processing channel %d\n", ich);
      return -1;
    }
    if (is_data_present) channel_enableds_[ich] = true;

    is_odd_ch = true;
    ++ich;
    channel_data_[ich].Process(data, is_odd_ch, is_data_present);
    if (n < 0) {
      printf("DT5730 error processing channel %d\n", ich);
      return -1;
    }
    if (is_data_present) channel_enableds_[ich] = true;

    data += n;
    nwords_processed += n;
  }
  return nwords_processed;
}

DT5730BoardData::Header DT5730BoardData::ProcessHeader(uint32_t* data) {
  Header header = {0};
  header.nwords = data[0] & 0x0FFFFFFF;
  const int channel_pair_mask = data[1] & 0xF;
  for (int i = 0; i < kNChan/2; ++i)
    header.chpair_en[i] = channel_pair_mask & 1 << i;
  return header;
}
