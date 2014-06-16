#include <TROOT.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <TH1.h>
#include <TH2.h>
#include <TFile.h>
#include <TKey.h>

#include <iostream>
#include <sstream>

void RunPlots(const char* data_dir, int first_run, const int n_runs) {

  gROOT->Reset();
  gROOT->SetStyle("Plain");
  gStyle->SetCanvasBorderMode(0); // turn off canvas borders

  TCanvas* c1 = new TCanvas("c1", "c1");

  // Go through the runs and create a PDF for each one
  for (int iRun = 0; iRun < n_runs; ++iRun) {

    // Open up the file for this run
    std::stringstream filename;
    filename << data_dir << "/hist/hist0" << first_run + iRun << ".root";
    TFile* file = new TFile(filename.str().c_str(), "READ");

    // Create the output PDF file to save all the plots to
    std::stringstream basepdfname;
    basepdfname << "DQ_LowLevel_RunPlots_Run0" << first_run + iRun << ".pdf";
    
    std::string openpdfname = basepdfname.str() + "[";
    std::string closepdfname = basepdfname.str() + "]";
    c1->Print(openpdfname.c_str());

    TIter nextFileKey(file->GetListOfKeys()); // get the list of keys in this file
    TKey *fileKey;
        
    while (fileKey = (TKey*)nextFileKey()) {

      // Get the directory for low-level data quality plots
      if (strcmp(fileKey->ReadObj()->ClassName(), "TDirectoryFile") == 0) {
	// Check that the directory has the correct name
	if (strcmp(fileKey->ReadObj()->GetName(), "DataQuality_LowLevel") == 0) {

	  TDirectoryFile* dir = (TDirectoryFile*) fileKey->ReadObj();

	  TIter nextDirKey(dir->GetListOfKeys()); // get the list of keys in the directory (all histograms should be in this folder)
	  TKey *dirKey;
        
	  while (dirKey = (TKey*)nextDirKey()) {
	    
	    if (strcmp(dirKey->ReadObj()->ClassName(), "TH1F") == 0) {

	      // Get histogram and print
	      TH1F* hist = (TH1F*) dirKey->ReadObj();
	      hist->Draw();
	      c1->Update();
	      c1->Print(basepdfname.str().c_str());
	    }
	    else if (strcmp(dirKey->ReadObj()->ClassName(), "TH2F") == 0) {

	      TH2F* hist = (TH2F*) dirKey->ReadObj();
	      
	      // Search through the histogram name
	      if (strstr(hist->GetName(), "PulseShapes") != NULL) {
		c1->SetLogz();
	      }

	      hist->Draw("COLZ");
	      c1->Update();
	      c1->Print(basepdfname.str().c_str());
	    } // end if statements

	    // Undo any c1 log scales
	    c1->SetLogz(0);
	  } // end while loop through histograms in directory
	} // end if correctly name directory
      } // end if the key is a directory
    } // end while loop through keys in file

    // Close the PDF file
    c1->Print(closepdfname.c_str());

  } // end loop through each run file
}
