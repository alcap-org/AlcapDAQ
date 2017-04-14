#include "IdentifySyncs.h"
#include "RegisterModule.inc"
#include "TGlobalData.h"
#include "TSetupData.h"
#include "ModulesOptions.h"
#include "definitions.h"
#include "TAnalysedPulse.h"

#include <iostream>
#include <algorithm>
#include <cassert>
#include <cmath>
using std::cout;
using std::endl;
using std::string;
using std::vector;
using std::map;
using std::find;

extern SourceAnalPulseMap gAnalysedPulseMap;

class Sync {
public:
  int ch, i;
  double t, e;
  Sync(int ch = -1, int i = -1, double t = -1, double e = -1) :
  ch(ch), i(i), t(t), e(e) {}
  Sync(int ch, int i, TAnalysedPulse* tap) :
  ch(ch), i(i), t(tap->GetTime()), e(tap->GetAmplitude()) {}
  bool Valid() {
    return ch != -1 && i != -1;
  }
  void Print() {
    std::cout << "ch " << ch << ", evt " << i
              << ", t " << t << ", e " << e << std::endl;
  }
};

class Syncs {
 private:
  static double eps_t;
  static double eps_e;
  static double min_dt;
 public:
  double dt;
  Sync s0, sf;
  Syncs(const Sync& s0 = Sync(), const Sync& sf = Sync()) :
        dt(sf.t-s0.t), s0(s0), sf(sf) {}
  double operator[] (int i) const {
    switch (i) {
    case 0:  return s0.t;
    case 1:  return sf.t;
    default: assert(0);
    }
  }
  std::pair<int, int> Indexes() const {
    return std::make_pair(s0.e, sf.i);
  }
  static bool ValidEDiff(const TAnalysedPulse* t1, const TAnalysedPulse* t2) {
    return ValidEDiff(t1->GetAmplitude(), t2->GetAmplitude());
  }
  static bool ValidTDiff(const TAnalysedPulse* t1, const TAnalysedPulse* t2) {
    return ValidTDiff(t1->GetTime(), t2->GetTime());
  }
  static bool ValidEDiff(double e1, double e2) {
    return 2.*std::abs(e1 - e2)/(e1+e2) < eps_e;
  }
  static bool ValidTDiff(double t1, double t2) {
    return t2 - t1 > min_dt;
  }
  static void SetContraints(double t, double e, double dt) {
    eps_t  = t;
    eps_e  = e;
    min_dt = dt;
  }
  bool operator==(const Syncs& rhs) const {
    return 2.*std::abs(dt - rhs.dt)/(dt+rhs.dt) < eps_t;
  }
  bool operator!=(const Syncs& rhs) const {
    return !(*this == rhs);
  }
  bool operator<(const Syncs& rhs) const {
    return dt < rhs.dt && *this != rhs;
  }
  void Print() {
    std::cout << "s0: "; s0.Print();
    std::cout << "sf: "; sf.Print();
  }
};
double Syncs::eps_t  = 1.e-6;  // ns
double Syncs::eps_e  = 2.5e-1; // % difference
double Syncs::min_dt = 70.e6;  // ns

// Comparators
template <typename T>
bool CompLength(const vector<T>& v1, const vector<T>& v2) {
  return v1.size() < v2.size();
}
bool CompChannel(const Syncs& s1, const Syncs& s2) {
  return s1.s0.ch < s2.s0.ch;
}
bool CompTDiff(const Syncs& s1, const Syncs& s2) {
  return s1.dt < s2.dt;
}

class SyncStats {
public:
  int n0max, nch, nmax;
  // Each element refers to a channel in the board.
  // syncs are the sync pulse pairs, counts are the number of
  // times those sync pulse pairs are found in that channel (should be
  // exactly one).
  vector<Syncs> syncs;
  vector<int> counts;
  SyncStats(const vector< vector<Syncs> >& s, const vector<int>& c) {
    if (c.empty()) {
      nmax = -1;
      return;
    }
    int i   = std::max_element(c.begin(), c.end()) - c.begin();
    n0max   = c[i];
    nch     = s.size();
    nmax    = std::count(c.begin(), c.end(), n0max);
    counts.reserve(nch);
    syncs .reserve(nch);
    syncs .push_back(s[0][i]);
    counts.push_back(std::count(s[0].begin(), s[0].end(), syncs[0]));
    for (int j = 1; j < nch; ++j) {
      counts.push_back(std::count(s[j].begin(), s[j].end(), syncs[0]));
      if (counts.back())
        syncs.push_back(*std::find(s[j].begin(), s[j].end(), syncs[0]));
      else
        syncs.push_back(Syncs());
    }
    std::sort(syncs.begin(), syncs.end(), CompChannel);
  }
  int Invalid() {
    if      (nmax > 1) return 1;
    else if (nmax < 1) return -1;
    for (int i = 0; i < nch; ++i)
      if (counts[i] >  1) return  1;
    return 0;
  }
  void Print() {
    if (nmax < 0) {
      std::cout << "Invalid syncs" << std::endl;
      return;
    }
    std::cout << "n/nch: " << n0max << '/' << nch
              << "; nmax: " << nmax << "; n_ch: ";
    for (int i = 0; i < nch; ++i)
      std::cout << counts[i] << ' ';
    std::cout << std::endl;
    for (int i = 0; i < nch; ++i)
      syncs[i].Print();
  }
};

