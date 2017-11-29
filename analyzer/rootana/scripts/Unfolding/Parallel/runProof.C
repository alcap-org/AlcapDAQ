void runProof(bool debug=false)
{
	if(debug) {
		gROOT->ProcessLine(".L SimplePulse.h");
		TChain *chain = new TChain("TMETree/TMETree");
		chain->AddFile("~/data/out/v1/out09682.root");
		//chain->AddFile("~/data/out/all.root");
		chain->Process("TMETree.C+");
	} else {
		TProof *p = TProof::Open("lite://", "workers=4");
		//p->SetBit(TProof::kUsingSessionGui);
		p->Load("SimplePulse.h");
		TChain *chain = new TChain("TMETree/TMETree");
		chain->AddFile("~/data/out/all.root");
		chain->SetProof();
		chain->Process("TMETree.C+");
	}
}
