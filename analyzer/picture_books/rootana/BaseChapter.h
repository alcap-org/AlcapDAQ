#ifndef BaseChapter_h_
#define BaseChapter_h_

#include "ModulesOptions.h"

class BaseChapter {
 public:
  BaseChapter(std::string chapter_name, modules::options* opts);
  ~BaseChapter() {};
  
  std::string GetChapterName() { return fChapterName; }

  /// \brief
  /// Gets the name of the module that should have created the plots for this chapter
  std::string GetModuleName();

  /// \brief
  /// Gets the string that indicates the plot type that should be used for this chapter
  std::string GetPlotType() { return fPlotType; }

 private:
  std::string fChapterName;
  modules::options* fOpts;

  std::string fModuleName; // the name of the rootana module that produced this plot
  std::string fPlotType; // the string that indicates the type of plot that wants to be used for this chapter
};

#endif
