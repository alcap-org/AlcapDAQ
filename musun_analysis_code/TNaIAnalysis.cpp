#include "TNaIAnalysis.h"

#include "TOctalFADCBankReader.h"
#include "TOctalFADCIsland.h"
#include "TMucapData.h"
#include "midas.h"
#include "TNaIHit.h"
#include "TTimeWindow.h"

#include <vector>
#include <algorithm>

using std::vector;

/******* global variables **********/
extern TMucapData *gData;
extern OCTALFADC_PARAM octalfadc_parameters;
/******* end global variables **********/

TNaIAnalysis::TNaIAnalysis()
{
  hXRayAmplitude = new TH2F("hXRayAmplitude","X ray det pulse amplitude vs channel", 5000,0,5000, 4,0.5,4.5 );
  hXRayAmplitude->GetXaxis()->SetTitle("Amplitude (channels)");

  hXRayAmplitudeMissed = new TH2F("hXRayAmplitudeMissed","X ray det rejected pulse amplitudes vs channel", 5000,0,5000, 4,0.5,4.5 );
  hXRayAmplitudeMissed->GetXaxis()->SetTitle("Amplitude (channels)");

//  hXRayAvAmplitude = new TH2F("hXRayAvAmplitude","X ray det pulse averaged amplitude vs channel", 5000,0,5000, 4,0.5,4.5 );
//  hXRayAvAmplitude->GetXaxis()->SetTitle("Amplitude (channels)");

//  hXRayFitAmplitude = new TH2F("hXRayFitAmplitude","X ray det pulse fit amplitude vs channel", 5000,0,5000, 4,0.5,4.5 );
//  hXRayFitAmplitude->GetXaxis()->SetTitle("Amplitude (channels)");


  hXRayTime = new TH2F("hXRayTime","X ray  pulse trigger time vs channel", 40,0,4e8, 4,0.5,4.5);
  hXRayTime->GetXaxis()->SetTitle("Block time (ns)");
  hXRayTimingEnergyTDiffVsAmplitude = new TH3F("hXRayTimingEnergyTDiffVsAmplitude", "XRay T diff energy channel and timing channel versus amplitude", 200,-8000,8000,100,0,5000,2,0.5,2.5);
  hXRayTimingEnergyTDiffVsAmplitude->GetXaxis()->SetTitle("Time difference (ns)");
  hXRayTimingEnergyTDiffVsAmplitude->GetZaxis()->SetTitle("Detector Nr.");
  hXRayTimingEnergyTDiffVsAmplitude->GetYaxis()->SetTitle("Amplitude");
//  hNaITimingAmpVsEnergyAmp = new TH2F("hNaITimingAmpVsEnergyAmp", "NaI Hits: Amplitude of timing channel vs energy channel",4096,0,4096,4096,0,4096);

//  hNaIAmplitudeVsSizeIslandAccept = new TH3F("hNaIAmplitudeVsSizeIslandAccept","Accepted NaI pulse amplitude vs Size Island", 500,0,5000,100,0,1000,2,0.5,2.5 );
//  hNaIAmplitudeVsSizeIslandReject = new TH3F("hNaIAmplitudeVsSizeIslandReject","Rejected NaI pulse amplitude vs Size Island", 500,0,5000,100,0,1000,2,0.5,2.5 );

//  hNaIAverageVsAmpAccept = new TH3F("hNaIAverageVsAmpAccept","Accepted NaI pulse amplitude Vs Average Island / Amp", 310,1000,4100,440,0,1.1,2,0.5,2.5 );
//  hNaIAverageVsAmpReject = new TH3F("hNaIAverageVsAmpReject","Rejected NaI pulse  amplitude Vs Average Island / Amp", 310,1000,4100,440,0,1.1,2,0.5,2.5 );

//  hNaIPileUp= new TH1I("hNaIPileUp","NaI Pile Ups, no pile up = 1, pile up = 2", 2,0.5,2.5);
//  hNaICount = new TH1I("hNaICount","Ambiguous or missed hits, ok = 1, ambiguous time tagging = 2, missed E signal = 3", 3,0.5,3.5 );

  hXRayStats = new TH1I("hXRayStats","X ray stats", 20,0.5,20.5);
  hXRayStats->GetXaxis()->SetBinLabel(1, "NaI-fast islands");
  hXRayStats->GetXaxis()->SetBinLabel(2, "NaI-slow islands");
  hXRayStats->GetXaxis()->SetBinLabel(3, "CsI-fast islands");
  hXRayStats->GetXaxis()->SetBinLabel(4, "CsI-slow islands");
  hXRayStats->GetXaxis()->SetBinLabel(5, "NaI-fast bad islands");
  hXRayStats->GetXaxis()->SetBinLabel(6, "NaI-slow bad islands");
  hXRayStats->GetXaxis()->SetBinLabel(7, "CsI-fast bad islands");
  hXRayStats->GetXaxis()->SetBinLabel(8, "CsI-slow bad islands");\
  hXRayStats->GetXaxis()->SetBinLabel(9, "NaI hits");
  hXRayStats->GetXaxis()->SetBinLabel(10, "CsI hits");
  hXRayStats->GetXaxis()->SetBinLabel(11, "NaI Pile ups");
  hXRayStats->GetXaxis()->SetBinLabel(12, "CsI Pile ups");
  hXRayStats->GetXaxis()->SetBinLabel(13, "NaI double counts");
  hXRayStats->GetXaxis()->SetBinLabel(14, "CsI double counts");


  levelGoodPulse = 3;
  nAverage = 4;
  tLow = +4000.;//coincidence window for making hits
  tHigh = +5600.;

  //get runnr
  fRunInfoEvent = gData->runinfoEvent;
  runnr = fRunInfoEvent->GetRunNumber();


  kNaITimingChannel = OCTALFADC_PARAM::kNaITimingChannel;
  kNaIEnergyChannel = OCTALFADC_PARAM::kNaIEnergyChannel;

  kCsITimingChannel = OCTALFADC_PARAM::kCsITimingChannel;
  kCsIEnergyChannel = OCTALFADC_PARAM::kCsIEnergyChannel;

  //Get the shaping times, clock periods and pedestals
  shapNaIEnergyChannel = OCTALFADC_PARAM::shapNaIEnergyChannel;
  shapNaITimingChannel = OCTALFADC_PARAM::shapNaITimingChannel;
  
  shapCsIEnergyChannel = OCTALFADC_PARAM::shapCsIEnergyChannel;
  shapCsITimingChannel = OCTALFADC_PARAM::shapCsITimingChannel;
  
  timingNaIClockPeriod = octalfadc_parameters.kClockPeriod[kNaITimingChannel];
  energyNaIClockPeriod = octalfadc_parameters.kClockPeriod[kNaIEnergyChannel];

  timingCsIClockPeriod = octalfadc_parameters.kClockPeriod[kCsITimingChannel];
  energyCsIClockPeriod = octalfadc_parameters.kClockPeriod[kCsIEnergyChannel];

  pedestalNaIEnergyChannel = OCTALFADC_PARAM::pedestalNaITimingChannel;
  pedestalNaITimingChannel = OCTALFADC_PARAM::pedestalNaITimingChannel;
  
  pedestalCsIEnergyChannel = OCTALFADC_PARAM::pedestalCsIEnergyChannel;
  pedestalCsITimingChannel = OCTALFADC_PARAM::pedestalCsITimingChannel;
}