// Use compare so we can get board or board and channel.
Board BoardEnum(const string& s) {
  for (Board b = D4; b < NB; ++b)
    if (s.compare(0, b <= T4 ? 2 : 8, BOARDSTR[b]) == 0)
      return b;
  throw "Invalid board: " + s;
}

string BoardStr(Board b) {
  return BOARDSTR[b];
}

bool FirstCh1(Board b) {
  return SIS3350_B1 <= b;
}

string ChannelStr(Board b, int ch) {
  return BoardStr(b) + ((FirstCh1(b) ? '1' : '0') + ch);
}

bool IsSIS3300(Board b) {
  return b == SIS3300_B1 || b == SIS3300_B2 || b == SIS3300_B3 ||
         b == SIS3300_B4 || b == SIS3300_B5 || b == SIS3301_B6;
}

bool IsCAEN(Board b) {
  return b == D4 || b == D5 || b == D7 || b == T4;
}

int NCh(Board b) {
  if      (b == D5 || b == SIS3350_B1 || b == SIS3350_B2) return 4;
  else if (b == T4)                                       return 16;
  else if (b <  NBRD)                                     return 8;
  throw "Invalid board: NBRD";
}

int Ch(const string& c) {
  return atoi(c.substr(c.size()-2).c_str()) - FirstCh1(BoardEnum(c));
}

vector<Syncs> postulate_syncs(int ich, const vector<TAnalysedPulse*>& ch) {
  vector<Syncs> syncs;
  for (int i = 0; i < ch.size(); ++i)
    for (int j = i+1; j < ch.size(); ++j)
      if (Syncs::ValidTDiff(ch[i], ch[j]) &&
          Syncs::ValidEDiff(ch[i], ch[j]) )
        syncs.push_back(Syncs(Sync(ich, i, ch[i]), Sync(ich, j, ch[j])));
  std::sort(syncs.begin(), syncs.end(), CompTDiff);
  return syncs;
}

vector< vector<Syncs> > postulate_syncs(const vector<IDs::source>& brd) {
  vector< vector<Syncs> > syncs(brd.size());
  for (int i = 0; i < brd.size(); ++i) {
    if (brd[i] == IDs::source()) continue; // Not a processed channel
    syncs[i] = postulate_syncs(i, gAnalysedPulseMap[brd[i]]);
  }
  return syncs;
}

void print_results(const vector<Syncs> s, const vector<int>& c) {
  for (int i = 0; i < s.size(); ++i)
    if (c[i] > 0)
      std::cout << i << ' ' << " ("
                << s[i][1] << ',' << s[i][2] << ',' << s[i].dt
                << ") " << c[i] << std::endl;
}

// TODO: Function returns vector of indices for channels
// in board from shortest to longest

Syncs find_board_syncs_d4(const vector<TAnalysedPulse*> ch) {
  for (int i = 0; i < ch.size(); ++i)
    if (ch[i]->GetAmplitude() > 5000)
      return Syncs(Sync(0, i, ch[i]), Sync());
  return Syncs();
}

// Each element is the sync for the channel. Also good for SIS3301 and
// seemingly D5 for when we used it early on (wasn't used for production).
// Also D7, but we failed on that board generally
vector<Syncs> find_board_syncs_sis3300(const vector<IDs::source>& brd) {
  vector< vector<Syncs> > syncs = postulate_syncs(brd);
  vector<int> count(syncs[0].size(), 1);
  for (int jch = 1; jch < syncs.size(); ++jch)
    for (int isync = 0; isync < syncs[0].size(); ++isync)
      count[isync] += std::count(syncs[jch].begin(), syncs[jch].end(),
                                 syncs[0][isync]);
  SyncStats st(syncs, count);
  if (st.Invalid()) {
    cout << "fbs_sis3300 invalid " << brd.front().str() << endl;
    st.Print();
    return vector<Syncs>();
  }
  return st.syncs;
}


// D4: Hits at a bit over 5000 pedsub'd ADC in channel 0 only after run 9642.
// D5: Didn't really... use
// D7: All thresholds set too high and crate started too late, but otherwise
//     should have the sync pulses in all channels like any Si.
// T4: T401 for all runs.
IdentifySyncs::IdentifySyncs(modules::options* opts):
   BaseModule("IdentifySyncs", opts) {
    for (Board b = D4; b < NBRD; ++b)
      fBoardMap[b].resize(NCh(b));
}

