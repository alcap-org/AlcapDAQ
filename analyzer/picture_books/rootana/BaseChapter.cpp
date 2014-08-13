#include "BaseChapter.h"

#include <iostream>

BaseChapter::BaseChapter(std::string chapter_name, modules::options* opts): fChapterName(chapter_name), fOpts(opts) {

  fModuleName = opts->GetString("module", "Not specified");
}

std::string BaseChapter::GetModuleName() {
  if (fModuleName != "Not specified") {
    return fModuleName;
  }
  else {
    std::cout << "The module name for chapter " << fChapterName << " was not specified (use module=XXXX)" << std::endl;
    return "0";
  }
}