TNaIAnalysis::~TNaIAnalysis()
{
  delete hXRayAmplitude;
  delete hXRayAmplitudeMissed;
//  delete hXRayAvAmplitude;
//  delete hXRayFitAmplitude;
  delete hXRayTime;
  delete hXRayStats;
//  delete hNaIAvAmplitude;
//  delete hNaIAmplitudePP;
//  delete hNaIAmplitudeMissed;
//  delete hNaITime;
  delete hXRayTimingEnergyTDiffVsAmplitude;
//  delete hNaITimingAmpVsEnergyAmp;

//  delete hNaIAmplitudeVsSizeIslandAccept;
//  delete hNaIAmplitudeVsSizeIslandReject;

//  delete hNaIAverageVsAmpAccept;
//  delete hNaIAverageVsAmpReject;

//  delete hNaIPileUp;
//  delete hNaICount;

}

INT TNaIAnalysis::ProcessEvent(EVENT_HEADER *pheader, void *pevent)
{
  printf("Running TNaIAnalysis (NaI + CsI run 5)...\n");

  if(gData->fGeFADCBanks.size() < 8)
  {
    printf("TNaIAnalysis::ProcessEvent : Could not find NaI Detector banks.\n");
    return SUCCESS;
  }


  // Get the banks
  TOctalFADCBankReader& bNaI_timing = gData->fGeFADCBanks.at(kNaITimingChannel);
  TOctalFADCBankReader& bNaI_energy = gData->fGeFADCBanks.at(kNaIEnergyChannel);

  TOctalFADCBankReader& bCsI_energy = gData->fGeFADCBanks.at(kCsIEnergyChannel);
  TOctalFADCBankReader& bCsI_timing = gData->fGeFADCBanks.at(kCsITimingChannel);

  // Get the vector of islands
  vector<TOctalFADCIsland>& vNaI_timing = bNaI_timing.GetIslandVector();
  vector<TOctalFADCIsland>& vNaI_energy = bNaI_energy.GetIslandVector();

  vector<TOctalFADCIsland>& vCsI_timing = bCsI_timing.GetIslandVector();
  vector<TOctalFADCIsland>& vCsI_energy = bCsI_energy.GetIslandVector();

//std::cout << "timingNaIClockPeriod " << timingNaIClockPeriod << "timingCsIClockPeriod " << timingCsIClockPeriod << std::endl;
//std::cout << "kNaITimingChannel " << kNaITimingChannel << " kNaIEnergyChannel " << kNaIEnergyChannel << std::endl;

 
 //loop all channels and fill hAmplitude and time and set pulse quality flags
  for(int i=0; i < vNaI_timing.size(); i++)
  {
    hXRayStats->Fill(1);
    if(vNaI_timing[i].GoodPulse(shapNaITimingChannel,timingNaIClockPeriod,levelGoodPulse))
    {
      hXRayAmplitude->Fill(vNaI_timing[i].GetMax(), kNaITimingChannel+1 );
      hXRayTime->Fill(vNaI_timing[i].GetTimeNs(),kNaITimingChannel+1 );
    }
    else{hXRayStats->Fill(5);hXRayAmplitudeMissed->Fill(vNaI_timing[i].GetMax(), kNaITimingChannel+1 );}
  }


  for(int i=0; i < vNaI_energy.size(); i++)
  {
    hXRayStats->Fill(2);
    if(vNaI_energy[i].GoodPulse(shapNaIEnergyChannel,energyNaIClockPeriod,levelGoodPulse))
    {
      hXRayAmplitude->Fill(vNaI_energy[i].GetMax(), kNaIEnergyChannel+1 );
     // hXRayAvAmplitude->Fill( vNaI_energy[i].GetAverageMax(nAverage), kNaIEnergyChannel+1 );
      //if(vNaI_energy[i].FitGauss(10, 10, shapNaIEnergyChannel, energyNaIClockPeriod, pedestalNaIEnergyChannel)) { hXRayFitAmplitude->Fill( vNaI_energy[i].GetMaxGauss(), kNaIEnergyChannel+1 ); }
      hXRayTime->Fill(vNaI_energy[i].GetTimeNs(),kNaIEnergyChannel+1 );
    }
    else{hXRayStats->Fill(6);hXRayAmplitudeMissed->Fill(vNaI_energy[i].GetMax(), kNaIEnergyChannel+1 );} 
  }

 
  for(int i=0; i < vCsI_timing.size(); i++)
  {
    hXRayStats->Fill(3);
    if(vCsI_timing[i].GoodPulse(shapCsITimingChannel,timingCsIClockPeriod,levelGoodPulse))
    {
      hXRayAmplitude->Fill(vCsI_timing[i].GetMax(), kCsITimingChannel+1 );
      hXRayTime->Fill(vCsI_timing[i].GetTimeNs(),kCsITimingChannel+1 );
    }
    else{hXRayStats->Fill(7);hXRayAmplitudeMissed->Fill(vCsI_timing[i].GetMax(), kCsITimingChannel+1 );} 
  }
 

  for(int i=0; i < vCsI_energy.size(); i++)
  {
    hXRayStats->Fill(4);
    if(vCsI_energy[i].GoodPulse(shapCsIEnergyChannel,energyCsIClockPeriod,levelGoodPulse))
    {
      hXRayAmplitude->Fill(vCsI_energy[i].GetMax(), kCsIEnergyChannel+1 );
     // hXRayAvAmplitude->Fill(nAverage, vCsI_energy[i].GetAverageMax(nAverage), kCsIEnergyChannel+1 );
     // if(vCsI_energy[i].FitGauss(10, 10, shapCsIEnergyChannel, energyCsIClockPeriod, pedestalCsIEnergyChannel)) { hXRayFitAmplitude->Fill( vCsI_energy[i].GetMaxGauss(), kCsIEnergyChannel+1 ); }
      hXRayTime->Fill(vCsI_energy[i].GetTimeNs(),kCsIEnergyChannel+1 );
    }
    else {hXRayStats->Fill(8);hXRayAmplitudeMissed->Fill(vCsI_energy[i].GetMax(), kCsIEnergyChannel+1 );}
  }
 

  gData->fNaIHits.clear();
  MakeHits(vNaI_timing, vNaI_energy);
  MakeHits(vCsI_timing, vCsI_energy);
  PileUpProtectHits();
  SortHits();//After PP algorithm!

  return SUCCESS;

}

