/********************************************************************\

Name:         TDefna
Created by:   Volodya Tishchenko

Contents:     calculation of defna parameters of pulses

\********************************************************************/

/* Standard includes */
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <map>
#include <utility>
#include <sstream>
#include <cmath>

#include <math.h>

#include "TPulseIsland.h"
#include "TDefna.h"

void TDefna::reset()
{
  pedestal = t0 = E = A = 0;
}

bool defna_eval(TPulseIsland *island, TDefna_param &param, TDefna &res)
{

  std::vector<int> theSamples = island->GetSamples();
  unsigned int nsamples = theSamples.size();
    
  // reset defna parameters
  res.reset();

  // calculate pedestal

  int imax = param.nsamples_ped;
  if ( imax > nsamples ) imax = nsamples;
  
  for (unsigned int i=0; i<imax; i++)
    {
      res.pedestal += theSamples[i];
    }
  if ( imax > 0 ) res.pedestal /= imax;

  
  // find sample where the pulse goes above threshold 
  int i_thr_ris = 0;
  for (unsigned int i=0; i<nsamples; i++)
    {
      if ( param.polarity*(theSamples[i] - res.pedestal) > param.threshold )
	{
	  i_thr_ris = i;
	  break;
	}
    }

  if ( i_thr_ris == 0 ) 
    {
      // no pulses above threshold 
      return false;
    }

  // find sample where the pulse goes below threshold
  int i_thr_fal = 0;
  for (unsigned int i=i_thr_ris+1; i<nsamples; i++)
    {
      if ( param.polarity*(theSamples[i] - res.pedestal) < param.threshold )
	{
	  i_thr_fal = i;
	  break;
	}
    }

  if ( i_thr_fal == 0 ) 
    {
      // the pulse is longer than the waveform. DEFNA algorithm does not work
      return false;
    }

  // calculate pulse parameters
  int i_1 = i_thr_ris - param.presamples;
  if ( i_1 < 0 ) i_1 = 0;

  int i_2 = i_thr_fal + param.postsamples;
  if ( i_2 >= nsamples ) i_2 = nsamples-1;

  for (int i=i_1; i<=i_2; i++)
    {
      Double_t A = fabs( theSamples[i] - res.pedestal );
      if ( A > res.A )
	{
	  res.A = A;
	}
      res.E += A;
      res.t0 += A*i;
    }
  if ( res.E > 0 ) res.t0 /= res.E;

  return true;
}
