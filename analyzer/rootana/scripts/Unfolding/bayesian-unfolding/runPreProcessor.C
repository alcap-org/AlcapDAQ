void runPreProcessor() {
	TChain *chain = new TChain("tree");
	TString inputFileName = "/home/mark/montecarlo/proton.0-70.200k.target.100.leftright.root";
	chain->AddFile(inputFileName); //response matrix file
	chain->Process("MCProcessor.C+", inputFileName + " proton"); //protons, deuterons, tritons ...
}
