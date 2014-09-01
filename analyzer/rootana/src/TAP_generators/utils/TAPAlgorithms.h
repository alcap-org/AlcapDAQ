#ifndef TAPAlgorithms_h_
#define TAPAlgorithms_h_

#include "TPulseIsland.h"

// We will keep all the algorthims in their own namespace
namespace Algorithm {

  // The list of algorthims we have
  struct MaxBinAmplitude;
  struct MaxBinTime;
  struct ConstantFractionTime;
  struct SimpleIntegral;
  struct IntegralRatio;
};

struct Algorithm::MaxBinAmplitude {
  public:
  MaxBinAmplitude(double ped, int trig_pol)
    :trigger_polarity(trig_pol), pedestal(ped){}
  double operator() (const TPulseIsland* tpi);

  const int trigger_polarity;
  const double pedestal;
};

struct Algorithm::MaxBinTime {
  public:
  MaxBinTime(int trig_pol, double clk_tick, double t_shift)
    :trigger_polarity(trig_pol), clock_tick_in_ns(clk_tick), time_shift(t_shift){}
  double operator() (const TPulseIsland* tpi);

  const int trigger_polarity;
  const double clock_tick_in_ns;
  const double time_shift;
};

struct Algorithm::ConstantFractionTime {
  public:
  ConstantFractionTime(double ped, int trig_pol, int max_adc, double clk_tick, double t_shift, double const_frac)
    :pedestal(ped), trigger_polarity(trig_pol), max_adc_value(max_adc), clock_tick_in_ns(clk_tick), time_shift(t_shift), constant_fraction(const_frac){}
  double operator() (const TPulseIsland* tpi);

  const double pedestal;
  const int trigger_polarity;
  const int max_adc_value;
  const double clock_tick_in_ns;
  const double time_shift;

  const double constant_fraction;
};


struct Algorithm::SimpleIntegral {
  public:
  SimpleIntegral(double ped, int trig_pol)
    :trigger_polarity(trig_pol), pedestal(ped),start(0),stop(0){}
  /// @param first Start integral from "samples.begin()+first" sample
  /// @param last End integral at "samples.begin()+last" if last>0 or
  /// "samples.end()+last" if last<=0 
  SimpleIntegral(double ped, int trig_pol, int first, int last)
    :trigger_polarity(trig_pol), pedestal(ped),start(first),stop(last){}
  double operator() (const TPulseIsland* tpi);

  const int trigger_polarity;

  double GetPedestal()const{return pedestal;};
  int GetStart()const{return start;};
  int GetStop()const{return stop;};

  void SetPedestal(double v){pedestal=v;};
  void SetStart(int v){start=v;};
  void SetStop(int v){stop=v;};

private:
  double pedestal;
  int start;
  int stop;
};

struct Algorithm::IntegralRatio{
  IntegralRatio(int begin,int tail, int end, int trig_pol,double ped=0)
    :fTailIntegrator(ped,trig_pol,tail,end)
     ,fHeadIntegrator(ped,trig_pol,begin,tail){
     }
  double operator() (const TPulseIsland* tpi);

  double GetRatio()const {return fTail/(fHead+fTail);}
  double GetTotal()const{return fHead+fTail;}
  double GetTail()const{return fTail;}
  
  void SetTailStart(int v){ fTailIntegrator.SetStart(v); fHeadIntegrator.SetStop(v);}
  void SetPedestal(double v){ fTailIntegrator.SetPedestal(v); fHeadIntegrator.SetPedestal(v);}

private:
  double fTail;
  double fHead;
  Algorithm::SimpleIntegral fTailIntegrator;
  Algorithm::SimpleIntegral fHeadIntegrator;
};

#endif
