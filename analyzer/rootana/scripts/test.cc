{
	gROOT->ProcessLine("#include <map>");
	gROOT->ProcessLine("#include <pair>");
	gROOT->ProcessLine("#include <vector>");
	gROOT->ProcessLine(".L ./libAnalysis.so");

	TFile *file = 
		TFile::Open("~/work/RunPSI2013/data/root/analysedtree/anatree02808.root");

	TTree *tree = (TTree*)file->Get("AnalysedPulseTree");
	tree->Print();

	TAnalysedPulseMapWrapper *mapWrapper;
	map<string, vector<TAnalysedPulse*> > currentMap;

	//string detname;
	//vector<TAnalysedPulse *> theVector;
	tree->SetMakeClass(1);
	//tree->SetBranchAddress("AnalysedPulse", &mapWrapper);
	tree->SetBranchAddress("fChn2PulseMap", &currentMap);


	int nentries = tree->GetEntriesFast();
	//for (int i = 0; i < nentries; ++i)
	for (int i = 0; i < 1; ++i)
	{
		std::cout<<"Event "<<i<<", ";
		std::cout<<"byte read: "<<tree->GetEntry(i);
		std::cout<<", size: "<<currentMap.size()<<std::endl;
		//mapWrapper->ShowInfo();
		
		for (map<string, vector <TAnalysedPulse*> >::iterator mapIter = currentMap.begin(); 
				mapIter != currentMap.end(); ++mapIter)
		{
			string detname = mapIter->first;
			vector<TAnalysedPulse *> pulseVector = mapIter->second;

			std::cout<<detname<<": "<< pulseVector.size()<<" pulses\n";

			vector<TAnalysedPulse *>::iterator pulseIter;
			for (pulseIter = pulseVector.begin(); 
					pulseIter != pulseVector.end(); ++pulseIter)
			{
				printf("height: %.2f\n", (*pulseIter)->GetAmplitude());
			}
		}
	}
}
