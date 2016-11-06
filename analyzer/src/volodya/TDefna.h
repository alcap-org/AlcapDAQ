#ifndef TDefna_h
#define TDefna_h

struct TDefna_param {

  int    polarity;        // pulse polarity (1 - positive, -1 negative)
  int    nsamples_ped;    // number of samples for pedestal calculation
  int    threshold;       // threshold realtive to pedestal, ADC value
  int    presamples;      // number of samples before threshold at rising edge for calculation of average
  int    postsamples;     // number of samples after threshold at falling edge for calculation of average
}; 

struct TDefna {

  double pedestal;    // pedestal
  double t0;          // weighted time of pulse in ct (from 0 to nsamples)
  double E;           // pedestal-subtracted ADC sum 
  double A;           // pedestal-subtraced max ADC sample

  void   reset();

};

bool defna_eval(TPulseIsland *island, TDefna_param &param, TDefna &res);

#endif // TDefna_h defined
