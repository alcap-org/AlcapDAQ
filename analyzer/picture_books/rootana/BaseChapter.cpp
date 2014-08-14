#include "BaseChapter.h"

#include <iostream>

BaseChapter::BaseChapter(std::string chapter_name, modules::options* opts): fChapterName(chapter_name), fOpts(opts) {

  fModuleName = opts->GetString("module", "Not specified");
  fPlotType = opts->GetString("plot_type", "all");
  fDrawOption = opts->GetString("draw_option", "");
  fSubPlotType = opts->GetString("sub_plot_type", "all");

  fLogX = opts->GetBool("logX", false);
  fLogY = opts->GetBool("logY", false);
  fLogZ = opts->GetBool("logZ", false);

  fStatsBox = opts->GetBool("stats_box", false);

  fXLow = opts->GetInt("x_low", -999999); fXHigh = opts->GetInt("x_high", -999999);
  fYLow = opts->GetInt("y_low", -999999); fYHigh = opts->GetInt("y_high", -999999);
  fZLow = opts->GetInt("z_low", -999999); fZHigh = opts->GetInt("z_high", -999999);
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