// Just want to see whether or not compiles without this.
// IdentifySyncs::~IdentifySyncs(){
// }

int IdentifySyncs::BeforeFirstEntry(TGlobalData* gData, const TSetupData *setup){
  set<IDs::channel> dets;
  for (SourceAnalPulseMap::const_iterator i = gAnalysedPulseMap.begin();
       i != gAnalysedPulseMap.end(); ++i) {
    IDs::channel det = i->first.Channel();
    string ch  = setup->GetBankName(det.str());
    Board brd  = BoardEnum(ch);
    fBoardMap[brd][Ch(ch)] = i->first;
    if (!dets.insert(det).second) {
      cout << "ERROR: IdentifySyncs: Multiple Sources for detector " << det
           << endl;
      return 1;
    }
    fChanSyncs    [det].resize(EventNavigator::Instance()->GetInputNEntries());
    fBoardSyncTime[det].resize(EventNavigator::Instance()->GetInputNEntries());
  }
  return 0;
}

vector<string> enumerate_channels(string brd) {
  int i0     = 1;
  int nch    = 8;
  string fmt = "%1d";
  if        (brd == "T4") {
    nch == 16;
  } else if (brd == "D5" || brd == "SIS3350_B1" || brd == "SIS3350_B2") {
    nch = 4;
  }
  if        (brd == "T4" || brd == "D4" || brd == "D5" || brd == "D7") {
    i0 = 0;
    fmt = "%02d";
  }
  vector<string> chs;
  for (int i = 0; i < nch; ++i) {
    char str[32];
    sprintf(str, (brd+fmt).c_str(), i);
    chs.push_back(str);
  }
  return chs;
}

int IdentifySyncs::ProcessEntry(TGlobalData* gData, const TSetupData *setup) {
  //gErrorIgnoreLevel = kError; // TError.h
  map< Board, vector<Syncs> > block_syncs;
  for (map< Board, vector<IDs::source> >::iterator ibrd = fBoardMap.begin();
     ibrd != fBoardMap.end(); ++ibrd) {
    if  (ibrd->first == D4) {
      Syncs s = find_board_syncs_d4(gAnalysedPulseMap[ibrd->second[0]]);
      block_syncs[ibrd->first].assign(1, s);
    } else if (ibrd->first == D5 ||  ibrd->first == D7 || IsSIS3300(ibrd->first)) {
      block_syncs[ibrd->first] = find_board_syncs_sis3300(ibrd->second);
    } else if (ibrd->first == SIS3350_B1 || ibrd->first == SIS3350_B2) {
      // Won't worry about this.
      // Doesn't seem like we ever seriously considered these boards.
      continue;
    }
  }
  // TDC sees two pulses in beginning, and two at end.
  // So we take the first of these pairs.
  if (gData->fTDCHitsToChannelMap.count("T401")) {
    static const double dt = 1./40.96; // ns
    const vector<int64_t>& ts = gData->fTDCHitsToChannelMap["T401"];
    block_syncs["T4"].assign(1, Syncs(Sync(1, 0, ts[0]*dt, 0.),
                                      Sync(1, 2, ts[2]*dt, 0.)));
  }

  // Now fill SetupNavigator info
  // Use SiT-1-S board as t0 for syncs
  //string ch0 = setup->GetBankName("SiT-1-S");
  map< Board, pair<int, double> > t0s;
  for (map< Board, vector<Syncs> >::const_iterator i = block_syncs.begin();
       i != block_syncs.end(); ++i) {
    for (int j = 0; j < i->second.size(); ++j) {
      const int b = EventNavigator::Instance()->EntryNo();
      string ch = (i->first, j);  /////////////////START HERE
      fChannelSyncs[ch][b] = i->second[j].Indexes();
      board_t0[brd] += (i->second[j][0].t - board_t0[brd].second) /
                       (++board_t0[brd]->first);
    }
  }
  return 0;
}

const string IdentifySyncs::BOARDSTR[IdentifySync::NBRD] = {
  "D4",         "D5",         "D7",         "T4",
  "SIS3350_B1", "SIS3350_B2", "SIS3300_B1", "SIS3300_B2",
  "SIS3300_B3", "SIS3300_B4", "SIS3300_B5", "SIS3301_B6"
}

const int IdentifySyncs::BOARDNCH[IdentifySyncs::NBRD] = {
  4, 4,
}
// The following macro registers this module to be useable in the config file.
// The first argument is compulsory and gives the name of this module
// All subsequent arguments will be used as names for arguments given directly
// within the modules file.  See the github wiki for more.
ALCAP_REGISTER_MODULE(IdentifySyncs);
