void runProof(bool debug=true)
{
//        std::vector<TString> dataFiles;
//        TString dirname="/home/mark/data/out/Al100-full/";
//        TString ext=".root";
//        TSystemDirectory dir(dirname, dirname);
//        TList *files = dir.GetListOfFiles();
//        if (files) {
//                TSystemFile *file;
//                TString fname;
//                TIter next(files);
//                while ((file=(TSystemFile*)next())) {
//                        fname = file->GetName();
//                        if (!file->IsDirectory() && fname.EndsWith(ext)) {
//                                dataFiles.push_back(dirname + fname);
//                        }
//                }
//        }
	if(debug) {
		gROOT->ProcessLine(".L SimplePulse.h");
		gInterpreter->GenerateDictionary("vector<SimplePulse>", "SimplePulse.h;vector");
		gInterpreter->GenerateDictionary("SimplePulse", "SimplePulse.h");
		TChain *chain = new TChain("TMETree/TMETree");
                chain->Add("/home/mark/data/out/Al100.root");
		chain->Process("TMETree.C+");
	} else {
		TProof *p = TProof::Open("lite://", "workers=4");
		//p->SetBit(TProof::kUsingSessionGui);
		p->Load("SimplePulse.h");
		TChain *chain = new TChain("TMETree/TMETree");
                for(int i=0; i < dataFiles.size(); i++) {
                        chain->Add(dataFiles.at(i) );
                }
		chain->SetProof();
		chain->Process("TMETree.C+");
	}
}
