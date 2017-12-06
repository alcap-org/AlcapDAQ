#ifndef TMETree_h
#define TMETree_h

#include "TFile.h"
#include "TTree.h"
#include "AlCapConstants.h"
#include "SimplePulse.h"
#include "TSelector.h"
#include "TH1.h"
#include "TH2.h"
#include "TF1.h"

class TMETree : public TSelector {
	public:
		std::map<const char *, std::vector<TH1D *> > hSiT;                    
		std::map<const char *, TH1D *> hGeLoGain;                        
		std::map<const char *, TH1D *> hGeHiGain;                        
		TH2D *hGeLoGainDrift511;
		TH2D *hGeHiGainDrift511;
		TH2D *hGeLoGainDrift1460;
		TH2D *hGeHiGainDrift1460;
		/**                                                            
		 *  * map contains, time, energy, likelihood probability cut, neural network cut as the key
		 *   * and histograms of the result, raw and cut                   
		 *    * */
		std::map<const char *, std::vector<TH2D *> > hSiL;                    
		std::map<const char *, std::vector<TH2D *> > hSiR;                    
		std::map<const char *, TH1D *> hProbabilityDistribution;         
		std::map<const char *, TF1 *> fitFunctions;

		TFile *fOutputFile;                                         
		Double_t SiL1_E, SiL3_E, SiR1_E, SiR2_E, SiR3_E;            
		TTree *tNNUnfolding;

		TH1D *hSiT_t;                                               
		TH1D *hSiT_E;                                               
		TH2D *hSiTCheck;

		TH1D *hSiL_tDiff;                                           
		TH1D *hSiR_tDiff;                                           
		TH2D *hSiL_EvdE;
		TH2D *hSiL_EvdE_proton;                                     
		TH2D *hSiL_EvdE_deuteron;                                   
		TH2D *hSiL_EvdE_triton;
		TH2D *hSiR_EvdE;
		TH2D *hSiR_EvdE_proton;                                     
		TH2D *hSiR_EvdE_deuteron;                                   
		TH2D *hSiR_EvdE_triton;

		Int_t runId; // run number
		Int_t blockId; // MIDAS event number
		Int_t TMEId; // muon event number
		Double_t TMEWindowWidth; // half width ofTMuonEvent window
		std::string* centralMuonChannel; // SiT channel that the central muon was seen in (e.g. SiT-1-S)
		Int_t centralMuonTPIID; // the ID of the central muon TPulseIsland
		Double_t centralMuonEnergy; // energy of the central muon
		Double_t centralMuonTime; // block time of the central muon
		Double_t timeToPrevTME; // time from the central muon in this TME to the central muon in the previous TME (-1 if first TME)
		Double_t timeToNextTME; // time from the central muon in this TME to the central muon in the next TME (-1 if last TME)
		Bool_t anyDoubleCountedPulses; // have any pulses in this TMuonEvent been included in more than one TMuonEvent?
		std::vector<SimplePulse>* SiT_1; // all the pulses in this channel for this TMuonEvent
		std::vector<SimplePulse>* SiT_2;
		std::vector<SimplePulse>* SiT_3;
		std::vector<SimplePulse>* SiT_4;
		std::vector<SimplePulse>* all_SiT_channels[n_SiT_channels];
		//std::vector<SimplePulse>* SiL1_1; // all the pulses in this channel for this TMuonEvent
		std::vector<SimplePulse>* SiL1_2;
		std::vector<SimplePulse>* SiL1_3;
		std::vector<SimplePulse>* SiL1_4;
		std::vector<SimplePulse>* SiL1_5;
		std::vector<SimplePulse>* SiL1_6;
		std::vector<SimplePulse>* SiL1_7;
		std::vector<SimplePulse>* SiL1_8;
		std::vector<SimplePulse>* SiL1_9;
		std::vector<SimplePulse>* SiL1_10;
		std::vector<SimplePulse>* SiL1_11;
		std::vector<SimplePulse>* SiL1_12;
		std::vector<SimplePulse>* SiL1_13;
		std::vector<SimplePulse>* SiL1_14;
		std::vector<SimplePulse>* SiL1_15;
		//std::vector<SimplePulse>* SiL1_16;
		std::vector<SimplePulse>* all_SiL1_channels[n_SiL1_channels];
		std::vector<SimplePulse>* SiL3;
		std::vector<SimplePulse>* SiR1_1; // all the pulses in this channel for this TMuonEvent
		std::vector<SimplePulse>* SiR1_2;
		std::vector<SimplePulse>* SiR1_3;
		std::vector<SimplePulse>* SiR1_4;
		std::vector<SimplePulse>* all_SiR1_channels[n_SiR1_channels];
		std::vector<SimplePulse>* SiR2;
		std::vector<SimplePulse>* SiR3;
		std::vector<SimplePulse>* GeLoGain;
		std::vector<SimplePulse>* GeHiGain;

