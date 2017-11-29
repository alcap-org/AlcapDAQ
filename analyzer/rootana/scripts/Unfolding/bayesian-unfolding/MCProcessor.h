//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Wed Nov 29 16:48:55 2017 by ROOT version 5.34/34
//////////////////////////////////////////////////////////

#ifndef MCProcessor_h
#define MCProcessor_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>
#include <TSelector.h>
#include <TTree.h>
#include <TH2.h>
#include <TH1.h>

// Header file for the classes stored in the TTree if any.
#include <vector>
#include <string>

// Fixed size dimensions of array or collections stored in the TTree if any.

class MCProcessor : public TSelector {
public :
   TFile *fOutputFile;
   bool r_thick_stop;
   bool r_thick_stop_2;
   bool l_thick_stop;
   Int_t r_N_thin, r_N_thick, r_N_thick2;
   Int_t l_N_thin, l_N_thick;
   double M;
   TString particle;
   TString inputAbsolutePath;

   Double_t truth_E, SiR1_E, SiR2_E, SiR3_E, SiL1_E, SiL3_E;
   TTree *tUnfolding;
   TH2D *hEvdELeft;
   TH2D *hEvdEStoppedLeft;
   TH2D *hEvdEPunchLeft;
   TH2D *hTransferLeft;
   TH2D *hTransferPTLeft;
   TH1D *hInputLeft;
   TH1D *hInputPTLeft;
   TH1D *hAcceptanceLeft;
   TH1D *hEvdELeft_px;
   TH2D *hEvdERight;
   TH2D *hEvdEStoppedRight;
   TH2D *hEvdE2StoppedRight;
   TH2D *hEvdEPunchRight;
   TH2D *hTransferRight;
   TH2D *hTransferPTRight;
   TH1D *hInputRight;
   TH1D *hInputPTRight;
   TH1D *hAcceptanceRight;
   TH1D *hDepth;
   TH1D *hEvdERight_px;

   TTree          *fChain;   //!pointer to the analyzed TTree or TChain

   // Declaration of leaf types
   Int_t           M_nHits;
   vector<double>  *M_Ox;
   vector<double>  *M_local_Ox;
   vector<double>  *M_Oy;
   vector<double>  *M_local_Oy;
   vector<double>  *M_Oz;
   vector<double>  *M_local_Oz;
   vector<double>  *M_Ot;
   vector<double>  *M_x;
   vector<double>  *M_local_x;
   vector<double>  *M_y;
   vector<double>  *M_local_y;
   vector<double>  *M_z;
   vector<double>  *M_local_z;
   vector<double>  *M_t;
   vector<double>  *M_Opx;
   vector<double>  *M_Opy;
   vector<double>  *M_Opz;
   vector<double>  *M_px;
   vector<double>  *M_py;
   vector<double>  *M_pz;
   vector<double>  *M_ox;
   vector<double>  *M_local_ox;
   vector<double>  *M_oy;
   vector<double>  *M_local_oy;
   vector<double>  *M_oz;
   vector<double>  *M_local_oz;
   vector<double>  *M_ot;
   vector<double>  *M_opx;
   vector<double>  *M_opy;
   vector<double>  *M_opz;
   vector<double>  *M_ekin;
   vector<double>  *M_e;
   vector<double>  *M_edep;
   vector<double>  *M_edepAll;
   vector<double>  *M_stepL;
   vector<int>     *M_volID;
   vector<string>  *M_volName;
   vector<int>     *M_ppid;
   vector<int>     *M_ptid;
   vector<string>  *M_oprocess;
   vector<string>  *M_ovolName;
   vector<int>     *M_tid;
   vector<int>     *M_pid;
   vector<string>  *M_particleName;
   vector<int>     *M_charge;
   vector<int>     *M_stopped;
   vector<double>  *M_stop_time;
   Int_t           run_num;
   Int_t           evt_num;
   Int_t           R0;
   Int_t           R1;
   Double_t        i_px;
   Double_t        i_py;
   Double_t        i_pz;
   Double_t        i_x;
   Double_t        i_y;
   Double_t        i_z;
   Double_t        weight;
   string          *i_particleName;

