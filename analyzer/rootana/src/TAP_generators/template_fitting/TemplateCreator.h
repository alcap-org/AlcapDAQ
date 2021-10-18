#ifndef TEMPLATECREATOR_H_
#define TEMPLATECREATOR_H_

#include "BaseModule.h"
class gTGlobalData;
class gTSetupData;
class gModulesOptions;
class TH1D;

#include "TAPAlgorithms.h"
#include "TemplateArchive.h"
#include "PulseCandidateFinder_TSpectrum.h"
#include "TemplateMultiFitter.h"
#include "TTemplate.h"
#include "InterpolatePulse.h"

class TemplateCreator : public BaseModule{
  struct ChannelSet{
     std::string detname, bankname;

     int fit_successes, fit_attempts;
     int trigger_polarity;
     TTemplate* template_pulse;

     TemplateMultiFitter* fitter;
     PulseCandidateFinder_TSpectrum* pulse_finder;
     Algorithm::IntegralRatio *integralRatio;

     // constructor takes a lot of the effort of initialising the above fields
     ChannelSet(const std::string& detname, const std::string& bankname, modules::options* opts,int refine );
     void Clear();
  };
  typedef std::vector<ChannelSet> ChannelList;

 public:
  TemplateCreator(modules::options* opts);
  ~TemplateCreator();

 private:
  virtual int ProcessEntry(TGlobalData *gData, const TSetupData* gSetup);
  virtual int BeforeFirstEntry(TGlobalData* gData, const TSetupData* setup);
  virtual int AfterLastEntry(TGlobalData* gData, const TSetupData* setup);

  void AddPulseToTemplate(ChannelSet& current, TH1D* & hPulse);

  /// \brief
  /// Corrects a given sample value
  double CorrectSampleValue(double old_value, double template_pedestal);

  /// \brief
  /// Creates a refined histogram for a given TPulseIsland
  TH1D* CreateRefinedPulseHistogram(const TPulseIsland* pulse, std::string histname, std::string histtitle, bool interpolate){
     return InterpolatePulse(pulse,histname,histtitle, interpolate, fRefineFactor);
  }

  /// \brief
  /// Checks if the template has converged and that adding a new pulse has not effect on the template
  /// Returns: true if converged
  bool CheckConvergence(ChannelSet& current);

  //// @brief 
  ///  Check if any part of the pulse' waveform has ADC overflow
  int HasPulseOverflowed(const TPulseIsland* pulse, const std::string& bankname);

  /// @brief Init the template using the first valid pulse
  TH1D* StartTemplate(int pulseID, const TPulseIsland* pulse,const std::string& detname);

 
 private:

  /// \brief
  /// The factor that we scale the number of bins in the template histogram by
  int fRefineFactor;

  TemplateMultiFitter* fTemplateFitter;

  modules::options* fOpts; ///< Store the options for use in ProcessEntry

  std::string fArchiveName;
  TemplateArchive* fTemplateArchive;

  /// \brief
  /// If true, then print out all pulses that get added to the templates and also
  /// print out certain templates as we go along
  bool fPulseDebug;
  bool fAnalyseAllChannels;
  ChannelList fChannels;
  std::vector<std::string> fRequestedChannels;
  bool fSinglePulse;
  
  bool fCutIntegralRatio;
  double fIntegralMax, fIntegralMin;
  double fIntegralRatioMax, fIntegralRatioMin;
  int fStartIntegral, fStopIntegral, fStartTail;
  TH1F* fIntegralRatioHist;
};

#endif //TEMPLATECREATOR_H_
