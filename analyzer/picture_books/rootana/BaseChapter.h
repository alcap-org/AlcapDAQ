#ifndef BaseChapter_h_
#define BaseChapter_h_

#include "ModulesOptions.h"

class BaseChapter {
 public:
  BaseChapter(std::string chapter_name, modules::options* opts);
  ~BaseChapter() {};
  
  std::string GetChapterName() { return fChapterName; }

 private:
  std::string fChapterName;
  modules::options* fOpts;
};

#endif
