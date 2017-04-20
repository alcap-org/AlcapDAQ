#ifndef IDBOARD_H_
#define IDBOARD_H_

#include "IdChannel.h"
#include "TSetupData.h"

#include "Rtypes.h"
#include "TObject.h"

#include <string>
#include <vector>

/// Namespace defining various identification objects used in the
/// lists of pulses and elsewhere
namespace IDs{
  class board;
  enum Board_t {
    kD4,         kD5,         kD7,         kT4,
    kSIS3350_B1, kSIS3350_B2, kSIS3300_B1, kSIS3300_B2,
    kSIS3300_B3, kSIS3300_B4, kSIS3300_B5, kSIS3301_B6,
    kNBoard, kErrorBoard
  };
  bool operator!=(const board&, const board&);
  bool operator< (const board&, const board&);
}

class IDs::board : public TObject{
  Board_t             fBoard;
  static const std::string fgBoardStr[kNBoard];

  static Board_t Board(const IDs::channel&);

  std::string Bank(int i) const;

public:
  board(Board_t brd=kErrorBoard) : fBoard(brd)        {}
  board(const IDs::channel& brd) : fBoard(Board(brd)) {}
  virtual ~board()                                    {}
  int ChIndex(const IDs::channel&);
  board& operator++();
  bool IsSIS3300() const;
  bool IsSIS3350() const;
  bool IsCAEN()    const { return kD4 <= fBoard && fBoard <= kT4; }
  bool IsSIS()     const { return IsSIS3300()   || IsSIS3350(); }
  IDs::channel Channel(int i) const;
  Board_t Board()   const { return fBoard; }
  std::string Str() const { return fgBoardStr[fBoard]; }
  int     NCh()     const;
  friend bool operator!=(const board&, const board&);
  friend bool operator< (const board&, const board&);
  static board   Begin() { return board((Board_t)0); }
  static board   End()   { return board(); }

  ClassDef(IDs::board,1);
};

inline bool IDs::board::IsSIS3300() const {
  return fBoard == kSIS3300_B1 || fBoard == kSIS3300_B2 ||
         fBoard == kSIS3300_B3 || fBoard == kSIS3300_B4 ||
         fBoard == kSIS3300_B5 || fBoard == kSIS3301_B6;
}

inline bool IDs::board::IsSIS3350() const {
  return fBoard == kSIS3350_B1 || fBoard == kSIS3350_B2;
}

inline IDs::channel IDs::board::Channel(int i) const {
  return IDs::channel(TSetupData::Instance()->GetDetectorName(Bank(i)));
}

inline bool IDs::operator!=(const board& l, const board& r) {
  return l.fBoard != r.fBoard;
}

inline bool IDs::operator<(const board& l, const board& r) {
  return l.fBoard < r.fBoard;
}

#endif //IDBOARD_H_
