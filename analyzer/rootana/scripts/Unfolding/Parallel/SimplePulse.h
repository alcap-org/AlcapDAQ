#ifndef SimplePulse_h_
#define SimplePulse_h_

enum BitMask { kOK=1, kDoubleCounted=2, kPileUp=4 };

struct SimplePulse {
public:

SimplePulse() : tpi_id(0), E(0), Amp(0), tblock(0), tTME(0), bit_mask(0) { }

SimplePulse(Int_t id, Double_t energy, Double_t amplitude, Double_t block_time, Double_t tme_time, Char_t mask) :
  tpi_id(id), E(energy), Amp(amplitude), tblock(block_time), tTME(tme_time), bit_mask(mask) { };

  Int_t tpi_id; // ID of the TPulseIsland this pulse came from
  Double_t E; // energy of this pulse
  Double_t Amp; // amplitude of this pulse
  Double_t tblock; // MIDAS block time of this pulse (between 0 and 100 ms)
  Double_t tTME; // time to the central muon in this TMuonEvent
  Char_t bit_mask; // used for various statuses (e.g. double counted, unresolved pile-up etc.)
};


#endif