   // List of branches
   TBranch        *b_M_nHits;   //!
   TBranch        *b_M_Ox;   //!
   TBranch        *b_M_local_Ox;   //!
   TBranch        *b_M_Oy;   //!
   TBranch        *b_M_local_Oy;   //!
   TBranch        *b_M_Oz;   //!
   TBranch        *b_M_local_Oz;   //!
   TBranch        *b_M_Ot;   //!
   TBranch        *b_M_x;   //!
   TBranch        *b_M_local_x;   //!
   TBranch        *b_M_y;   //!
   TBranch        *b_M_local_y;   //!
   TBranch        *b_M_z;   //!
   TBranch        *b_M_local_z;   //!
   TBranch        *b_M_t;   //!
   TBranch        *b_M_Opx;   //!
   TBranch        *b_M_Opy;   //!
   TBranch        *b_M_Opz;   //!
   TBranch        *b_M_px;   //!
   TBranch        *b_M_py;   //!
   TBranch        *b_M_pz;   //!
   TBranch        *b_M_ox;   //!
   TBranch        *b_M_local_ox;   //!
   TBranch        *b_M_oy;   //!
   TBranch        *b_M_local_oy;   //!
   TBranch        *b_M_oz;   //!
   TBranch        *b_M_local_oz;   //!
   TBranch        *b_M_ot;   //!
   TBranch        *b_M_opx;   //!
   TBranch        *b_M_opy;   //!
   TBranch        *b_M_opz;   //!
   TBranch        *b_M_ekin;   //!
   TBranch        *b_M_e;   //!
   TBranch        *b_M_edep;   //!
   TBranch        *b_M_edepAll;   //!
   TBranch        *b_M_stepL;   //!
   TBranch        *b_M_volID;   //!
   TBranch        *b_M_volName;   //!
   TBranch        *b_M_ppid;   //!
   TBranch        *b_M_ptid;   //!
   TBranch        *b_M_oprocess;   //!
   TBranch        *b_M_ovolName;   //!
   TBranch        *b_M_tid;   //!
   TBranch        *b_M_pid;   //!
   TBranch        *b_M_particleName;   //!
   TBranch        *b_M_charge;   //!
   TBranch        *b_M_stopped;   //!
   TBranch        *b_M_stop_time;   //!
   TBranch        *b_run_num;   //!
   TBranch        *b_evt_num;   //!
   TBranch        *b_R0;   //!
   TBranch        *b_R1;   //!
   TBranch        *b_i_px;   //!
   TBranch        *b_i_py;   //!
   TBranch        *b_i_pz;   //!
   TBranch        *b_i_x;   //!
   TBranch        *b_i_y;   //!
   TBranch        *b_i_z;   //!
   TBranch        *b_weight;   //!
   TBranch        *b_i_particleName;   //!

   MCProcessor(TTree * /*tree*/ =0) : fChain(0) { }
   virtual ~MCProcessor() { }
   virtual Int_t   Version() const { return 2; }
   virtual void    Begin(TTree *tree);
   virtual void    SlaveBegin(TTree *tree);
   virtual void    Init(TTree *tree);
   virtual Bool_t  Notify();
   virtual Bool_t  Process(Long64_t entry);
   virtual Int_t   GetEntry(Long64_t entry, Int_t getall = 0) { return fChain ? fChain->GetTree()->GetEntry(entry, getall) : 0; }
   virtual void    SetOption(const char *option) { fOption = option; }
   virtual void    SetObject(TObject *obj) { fObject = obj; }
   virtual void    SetInputList(TList *input) { fInput = input; }
   virtual TList  *GetOutputList() const { return fOutput; }
   virtual void    SlaveTerminate();
   virtual void    Terminate();

   ClassDef(MCProcessor,0);
};

#endif

