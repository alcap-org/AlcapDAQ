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
  virtual int ProcessEntry(TGlobalData *gData, const TSetupData* gSetup);
  virtual int BeforeFirstEntry(TGlobalData* gData, const TSetupData* setup);
  virtual int AfterLastEntry(TGlobalData* gData, const TSetupData* setup);

  /// Store the options for use in ProcessEntry
  modules::options* fOpts;

  TemplateArchive* fTemplateArchive;

  void AddPulseToTemplate(TH1D* & hTemplate, const TPulseIsland* pulse);

  /// \brief 
  /// A map of the number of fit attempts on each detector
  std::map<std::string, int> fNFitAttempts;

  /// \brief 
  /// A map of the number of successful fits on each detector
  std::map<std::string, int> fNSuccessfulFits;

  /// \brief 
  /// A map of the number of pulses in the template
  std::map<std::string, int> fNPulsesInTemplate;

  std::map<std::string, TH1D*> fErrorVsPulseAddedHistograms;
  std::map<std::string, TH1D*> fProbVsPulseAddedHistograms;

  /// \brief
  /// Corrects a given sample value
  double CorrectSampleValue(double old_value);

  TemplateFitter* fTemplateFitter;
};

#endif //TEMPLATECREATOR_H_
