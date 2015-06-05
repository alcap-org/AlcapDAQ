#include "DT5720DataFormats.h"

#include <stdint.h>
#include <cstdio>
#include <vector>

struct DT5720ChannelData::Header {
  const static int header_size = 2;
  int nwords;
  bool format_en, dualtrace_en, charge_en;
  bool timetag_en, baseline_en, waveform_en;
};

int DT5720ChannelData::Process(uint32_t* data) {
  Header header = ProcessHeader(data);
  if (!header.format_en) {
    printf("DT5720 Error: Missing format string in data.\n");
    return -1;
  }
  if (!header.waveform_en) {
    printf("DT5720 Error: Missing waveforms.\n");
    return -1;
  }
  if (!header.timetag_en) {
    printf("DT5720 Error: Missing trigger time tag.\n");
    return -1;
  }

  const int n_samps_per_word = 2;
  const int evt_size = header.waveform_en*(waveform_length_/n_samps_per_word) +
                       header.charge_en + header.timetag_en
                       + header.baseline_en;
  const int nevt = (header.nwords - Header::header_size)/evt_size;
  waveforms_.reserve(nevt);

  data += Header::header_size;
  int nwords_processed = Header::header_size;
  for (int ievt = 0; ievt < nevt; ++ievt) {
   if (header.timetag_en) {
      time_tags_.push_back(*data & 0xFFFFFFFF);
      ++data; ++nwords_processed;
    }
    if (header.waveform_en) {
      std::vector<int> adcs;
      adcs.reserve(waveform_length_);
      for (int iword = 0; iword < waveform_length_/n_samps_per_word;
           ++iword, ++data)
        for (int isamp = 0; isamp < n_samps_per_word; ++isamp)
          adcs.push_back((*data >> (16*isamp)) & 0xFFF);
      waveforms_.push_back(adcs);
      nwords_processed += waveform_length_/n_samps_per_word;
    }
    if (header.baseline_en) ++data;
    if (header.charge_en) ++data;
  }
  return nwords_processed;
}

DT5720ChannelData::Header DT5720ChannelData::ProcessHeader(uint32_t* data) {
  const uint32_t SIZE_MASK         = 0x00007FFF;
  const uint32_t FORMAT_EN_MASK    = 0x80000000;
  const uint32_t NSAMPLES_MASK     = 0x00000FFF;
  const uint32_t WAVEFORM_EN_MASK  = 0x08000000;
  const uint32_t BASELINE_EN_MASK  = 0x10000000;
  const uint32_t TIMETAG_EN_MASK   = 0x20000000;
  const uint32_t CHARGE_EN_MASK    = 0x40000000;
  const uint32_t DUALTRACE_EN_MASK = 0x80000000;

  Header header = {0};
  header.format_en    = data[0] & FORMAT_EN_MASK;
  header.nwords       = data[0] & SIZE_MASK;
  header.dualtrace_en = data[1] & DUALTRACE_EN_MASK;
  header.charge_en    = data[1] & CHARGE_EN_MASK;
  header.timetag_en   = data[1] & TIMETAG_EN_MASK;
  header.baseline_en  = data[1] & BASELINE_EN_MASK;
  header.waveform_en  = data[1] & WAVEFORM_EN_MASK;
  waveform_length_    = 8*(data[1] & NSAMPLES_MASK);
  return header;
}

struct DT5720BoardData::Header {
  const static int header_size = 4;
  int nwords;
  int n_channel_data;
};

int DT5720BoardData::Process(uint32_t* data) {
  const Header header = ProcessHeader(data);
  data += Header::header_size;
  int nwords_processed = Header::header_size;
  for (int ich = 0; ich < kNChan; ++ich) {
    if (channel_enableds_[ich]) {
      const int n = channel_data_[ich].Process(data);
      if (n < 0) return -1;
      nwords_processed += n;
      data += n;
    }
  }
  if (header.nwords != nwords_processed) return -1;
  return nwords_processed;
}

DT5720BoardData::Header DT5720BoardData::ProcessHeader(uint32_t* data) {
  Header header = {0};
  header.nwords = data[0] & 0x0FFFFFFF;
  const int channel_mask = data[1] & 0xF;
  header.n_channel_data = 0;
  for (int i = 0; i < kNChan; ++i) {
    channel_enableds_[i] = channel_mask & 1 << i;
    ++header.n_channel_data;
  }
  return header;
}
