#include "TCanvas.h"
#include "TFile.h"
#include "TH1.h"
#include "TH1I.h"
#include "TSQLiteServer.h"
#include "TSQLResult.h"
#include "TSQLiteResult.h"
#include "TSQLiteRow.h"
#include "TTree.h"

#include <cstdio>
#include <cstdlib>

int method_sql(int run) {
  static TSQLiteServer db("sqlite://data/R15bRunInfo.sqlite");
  char cmd[256];
  std::sprintf(cmd, "SELECT nblock FROM runinfo WHERE run==%d", run);
  TSQLResult* res = db.Query(cmd);
  if (res->GetRowCount() == 0)
    return -1;
  TSQLRow* row = res->Next();
  if (row == nullptr)
    return -1;
  int nb = std::atof(row->GetField(0));
  delete row;
  delete res;
  return nb;
}

int method_bid(int run) {
  char fname[256];
  // std::sprintf(fname, "~/R15bTME/Al100/out%05d.root", run);
  std::sprintf(fname, "~/R15bTME/Al50/tme%05d.root", run);
  TFile f(fname, "READ");
  if (f.IsZombie())
    return -1;
  TTree* tr = (TTree*)f.Get("TMETree/TMETree");
  Int_t b;
  tr->SetBranchAddress("blockId", &b);
  tr->GetEntry(tr->GetEntries()-1);
  return b+1;
}

void tme_per_run() {
  // int RUNS[2] = { 9410, 9682 }; // Al100
  // int RUNS[2] = { 9890,  10080 }; // Al50, 1.02
  int RUNS[2] = { 10081, 10127 }; // Al50, 1.03
  TH1* hsql = new TH1I("hsql", "hsql", RUNS[1]-RUNS[0]+1, RUNS[0], RUNS[1]+1);
  TH1* hbid = new TH1I("hbid", "hbid", RUNS[1]-RUNS[0]+1, RUNS[0], RUNS[1]+1);
  for (int r = RUNS[0]; r <= RUNS[1]; ++r) {
    int nbid = method_bid(r), nsql = method_sql(r);
    if (nbid == -1 || nsql == -1)
      continue;
    hsql->Fill(r, nsql);
    hbid->Fill(r, nbid);
  }
  TH1* hdiff = (TH1*)hsql->Clone("hdiff");
  hdiff->Add(hbid, -1);
  hdiff->SetTitle("NBlock differences;Run;SQL - blockId");
  hdiff->SetStats(false);
  hdiff->Draw("HIST");
}
