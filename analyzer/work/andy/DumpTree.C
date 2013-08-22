// DumpTree.C
// -- A ROOT macro that dumps the information of everything in the tree

void DumpTree() {
  
  gSystem->Load("libDataClasses.so");
  
  TFile* file = new TFile("tree00403.root");
  TTree* tree = (TTree*) file->Get("EventTree");

  map<string, vector<TPulseIsland*> > theMap;

  tree->SetMakeClass(1); // needed this to get theMap out for some reason
  tree->SetBranchAddress("fPulseIslandToChannelMap", &theMap);

  // Loop through the entries in the tree
  for (int iEntry = 0; iEntry < tree->GetEntries(); iEntry++) {
    std::cout << "Entry #: " << iEntry << std::endl;
    std::cout << "Bytes read: " << tree->GetEntry(iEntry) << std::endl;
    std::cout << "# banks in map: " << theMap.size() << std::endl;
    
    // Loop through the banks
    for (map<string, vector<TPulseIsland*> >::iterator mapIter = theMap.begin();
	 mapIter != theMap.end();
	 mapIter++) {
      
      std::cout << "\tBank name: " << mapIter->first << std::endl;

      vector<TPulseIsland*> theVector = theMap[mapIter->first];
      if (!theVector.empty()) {

	std::cout << "\t\t# TPulseIsland* in vector = " << theVector.size() << std::endl;

	// Loop through the TPulseIslands
	for (vector<TPulseIsland*>::iterator islandIter = theVector.begin();
	     islandIter != theVector.end();
	     islandIter++) {
	  
	  std::cout << "\t\t\tBank Name = " <<  (*islandIter)->GetBankName() << std::endl;
	  std::cout << "\t\t\tClockTickInNs = " << (*islandIter)->GetClockTickInNs() << std::endl;
	  std::cout << "\t\t\tTime Stamp = " <<  (*islandIter)->GetTimeStamp() << std::endl;

	  std::vector<int> theSamples = (*islandIter)->GetSamples();
	  std::cout << "\t\t\t# of Samples = " << theSamples.size() << std::endl;

	  // Loop through the samples
	  int sampleCounter = 1;
	  for (std::vector<int>::iterator sampleIter = theSamples.begin();
	       sampleIter != theSamples.end();
	       sampleIter++) {
	    
	    std::cout << "\t\t\t\tSample #" << sampleCounter << " = " << *sampleIter << std::endl; 
	    sampleCounter++;
	  }
	}
      }
      else
	std::cout << "\t\tTPulseIsland* vector is empty" << std::endl;
    }    
    std::cout << std::endl;
  }
}
