#ifndef CreateTemplates_h__
#define CreateTemplates_h__

#include "FillHistBase.h"
#include "TGlobalData.h"

#include "TH2.h"

class CreateTemplates : public FillHistBase{
 public:
  CreateTemplates(char *HistogramDirectoryName);
  ~CreateTemplates();
  
  // For both fast and slow pulses
  TH1F* PlotPulse(TPulseIsland* pulse, int pulse_number, double& a_max, double& t_max);
  void Fill2DTemplateHistogram(TH1F* hPulse, TH2F* h2DTemplate, double a_max, double t_max, double A, double delta_t, double anchor_time, double anchor_ADC);
  void Plot1DTemplateHistogram(TH2F* h2DTemplate, TH1F* h1DTemplate);
  
  // For slow pulses
  TF1* FitGaussian(TH1F* hPulse); // for slow pulses
  
  // For fast pulses
  void FillPseudotimeDistributions(TH1F* hPulse, TH1F* hPseudotime, TH2F* hAMaxVsPsi);
  double CalculatePseudotimeConstant(TH1F* hPseudotime, double clock_tick);
  void PlotTauDistribution(TH1F* hTau, TH1F* hPseudotime, double constant);
  
  void PlotAverageAMaxDistribution(TH2F* hAMaxDistribution, TH1F* hAverageAMaxDistribution);
  double CalculateRConstant(TH1F* hAvgAMaxVsPsi);
  
  void GetFastPulsePseudotimeVariables(TH1F* hFastPulse, double& a_max, double& t_max, double& A, double& delta_t, double clock_tick, int iBank, double constant, double k);

 private:
  virtual int ProcessEntry(TGlobalData *gData);
};

#endif

