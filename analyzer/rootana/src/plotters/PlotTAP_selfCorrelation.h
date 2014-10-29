#ifndef PLOTTAP_SELFCORRELATION_H_
#define PLOTTAP_SELFCORRELATION_H_

#include <iostream>
#include "TH2F.h"
#include "TH1F.h"
#include "BaseModule.h"
#include "IdSource.h"
#include "definitions.h"
class TGlobalData;
class TSetupData;
namespace modules {class options;}

////////////////////////////////////////////////////////////////////////////////
/// \ingroup rootana_modules
/// \author AuthorName
///
/// \brief
/// A one line description of what your module does.
///
/// \details
/// A longer, more descriptive block of text.
/// Specifics like members and methods will be described later.
/// You can add this to other groups instead of rootana_modules or in addition
/// to rootana_modules by adding more of the ingroup tags.
////////////////////////////////////////////////////////////////////////////////
class PlotTAP_selfCorrelation : public BaseModule {

 public:
  /// \brief
  /// Constructor description. If necessary, add a details tag like above.
  ///
  /// \param[in] opts Describe the options this module takes.
  PlotTAP_selfCorrelation(modules::options* opts);
  /// \brief
  /// Is anything done in the destructor?
  ~PlotTAP_selfCorrelation();

 private:
  /// \brief
  /// What's calculated for every entry?
  /// Don't hesitate to repeat what was said in the class description.
  /// 
  /// \return Non-zero to indicate a problem.
  virtual int ProcessEntry(TGlobalData *gData, const TSetupData *gSetup);
  /// \brief
  /// What needes to be done before each run?
  /// Don't hesitate to repeat what was said in the class description.
  ///
  /// \return Non-zero to indicate a problem.
  virtual int BeforeFirstEntry(TGlobalData* gData,const TSetupData *setup);
  /// \brief
  /// What needs to be done after each run?
  /// Don't hesitate to repeat what was said in the class description.
  ///
  /// \return Non-zero to indicate a problem.
  virtual int AfterLastEntry(TGlobalData* gData,const TSetupData *setup);

private:
  /// \brief Prepare histograms.
  void BookHistograms(const TSetupData*);
  bool PassesCutA(const TAnalysedPulse* tap)const;
  bool PassesMuScCut(const TAnalysedPulse* tap, const AnalysedPulseList&)const;

  std::string fDetNameA, fDetNameB;
  double fTimeLow, fTimeHigh;
  double fMinAmpA, fMaxAmpA;
  double fMuScWindow;

  typedef std::map<std::string, std::vector<TH2F*> > HistList;
  HistList fHists;

  typedef std::vector<IDs::source> SourceList;
  SourceList fDetASources, fDetBSources;
  IDs::source fMuScSource;
};

#endif //PLOTTAP_SELFCORRELATION_H_
