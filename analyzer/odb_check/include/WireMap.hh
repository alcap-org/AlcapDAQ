#include <vector>
#include <string>

struct WireMap {
  unsigned int n;
  std::vector<std::string> bankname;
  std::vector<std::string> detname;
  std::vector<int> pedestal;
  std::vector<int> polarity;
  std::vector<int> threshold;
};
