#include "MyModule.h"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <map>
#include <utility>

#include "TH2.h"

using std::string;
using std::map;
using std::vector;
using std::pair;

const int NCHAN = 8;
static bool verbose = true;
static TH1 *hSamples1 = 0;
static TH1 *hSamples2 = 0;
static TH1 *hSamples3 = 0;
static TH2 *hBankSize = 0;
static TH1 *bankHists[NCHAN];
static vector<string> banks;

MyModule::MyModule(const char *HistogramDirectoryName) :
  FillHistBase(HistogramDirectoryName){

  // The following are the banks we want to compare in this example
  banks.push_back("CDG0");
  banks.push_back("CDG1");
  banks.push_back("CDG2");
  banks.push_back("CDG3");
  banks.push_back("CDG4");
  banks.push_back("CDG5");
  banks.push_back("CDG6");
  banks.push_back("CDG7");

  printf(" banks size %d \n",(int)banks.size());

  hSamples1 = new TH1F("hSamples1","",100,0,100);
  hSamples2 = new TH1F("hSamples2","",100,0,100);
  hSamples3 = new TH1F("hSamples3","",100,0,100);

  for (int i=0; i<NCHAN; i++)  bankHists[i] = new TH1F(Form("bank%d",i),"",100,0,100);
  
}

MyModule::~MyModule(){
}

int MyModule::ProcessEntry(TGlobalData *gData){
  typedef map<string, vector<TPulseIsland*> > StringPulseIslandMap;
  typedef pair<string, vector<TPulseIsland*> > TStringPulseIslandPair;
  typedef map<string, vector<TPulseIsland*> >::iterator map_iterator;

  //printf(" bank size %d\n ",banks.size());
  
  vector<TPulseIsland*> islands[banks.size()];

  for(unsigned int b1 = 0; b1 < banks.size(); ++b1){
    string bank = banks.at(b1);
    for(map_iterator iter = gData->fPulseIslandToChannelMap.begin(); iter != gData->fPulseIslandToChannelMap.end(); iter++){

      if(strcmp((iter->first).data(), bank.data()) == 0){
	    islands[b1] = iter->second;
        //std::cout<<" match! iter->first: "<<iter->first<<std::endl;//printf(" iter->first = %s\n",iter->first);
      }  
    }
    //printf(" b1 %d, island size %d\n",b1,islands[b1].size());
    //hBankSize->Fill(islands[b1].size(), b1+1);
  }

  int maxTPIsize = 10;

  vector<int> samples[banks.size()][maxTPIsize];

  for (int b1=0; b1<banks.size(); b1++){
    for (int i=0; i<islands[b1].size(); i++){
      samples[b1][i] = islands[b1][i]->GetSamples();
    }
  }

  //for (int j=0; j<banks.size(); j++) printf("islands %d size %d, sample size %d\n",j,islands[j].size(),samples[j][0].size());

  for(unsigned int b1 = 0; b1 < banks.size(); ++b1){

    //std::cout<<" b1 "<<b1<<" / "<<banks.size()<<std::endl;
  
    for(unsigned int i = 0; i < islands[b1].size(); ++i){
      //printf(" bank %d, island %d timestamp %d clock tick %d\n",b1,i,islands[b1][i]->GetTimeStamp(),islands[b1][i]->GetClockTickInNs());
      for (int j=0; j<samples[b1][i].size(); j++){
        bankHists[b1]->Fill(samples[b1][i][j]);
        if (i==0) hSamples1->Fill(samples[b1][i][j]);
        if (i==1) hSamples2->Fill(samples[b1][i][j]);
        if (i==2) hSamples3->Fill(samples[b1][i][j]);
        //printf(" for channel %d, island %d, adc samples #%d = %d\n",b1,i,j,samples[i][j]);
      }
    }
  }

  return 0;
}
