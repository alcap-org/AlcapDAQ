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
  /// Gets the string that indicates the sub plot type that should be used for this chapter
  std::string GetSubPlotType() { return fSubPlotType; }

  /// \brief
  /// Returns whether or not we want one of the axes scaled on the plots for this chapter
  bool GetLogX() { return fLogX; }
  bool GetLogY() { return fLogY; }
  bool GetLogZ() { return fLogZ; }

  /// \brief
  /// Returns whether or not we want a stats box
  bool GetStatsBox() { return fStatsBox; }

  /// \brief
  /// Returns whether or not these are supposed to be trend plots
  bool GetIsTrendPlot() { return fIsTrendPlot; }

  /// \brief
  /// Returns whether or not we want to zoom in on the axes
  bool GetAutoZoom() { return fAutoZoom; }

  /// \brief 
  /// Returns the axis range 
  int GetXLow() { return fXLow; } int GetXHigh() { return fXHigh; }
  int GetYLow() { return fYLow; } int GetYHigh() { return fYHigh; }
  int GetZLow() { return fZLow; } int GetZHigh() { return fZHigh; }

 private:
  std::string fChapterName;
  modules::options* fOpts;

  std::string fModuleName; // the name of the rootana module that produced this plot
  std::string fPlotType; // the string that indicates the type of plot that wants to be used for this chapter
  std::string fDrawOption; // the ROOT draw option that we will pass to TH1::Draw() in main.cpp
  std::string fSubPlotType; // a string that will be searched for in the histogram name

  /// \brief
  /// Do we want to draw one of the axes with a log scale?
  bool fLogX;
  bool fLogY;
  bool fLogZ;

  /// \brief
  /// Do we want a stats box?
  bool fStatsBox;

  /// \brief
  /// Is this a chapter of trend plots?
  bool fIsTrendPlot;

  /// \brief
  /// Do we want to zoom in on the axes?
  bool fAutoZoom;

  /// \brief
  /// The axis ranges for each axis
  int fXLow, fXHigh;
  int fYLow, fYHigh;
  int fZLow, fZHigh;
};

#endif