void TNaIAnalysis::MakeHits(vector<TOctalFADCIsland>& vec_timing,vector<TOctalFADCIsland>& vec_energy)
{
  vector<TNaIHit>& vHits = gData->fNaIHits;
  
  //get channel
  if(vec_timing.empty() || vec_energy.empty()) return;
  int channel = vec_timing[0].GetFADCChannel();
  //printf("Making a hits for channel %d and channel %d\n",vec_timing[0].GetFADCChannel(),vec_energy[0].GetFADCChannel());

  //set (channel specific) parameters
  double timingClockPeriod, energyClockPeriod;

  if(channel == kNaITimingChannel)
  {
    timingClockPeriod = timingNaIClockPeriod;
    energyClockPeriod = energyNaIClockPeriod;
  }
  else if(channel == kCsITimingChannel)
  {
    timingClockPeriod = timingCsIClockPeriod;
    energyClockPeriod = energyCsIClockPeriod;
  }
  else {printf("Invalid parameters to construct NaIHits!!!\n");};


  int lastEnergyPulse=0; // stays just behind the time of the timing pulse
  int count = 0;
  int energyHit = -1;

  //make coincidences and construct hits
  for(int iTime=0; iTime < vec_timing.size(); iTime++) 
  {
    if(vec_timing[iTime].GetPulseQuality() == 0)
    {
      double timingT = vec_timing[iTime].GetCFBlockTime()*timingClockPeriod;
      TTimeWindow coincWindow(timingT + tLow, timingT + tHigh);

      for(int iEgy=lastEnergyPulse; iEgy<vec_energy.size(); iEgy++)
      {
        if(vec_energy[iEgy].GetPulseQuality() == 0)
        {
          double energyT = vec_energy[iEgy].GetMaxBinBlockTime()*energyClockPeriod;

          if(coincWindow.IsLow(energyT))  { lastEnergyPulse = iEgy; }
          else if(coincWindow.Includes(energyT))
          {
            count++;
            energyHit = iEgy;
            if(channel == kNaITimingChannel) { hXRayTimingEnergyTDiffVsAmplitude->Fill(energyT-timingT,vec_energy[iEgy].GetMax(),1.); } //printf("NaI tDiff = %f\n",energyT-timingT); }
              else { hXRayTimingEnergyTDiffVsAmplitude->Fill(energyT-timingT,vec_energy[iEgy].GetMax(),2.); } //printf("CsI tDiff = %f\n",energyT-timingT);}
          }
          else if(coincWindow.IsHigh(energyT)) break;
        }
      }

      //the idea is that only one (slow) E signal lines up with a fast signal, otherwise there is a problem (and the hit is flagged with ncount > 1
      if(count>0)
      {
        if( channel == kNaITimingChannel) { hXRayStats->Fill(9); } else { hXRayStats->Fill(10); }
        TNaIHit naihit(vec_timing[iTime],vec_energy[energyHit],count);
        vHits.push_back(naihit);
      }
      count=0;
    }
  }


}