		TTree *fChain;

		TMETree(TTree * /*tree*/ =0) { }
		virtual ~TMETree() { } 
		virtual Int_t   Version() const { return 1; }
		virtual void    Begin(TTree *tree);
		virtual Bool_t  Notify();
		virtual void    SlaveBegin(TTree *tree);
		virtual Bool_t  Process(Long64_t entry);
		virtual Int_t   GetEntry(Long64_t entry, Int_t getall = 0) { return fChain ? fChain->GetTree()->GetEntry(entry, getall) : 0; }
		virtual void    SetOption(const char *option) { fOption = option; }
		virtual void    SetObject(TObject *obj) { fObject = obj; }
		virtual void    SetInputList(TList *input) { fInput = input; }
		virtual TList  *GetOutputList() const { return fOutput; }
		virtual void    SlaveTerminate();
		virtual void    Terminate();

		void Fits();
		Double_t LikelihoodProbabilityCut(Double_t X, Double_t mu, Double_t sigma);

		ClassDef(TMETree, 2);
};
#endif

#ifdef TMETree_cxx
void TMETree::Begin(TTree *tree) {
	fChain = tree;
	fChain->SetBranchAddress("runId", &runId);
	fChain->SetBranchAddress("blockId", &blockId);
	fChain->SetBranchAddress("TMEId", &TMEId);
	fChain->SetBranchAddress("TMEWindowWidth", &TMEWindowWidth);
	fChain->SetBranchAddress("centralMuonChannel", &centralMuonChannel);
	fChain->SetBranchAddress("centralMuonTPIID", &centralMuonTPIID);
	fChain->SetBranchAddress("centralMuonEnergy", &centralMuonEnergy);
	fChain->SetBranchAddress("centralMuonTime", &centralMuonTime);
	fChain->SetBranchAddress("timeToPrevTME", &timeToPrevTME);
	fChain->SetBranchAddress("timeToNextTME", &timeToNextTME);
	fChain->SetBranchAddress("anyDoubleCountedPulses", &anyDoubleCountedPulses);
	fChain->SetBranchAddress("SiT_1", &SiT_1);
	fChain->SetBranchAddress("SiT_2", &SiT_2);
	fChain->SetBranchAddress("SiT_3", &SiT_3);
	fChain->SetBranchAddress("SiT_4", &SiT_4);
	fChain->SetBranchAddress("SiL1_2", &SiL1_2);
	fChain->SetBranchAddress("SiL1_3", &SiL1_3);
	fChain->SetBranchAddress("SiL1_4", &SiL1_4);
	fChain->SetBranchAddress("SiL1_5", &SiL1_5);
	fChain->SetBranchAddress("SiL1_6", &SiL1_6);
	fChain->SetBranchAddress("SiL1_7", &SiL1_7);
	fChain->SetBranchAddress("SiL1_8", &SiL1_8);
	fChain->SetBranchAddress("SiL1_9", &SiL1_9);
	fChain->SetBranchAddress("SiL1_10", &SiL1_10);
	fChain->SetBranchAddress("SiL1_11", &SiL1_11);
	fChain->SetBranchAddress("SiL1_12", &SiL1_12);
	fChain->SetBranchAddress("SiL1_13", &SiL1_13);
	fChain->SetBranchAddress("SiL1_14", &SiL1_14);
	fChain->SetBranchAddress("SiL1_14", &SiL1_15);
	fChain->SetBranchAddress("SiL3", &SiL3);
	fChain->SetBranchAddress("SiR1_1", &SiR1_1);
	fChain->SetBranchAddress("SiR1_2", &SiR1_2);
	fChain->SetBranchAddress("SiR1_3", &SiR1_3);
	fChain->SetBranchAddress("SiR1_4", &SiR1_4);
	fChain->SetBranchAddress("SiR2", &SiR2);
	fChain->SetBranchAddress("SiR3", &SiR3);
	fChain->SetBranchAddress("GeLoGain", &GeLoGain);
	fChain->SetBranchAddress("GeHiGain", &GeHiGain);
	Fits();
}

