#ifndef TEMPLATECREATOR_H_
#define TEMPLATECREATOR_H_

#include "BaseModule.h"
class gTGlobalData;
class gTSetupData;
class gModulesOptions;

#include "utils/TemplateArchive.h"
#include "utils/PulseCandidateFinder.h"
#include "utils/TemplateFitter.h"

class TemplateCreator : public BaseModule{

 public:
  TemplateCreator(modules::options* opts);
  ~TemplateCreator();

 private:
  virtual int ProcessEntry(TGlobalData *gData, TSetupData *gSetup);
  virtual int BeforeFirstEntry(TGlobalData* gData, TSetupData *setup);
  virtual int AfterLastEntry(TGlobalData* gData, TSetupData *setup);

  /// Store the options for use in ProcessEntry
  modules::options* fOpts;

  TemplateArchive* fTemplateArchive;

  void AddPulseToTemplate(TH1D* & hTemplate, TH1D* & hPulse, std::string bankname);

  /// \brief 
  /// A map of the number of fit attempts on each detector
  std::map<std::string, int> fNFitAttempts;

  /// \brief 
  /// A map of the number of successful fits on each detector
  std::map<std::string, int> fNSuccessfulFits;

  /// \brief 
  /// A map of the number of pulses in the template
  std::map<std::string, int> fNPulsesInTemplate;

  /// \brief
  /// Corrects a given sample value
  double CorrectSampleValue(double old_value, double template_pedestal);

  /// \brief
  /// Creates a refined histogram for a given TPulseIsland
  TH1D* CreateRefinedPulseHistogram(const TPulseIsland* pulse, std::string histname, std::string histtitle, bool interpolate);

  /// \brief
  /// The factor that we scale the number of bins in the template histogram by
  int fRefineFactor;

  TemplateFitter* fTemplateFitter;

  /// \brief
  /// If true, then print out all pulses that get added to the templates and also
  /// print out certain templates as we go along
  bool fPulseDebug;
};

#endif //TEMPLATECREATOR_H_