void TNaIAnalysis::PileUpProtectHits()
{
  vector<TNaIHit>& vHits = gData->fNaIHits;
  //printf("Loop through %d Xray hist for PP\n",vHits.size());
  if(vHits.size() == 0 || vHits.size() == 1) return;

  //Hits are stacked first for one detector, than for the other, so a single loop is sufficient
  for(int i = 1; i < vHits.size()-1; i++)
  {
    //printf("%d Xray hit : time = %f, energy = %f, detector = %d P\n",i, vHits[i].GetTime(),vHits[i].GetEnergy(),vHits[i].GetDetector() );
    //the actual pile up algorithm
    if(vHits[i-1].GetEnergyTime()==vHits[i].GetEnergyTime())
    {
      vHits[i-1].SetPileUp(true);
      vHits[i].SetPileUp(true);
    }
    //Filling histograms
    if(vHits[i-1].GetPileUp()) { hXRayStats->Fill(vHits[i-1].GetDetector()+10); }
    if(vHits[i-1].GetCount() > 1) {hXRayStats->Fill(vHits[i-1].GetDetector()+12);}
  }

    //this "strange second step is to avoid a second for loop over all hits. don`t know if it makes much of a difference though
  if(vHits[vHits.size()-1].GetPileUp()){ hXRayStats->Fill(vHits[vHits.size()-1].GetDetector()+10); }
  if(vHits[vHits.size()-1].GetCount() > 1){ hXRayStats->Fill(vHits[vHits.size()-1].GetDetector()+12); }
}