void TMETree::Fits() {
	TF1 *p_led = new TF1("left_proton", "[0]*(1-[1]/x)**[2]*exp(-x/[3])", 1.5, 10);
	p_led->SetParameter(0, 0.585205);
	p_led->SetParameter(1, -0.329325);
	p_led->SetParameter(2, 8.51648);
	p_led->SetParameter(3, 16.3396);
	fitFunctions["left_proton"] = p_led;

	TF1 *d_led = new TF1("left_deuteron", "[0]*(1-[1]/x)**[2]*exp(-x/[3])", 2.0, 11);
	d_led->SetParameter(0, 0.704607);
	d_led->SetParameter(1, -2.26743);
	d_led->SetParameter(2, 2.13257);
	d_led->SetParameter(3, 24.0647);
	fitFunctions["left_deuteron"] = d_led;

	TF1 *t_led = new TF1("left_triton", "[0]*(1-[1]/x)**[2]*exp(-x/[3])", 2.5, 12);
	t_led->SetParameter(0, 0.941740);
	t_led->SetParameter(1, -2.34857);
	t_led->SetParameter(2, 2.10046);
	t_led->SetParameter(3, 24.3845);
	fitFunctions["left_triton"] = t_led;

	TF1 *p_red = new TF1("right_proton", "[0]*(1-[1]/x)**[2]*exp(-x/[3])", 1.5, 10);
	p_red->SetParameter(0, 0.62285);
	p_red->SetParameter(1, -0.251733);
	p_red->SetParameter(2, 11.1821);
	p_red->SetParameter(3, 12.538);
	fitFunctions["right_proton"] = p_red;

	TF1 *d_red = new TF1("right_deuteron", "[0]*(1-[1]/x)**[2]*exp(-x/[3])", 2.0, 11);
	d_red->SetParameter(0, 0.839878);
	d_red->SetParameter(1, -0.119888);
	d_red->SetParameter(2, 29.9925);
	d_red->SetParameter(3, 19.1545);
	fitFunctions["right_deuteron"] = d_red;

	TF1 *t_red = new TF1("right_triton", "[0]*(1-[1]/x)**[2]*exp(-x/[3])", 2.5, 12);
	t_red->SetParameter(0, 0.964517);
	t_red->SetParameter(1, -2.73694);
	t_red->SetParameter(2, 2.04411);
	t_red->SetParameter(3, 21.1987);
	fitFunctions["right_triton"] = t_red;

	for(std::map<const char *, TF1 *>::iterator it = fitFunctions.begin(); it != fitFunctions.end(); it++) {
		fOutput->Add(it->second);
	}

}

Bool_t TMETree::Notify()
{
	// The Notify() function is called when a new file is opened. This
	// can be either for a new TTree in a TChain or when when a new TTree
	// is started when using PROOF. It is normally not necessary to make changes
	// to the generated code, but the routine can be extended by the
	// user if needed. The return value is currently not used.
	return kTRUE;
}
#endif // #ifdef TMETree_cxx
