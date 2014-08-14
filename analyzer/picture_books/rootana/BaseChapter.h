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

  /// \brief
  /// Gets the draw option that we want ROOT to use
  std::string GetDrawOption() { return fDrawOption; }

  /// \brief
  /// Returns whether or not we want one of the axes scaled on the plots for this chapter
  bool GetLogX() { return fLogX; }
  bool GetLogY() { return fLogY; }
  bool GetLogZ() { return fLogZ; }

  /// \brief
  /// Returns whether or not we want a stats box
  bool GetStatsBox() { return fStatsBox; }

 private:
  std::string fChapterName;
  modules::options* fOpts;

  std::string fModuleName; // the name of the rootana module that produced this plot
  std::string fPlotType; // the string that indicates the type of plot that wants to be used for this chapter
  std::string fDrawOption; // the ROOT draw option that we will pass to TH1::Draw() in main.cpp

  /// \brief
  /// Do we want to draw one of the axes with a log scale?
  bool fLogX;
  bool fLogY;
  bool fLogZ;

  /// \brief
  /// Do we want a stats box?
  bool fStatsBox;
};

#endif