void TNaIAnalysis::SortHits()
{
  vector<TNaIHit>& vHits = gData->fNaIHits;
  std::sort(vHits.begin(), vHits.end(),TNaIHit::TimeSortNaIHits());
}

/*void TNaIAnalysis::MakeNaIHits(vector<TOctalFADCIsland>& vec_timing,vector<TOctalFADCIsland>& vec_energy)
{
  vector<TNaIHit>& vHits = gData->fNaIHits;
  
  const double timingClockPeriod = octalfadc_parameters.kClockPeriod[OCTALFADC_PARAM::kNaITimingChannel];
  const double energyClockPeriod = octalfadc_parameters.kClockPeriod[OCTALFADC_PARAM::kNaIEnergyChannel];

  const double shapNaITimingChannel = octalfadc_parameters.shapNaITimingChannel;
  const double shapNaIEnergyChannel = octalfadc_parameters.shapNaIEnergyChannel;
  
  int lastEnergyPulse=0; // stays just behind the time of the timing pulse
  int count = 0;
  int energyHit = -1;


  for(int iTime=0; iTime<vec_timing.size(); iTime++) 
  {
    if(vec_timing[iTime].GoodPulse(shapNaITimingChannel,timingClockPeriod,levelGoodPulse))
    {
      double timingT = vec_timing[iTime].GetTime()*timingClockPeriod;
      TTimeWindow coincWindow(timingT + 3000.,timingT +7000.);

      for(int iEgy=lastEnergyPulse; iEgy<vec_energy.size(); iEgy++)
      {
        if(vec_energy[iEgy].GoodPulse(shapNaIEnergyChannel,energyClockPeriod,levelGoodPulse))
        {
          double energyT = vec_energy[iEgy].GetMaxBinBlockTime()*energyClockPeriod;

          if(coincWindow.IsLow(energyT))
          {
            lastEnergyPulse = iEgy;
            continue;
          }
          else if(coincWindow.Includes(energyT))
          {
            count++;
            energyHit = iEgy;
            hNaITimingEnergyTDiff->Fill(energyT-timingT);
            continue;
          }
          else if(coincWindow.IsHigh(energyT)) break;
        }
      }

      //if(vec_timing[iTime].GetNSamples()<10000 && vec_energy[energyHit].GetNSamples()<10000){TNaIHit naihit(vec_timing[iTime],vec_energy[energyHit],count,nAverage);vHits.push_back(naihit);}
      if(count>0)
      {
        TNaIHit naihit(vec_timing[iTime],vec_energy[energyHit],count,nAverage);
        vHits.push_back(naihit);
      }
      count=0;
    }
  }

}*/

/*void TNaIAnailysis::PileUpProtectHits()
{
  vector<TNaIHit>& vHits = gData->fNaIHits;

  if(vHits.size() == 0) return;
  if(vHits.size() == 1) return;

  for(int i =1; i < vHits.size()-1; i++)
  {
    //the actual pile up algorithm
    if(vHits[i-1].GetEnergyTime()==vHits[i].GetEnergyTime())
    {
      vHits[i-1].SetPileUp(true);
      vHits[i].SetPileUp(true);
    }

    //Filling histograms
    if(!(vHits[i-1].GetPileUp()))
    {
      if(vHits[i-1].GetCount()==1){ hNaIAmplitudePP->Fill(vHits[i-1].GetEnergyAmp(),1); hNaIAmplitudePP->Fill(vHits[i-1].GetTimingAmp(),2); }
      hNaIPileUp->Fill(1);
    }else{hNaIPileUp->Fill(2);}

    if(vHits[i-1].GetCount() == 1)
    {
      hNaICount->Fill(1 );
    }
    else if(vHits[i-1].GetCount() > 2){hNaICount->Fill(2);}
    else {hNaICount->Fill(3);}
  }

    //this "strange second step is to avoid a second for loop over all hits. don`t know if it makes much of a difference though

  if(!(vHits[vHits.size()-1].GetPileUp()))
  {
    if(vHits[vHits.size()-1].GetCount()==1){hNaIAmplitudePP->Fill(vHits[vHits.size()].GetEnergyAmp(),1);}
    hNaIPileUp->Fill(1);
  }else{hNaIPileUp->Fill(2);}

  if(vHits[vHits.size()-1].GetCount() == 1)
  {
    hNaICount->Fill(1, 1 );
  }
  else if(vHits[vHits.size()-1].GetCount() > 2){hNaICount->Fill(2 );}
  else {hNaICount->Fill(3 );}


}
*/
