#include <TROOT.h>
#include <TStyle.h>
#include <TTree.h>
#include <TCanvas.h>
#include <TH1.h>
#include <TH2.h>
#include <TFile.h>
#include <TKey.h>

#include <iostream>
#include <sstream>

void check_common_runs() {

  int nUsers = 4;//folks that ran alcapana
  int nCommonRuns = 5;// runs 320[0-4]

  //work out file names we want to compare
  TString fileName[nUsers][nCommonRuns];
 
  for (int i=0; i<nUsers; i++){
    for (int j=0; j<nCommonRuns; j++)  
      fileName[i][j] = "/gpfs/home/";
  }

  for (int j=0; j<nCommonRuns; j++){
    fileName[0][j] += "quirk_j/";
    fileName[1][j] += "grange_j/";
    fileName[2][j] += "edmonds_a/";
    fileName[3][j] += "hoai_n/";
  }

  for (int i=0; i<nUsers; i++){
    for (int j=0; j<nCommonRuns; j++){
      fileName[i][j] += Form("data/tree/tree0320%d.root",j);
      //cout<<" i is "<<i<<" j is "<<j<<" fileName "<<fileName[i][j].Data()<<endl;
    }
  }

  //open files
  TFile *f[nUsers][nCommonRuns];
  for (int i=0; i<nUsers; i++){
    for (int j=0; j<nCommonRuns; j++){
      if ((i==3) && (j==1)) continue;//run 3201 not working for Nam
      f[i][j]  = TFile::Open(fileName[i][j].Data());
    }
  }

  //get trees
  TTree *tree[nUsers][nCommonRuns]; 
  for (int i=0; i<nUsers; i++){
    for (int j=0; j<nCommonRuns; j++){
      if ((i==3) && (j==1)) continue;//run 3201 not working for Nam
      tree[i][j] = (TTree *)f[i][j]->Get("EventTree");
    }
  }

  //values to compare
  int nCompBytes[nUsers][nCommonRuns]; 
  int nUncompBytes[nUsers][nCommonRuns];
  int nEntries[nUsers][nCommonRuns]; 

  //initialize and get values
  for (int i=0; i<nUsers; i++){
    for (int j=0; j<nCommonRuns; j++){

    if ((i==3) && (j==1)) continue;//run 3201 not working for Nam

    nCompBytes[i][j] = 0;
    nCompBytes[i][j] = tree[i][j]->GetZipBytes();

    nUncompBytes[i][j] = 0;
    nUncompBytes[i][j] = tree[i][j]->GetTotBytes();

    nEntries[i][j] = 0;
    nEntries[i][j] = tree[i][j]->GetEntries();

    cout<<" i "<<i<<" j "<<j<<" comp bytes "<<nCompBytes[i][j]<<" uncomp bytes "<<nUncompBytes[i][j]<<" entries "<<nEntries[i][j]<<endl;

    }  
  }

  //check for consistency
  for (int j=0; j<nCommonRuns; j++){
    if ((nCompBytes[0][j]==nCompBytes[1][j]) && (nCompBytes[0][j]==nCompBytes[2][j]) && (nCompBytes[0][j]==nCompBytes[3][j])) 
      cout<<" \n \tall users have same compressed size for run 320"<<j<<"! "<<endl;
    if ((nUncompBytes[0][j]==nUncompBytes[1][j]) && (nUncompBytes[0][j]==nUncompBytes[2][j]) && (nUncompBytes[0][j]==nUncompBytes[3][j])) 
      cout<<" \tall users have same uncompressed size for run 320"<<j<<"! "<<endl;
    if ((nEntries[0][j]==nEntries[1][j]) && (nEntries[0][j]==nEntries[2][j]) && (nEntries[0][j]==nEntries[3][j])) 
      cout<<" \tall users have same number of entries for run 320"<<j<<"! \n"<<endl;
  }

}