#ifdef MCProcessor_cxx
void MCProcessor::Init(TTree *tree)
{
   // The Init() function is called when the selector needs to initialize
   // a new tree or chain. Typically here the branch addresses and branch
   // pointers of the tree will be set.
   // It is normally not necessary to make changes to the generated
   // code, but the routine can be extended by the user if needed.
   // Init() will be called many times when running on PROOF
   // (once per file to be processed).

   // Set object pointer
   M_Ox = 0;
   M_local_Ox = 0;
   M_Oy = 0;
   M_local_Oy = 0;
   M_Oz = 0;
   M_local_Oz = 0;
   M_Ot = 0;
   M_x = 0;
   M_local_x = 0;
   M_y = 0;
   M_local_y = 0;
   M_z = 0;
   M_local_z = 0;
   M_t = 0;
   M_Opx = 0;
   M_Opy = 0;
   M_Opz = 0;
   M_px = 0;
   M_py = 0;
   M_pz = 0;
   M_ox = 0;
   M_local_ox = 0;
   M_oy = 0;
   M_local_oy = 0;
   M_oz = 0;
   M_local_oz = 0;
   M_ot = 0;
   M_opx = 0;
   M_opy = 0;
   M_opz = 0;
   M_ekin = 0;
   M_e = 0;
   M_edep = 0;
   M_edepAll = 0;
   M_stepL = 0;
   M_volID = 0;
   M_volName = 0;
   M_ppid = 0;
   M_ptid = 0;
   M_oprocess = 0;
   M_ovolName = 0;
   M_tid = 0;
   M_pid = 0;
   M_particleName = 0;
   M_charge = 0;
   M_stopped = 0;
   M_stop_time = 0;
   i_particleName = 0;
   // Set branch addresses and branch pointers
   if (!tree) return;
   fChain = tree;
   fChain->SetMakeClass(1);

   fChain->SetBranchAddress("M_nHits", &M_nHits, &b_M_nHits);
   fChain->SetBranchAddress("M_Ox", &M_Ox, &b_M_Ox);
   fChain->SetBranchAddress("M_local_Ox", &M_local_Ox, &b_M_local_Ox);
   fChain->SetBranchAddress("M_Oy", &M_Oy, &b_M_Oy);
   fChain->SetBranchAddress("M_local_Oy", &M_local_Oy, &b_M_local_Oy);
   fChain->SetBranchAddress("M_Oz", &M_Oz, &b_M_Oz);
   fChain->SetBranchAddress("M_local_Oz", &M_local_Oz, &b_M_local_Oz);
   fChain->SetBranchAddress("M_Ot", &M_Ot, &b_M_Ot);
   fChain->SetBranchAddress("M_x", &M_x, &b_M_x);
   fChain->SetBranchAddress("M_local_x", &M_local_x, &b_M_local_x);
   fChain->SetBranchAddress("M_y", &M_y, &b_M_y);
   fChain->SetBranchAddress("M_local_y", &M_local_y, &b_M_local_y);
   fChain->SetBranchAddress("M_z", &M_z, &b_M_z);
   fChain->SetBranchAddress("M_local_z", &M_local_z, &b_M_local_z);
   fChain->SetBranchAddress("M_t", &M_t, &b_M_t);
   fChain->SetBranchAddress("M_Opx", &M_Opx, &b_M_Opx);
   fChain->SetBranchAddress("M_Opy", &M_Opy, &b_M_Opy);
   fChain->SetBranchAddress("M_Opz", &M_Opz, &b_M_Opz);
   fChain->SetBranchAddress("M_px", &M_px, &b_M_px);
   fChain->SetBranchAddress("M_py", &M_py, &b_M_py);
   fChain->SetBranchAddress("M_pz", &M_pz, &b_M_pz);
   fChain->SetBranchAddress("M_ox", &M_ox, &b_M_ox);
   fChain->SetBranchAddress("M_local_ox", &M_local_ox, &b_M_local_ox);
   fChain->SetBranchAddress("M_oy", &M_oy, &b_M_oy);
   fChain->SetBranchAddress("M_local_oy", &M_local_oy, &b_M_local_oy);
   fChain->SetBranchAddress("M_oz", &M_oz, &b_M_oz);
   fChain->SetBranchAddress("M_local_oz", &M_local_oz, &b_M_local_oz);
   fChain->SetBranchAddress("M_ot", &M_ot, &b_M_ot);
   fChain->SetBranchAddress("M_opx", &M_opx, &b_M_opx);
   fChain->SetBranchAddress("M_opy", &M_opy, &b_M_opy);
   fChain->SetBranchAddress("M_opz", &M_opz, &b_M_opz);
   fChain->SetBranchAddress("M_ekin", &M_ekin, &b_M_ekin);
   fChain->SetBranchAddress("M_e", &M_e, &b_M_e);
   fChain->SetBranchAddress("M_edep", &M_edep, &b_M_edep);
   fChain->SetBranchAddress("M_edepAll", &M_edepAll, &b_M_edepAll);
   fChain->SetBranchAddress("M_stepL", &M_stepL, &b_M_stepL);
   fChain->SetBranchAddress("M_volID", &M_volID, &b_M_volID);
   fChain->SetBranchAddress("M_volName", &M_volName, &b_M_volName);
   fChain->SetBranchAddress("M_ppid", &M_ppid, &b_M_ppid);
   fChain->SetBranchAddress("M_ptid", &M_ptid, &b_M_ptid);
   fChain->SetBranchAddress("M_oprocess", &M_oprocess, &b_M_oprocess);
   fChain->SetBranchAddress("M_ovolName", &M_ovolName, &b_M_ovolName);
   fChain->SetBranchAddress("M_tid", &M_tid, &b_M_tid);
   fChain->SetBranchAddress("M_pid", &M_pid, &b_M_pid);
   fChain->SetBranchAddress("M_particleName", &M_particleName, &b_M_particleName);
   fChain->SetBranchAddress("M_charge", &M_charge, &b_M_charge);
   fChain->SetBranchAddress("M_stopped", &M_stopped, &b_M_stopped);
   fChain->SetBranchAddress("M_stop_time", &M_stop_time, &b_M_stop_time);
   fChain->SetBranchAddress("run_num", &run_num, &b_run_num);
   fChain->SetBranchAddress("evt_num", &evt_num, &b_evt_num);
   fChain->SetBranchAddress("R0", &R0, &b_R0);
   fChain->SetBranchAddress("R1", &R1, &b_R1);
   fChain->SetBranchAddress("i_px", &i_px, &b_i_px);
   fChain->SetBranchAddress("i_py", &i_py, &b_i_py);
   fChain->SetBranchAddress("i_pz", &i_pz, &b_i_pz);
   fChain->SetBranchAddress("i_x", &i_x, &b_i_x);
   fChain->SetBranchAddress("i_y", &i_y, &b_i_y);
   fChain->SetBranchAddress("i_z", &i_z, &b_i_z);
   fChain->SetBranchAddress("weight", &weight, &b_weight);
   fChain->SetBranchAddress("i_particleName", &i_particleName, &b_i_particleName);
}

Bool_t MCProcessor::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return kTRUE;
}

#endif // #ifdef MCProcessor_cxx
