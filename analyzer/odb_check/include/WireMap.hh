/**************************************
Struct: WireMap
Author: John R. Quirk

The WireMap contains some rough information from the ODB wiremap.
Specifically the run, banks/detectors, pedestals, polarities,
thresholds, and offsets.

CURRENTLY THRESHOLDS ARE NOT IMPLEMENTS
The reason for this is that the thresholds aren't stored in
the ODB wiremap.
**************************************/

#ifndef WIREMAP_H__
#define WIREMAP_H__

#include <vector>
#include <string>

struct WireMap {
  unsigned int n;
  std::vector<std::string> bankname;
  std::vector<std::string> detname;
  std::vector<int> pedestal;
  std::vector<int> polarity;
  std::vector<int> threshold; /*** NOT IMPLEMENTED YET ***/
  std::vector<int> offset;

  // Load the ODB values into
  void LoadFromODB(std::string odb_file, bool is_odb_file);
};

#endif
