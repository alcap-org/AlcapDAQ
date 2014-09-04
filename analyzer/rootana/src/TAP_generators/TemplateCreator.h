#ifndef TEMPLATECREATOR_H_
#define TEMPLATECREATOR_H_

#include "BaseModule.h"
class gTGlobalData;
class gTSetupData;
class gModulesOptions;

#include "TemplateArchive.h"
#include "PulseCandidateFinder.h"
#include "TemplateFitter.h"

class TemplateCreator : public BaseModule{
  struct ChannelSet{
     bool converged_status; ///< have we converged or not?
     std::string detname, bankname;
     int fit_attempts; ///< how many attempts were made to fit a pulse
     int fit_successes; ///< How many fits were successful
     int pulses_in_template; ///< How many pulses have been averaged to make the template
     TH1D *error_v_pulse_hist;
     TH1D *template_pulse;
     PulseCandidateFinder* pulse_finder;
     TemplateFitter* fitter;

     // constructor takes a lot of the effort of initialising the above fields
     ChannelSet(const std::string& detname, const std::string& bankname, modules::options* opts,int refine );
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
  TH1D* CreateRefinedPulseHistogram(const TPulseIsland* pulse, std::string histname, std::string histtitle, bool interpolate);

  /// \brief
  /// Checks if the template has converged and that adding a new pulse has not effect on the template
  /// Returns: true if converged
  bool CheckConvergence(ChannelSet& current);

  //// @brief 
  ///  Check if any part of the pulse' waveform has ADC overflow
  int HasPulseOverflowed(const TPulseIsland* pulse, const std::string& bankname);

 private:

  /// \brief
  /// The factor that we scale the number of bins in the template histogram by
  int fRefineFactor;

  TemplateFitter* fTemplateFitter;

  modules::options* fOpts; ///< Store the options for use in ProcessEntry

  TemplateArchive* fTemplateArchive;

  /// \brief
  /// If true, then print out all pulses that get added to the templates and also
  /// print out certain templates as we go along
  bool fPulseDebug;
  bool fAnalyseAllChannels;
  ChannelList fChannels;
  std::vector<std::string> fRequestedChannels;
  
};

#endif //TEMPLATECREATOR_H_
