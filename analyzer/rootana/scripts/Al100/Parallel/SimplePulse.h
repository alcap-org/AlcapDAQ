#ifndef SimplePulse_h_
#define SimplePulse_h_

enum BitMask { kOK=1, kDoubleCounted=2, kCutOff=4 };

struct SimplePulse {
public:

SimplePulse() : tpi_id(0), tap_id(0), E(0), Amp(0), Sum(0), tblock(0), tTME(0), bit_mask(0) { }

SimplePulse(Int_t TPIid, Int_t TAPid, Double_t energy, Double_t amplitude, Double_t integral, Double_t block_time, Double_t tme_time, Char_t mask) :
  tpi_id(TPIid), tap_id(TAPid), E(energy), Amp(amplitude), Sum(integral), tblock(block_time), tTME(tme_time), bit_mask(mask) { };

  Int_t tpi_id;    // ID of the TPulseIsland this pulse came from
  Int_t tap_id;    // ID of the TAnalysedPulse this pulse came from (can have multiple TAPs from a single TPI)
  Double_t E;      // energy of this pulse
  Double_t Amp;    // amplitude of this pulse
  Double_t Sum; // integral of this pulse
  Double_t tblock; // MIDAS block time of this pulse (between 0 and 100 ms)
  Double_t tTME;   // time to the central muon in this TMuonEvent
  Char_t bit_mask; // used for various statuses (e.g. double counted, unresolved pile-up etc.)
};

#endif
