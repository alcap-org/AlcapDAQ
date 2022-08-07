void runProof(const char * set="ti50", bool debug=false)
{
        std::vector<TString> dataFiles;
        TString dirname="/data/R15b/tme/Al50/";
	if(std::strcmp(set, "al100")==0 ) {
		dirname = "/data/ssd3/R15bAnalysis/m-wong/al100/";
	} else if(std::strcmp(set, "MT") == 0 ) {
		dirname = "/data/ssd3/R15bAnalysis/m-wong/MT/tme/";
	} else if(std::strcmp(set, "al50")==0 ) {
		dirname = "/data/ssd2/R15bAnalysis/tmetree/Al50/";
	} else if(std::strcmp(set, "ti50")==0 ) {
		dirname="/data/ssd3/R15bAnalysis/m-wong/ti50/";
	} else if(std::strcmp(set, "si16b")==0 ) {
                dirname="/data/ssd3/R15bAnalysis/m-wong/Si16b/tme/";
        }
        TString ext=".root";
        TSystemDirectory dir(dirname, dirname);
        TList *files = dir.GetListOfFiles();
        if (files) {
                TSystemFile *file;
                TString fname;
                TIter next(files);
                while ((file=(TSystemFile*)next())) {
                        fname = file->GetName();
                        if (!file->IsDirectory() && fname.EndsWith(ext) && fname!="tme10128.root") {
                                dataFiles.push_back(dirname + fname);
                        }
                }
        }
	if(debug) {
//		gROOT->ProcessLine(".L SimplePulse.h");
//		gInterpreter->GenerateDictionary("vector<SimplePulse>", "SimplePulse.h;vector");
//		gInterpreter->GenerateDictionary("SimplePulse", "SimplePulse.h");

		TProof *p = TProof::Open("lite://", "workers=2");
		p->SetBit(TProof::kUsingSessionGui);
		TChain *chain = new TChain("TMETree/TMETree");
                for(int i=0; i < dataFiles.size(); i++) {
                        chain->Add(dataFiles.at(i) );
                }
		chain->SetProof();
		chain->Process("TMETree.C+", "test", 10000000);
	} else {
		TProof *p = TProof::Open("lite://", "workers=12");
		p->SetBit(TProof::kUsingSessionGui);
		TChain *chain = new TChain("TMETree/TMETree");
                for(int i=0; i < dataFiles.size(); i++) {
                        chain->Add(dataFiles.at(i) );
                }
		chain->SetProof();
		chain->Process("TMETree.C+", set);
	}
}
