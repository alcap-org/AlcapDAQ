#include "IdBoard.h"

#include "IdChannel.h"
#include "TSetupData.h"

#include "Rtypes.h"

#include <cassert>
#include <cstdlib>
#include <cstdio>
#include <string>
#include <vector>

using std::string;
using std::vector;

ClassImp(IDs::board);

IDs::Board_t IDs::board::Board(const IDs::channel& b) {
  string d = TSetupData::Instance()->GetBankName(b.str());
  for (board i = Begin(); i != End(); ++i)
    if (d.compare(0, d.size()-2, i.Str()) == 0)
      return i.Board();
  assert(false);
}

IDs::Board_t IDs::board::Board(const string& b) {
  for (board i = Begin(); i != End(); ++i)
    if (b == i.Str())
      return i.Board();
  assert(false);
}

int IDs::board::ChIndex(const IDs::channel& c) {
  assert(IsCAEN() || IsSIS());
  assert(Board(c)==fBoard);
  string bnk = TSetupData::Instance()->GetBankName(c.str());
  if (IsCAEN())
    return std::atoi(bnk.substr(bnk.size()-2).c_str());
  else
    return *--bnk.end()-'1';
}

IDs::board& IDs::board::operator++() {
  switch(fBoard) {
    case kD4:         fBoard = kD5;         break;
    case kD5:         fBoard = kD7;         break;
    case kD7:         fBoard = kT4;         break;
    case kT4:         fBoard = kSIS3350_B1; break;
    case kSIS3350_B1: fBoard = kSIS3350_B2; break;
    case kSIS3350_B2: fBoard = kSIS3300_B1; break;
    case kSIS3300_B1: fBoard = kSIS3300_B2; break;
    case kSIS3300_B2: fBoard = kSIS3300_B3; break;
    case kSIS3300_B3: fBoard = kSIS3300_B4; break;
    case kSIS3300_B4: fBoard = kSIS3300_B5; break;
    case kSIS3300_B5: fBoard = kSIS3301_B6; break;
    default:          fBoard = kErrorBoard;
  }
  return *this;
}

string IDs::board::Str() const {
  switch (fBoard) {
    case kD4:         return "D4";
    case kD5:         return "D5";
    case kD7:         return "D7";
    case kT4:         return "T4";
    case kSIS3350_B1: return "SIS3350_B1";
    case kSIS3350_B2: return "SIS3350_B2";
    case kSIS3300_B1: return "SIS3300_B1";
    case kSIS3300_B2: return "SIS3300_B2";
    case kSIS3300_B3: return "SIS3300_B3";
    case kSIS3300_B4: return "SIS3300_B4";
    case kSIS3300_B5: return "SIS3300_B5";
    case kSIS3301_B6: return "SIS3301_B6";
    default:          assert(false);
  }
}

int IDs::board::NCh() const {
  assert((Board_t)0 <= fBoard && fBoard < kNBoard);
  if      (fBoard == kD5 || IsSIS3350())                  return 4;
  else if (IsSIS3300() || fBoard == kD4 || fBoard == kD7) return 8;
  else                                                    return 16;
}

std::string IDs::board::Bank(int i) const {
  assert(0 <= i && i < NCh());
  char ch[4];
  int i0 = 1;
  string fmt = "C%d";
  if (IsCAEN()) {
    i0 = 0;
    fmt = "%02d";
  }
  sprintf(ch, fmt.c_str(), i0+i);
  return Str() + ch;
}
