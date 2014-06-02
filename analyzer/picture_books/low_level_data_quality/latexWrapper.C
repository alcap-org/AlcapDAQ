#include <TROOT.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <TH1.h>
#include <TH2.h>
#include <TFile.h>
#include <TKey.h>

#include <iostream>
#include <sstream>

/*

 > root [0] .L latexWrapper.C++
 > root [1] latexWrapper(n_run)

*/

const int n_sections = 8;
const int n_max_subsections = 5;
struct Section {
  std::string section_name;
  bool section_started;

  std::string subsection_names[n_max_subsections];
  bool subsection_started[n_max_subsections];
} section_headings[n_sections];

void latexHeader(FILE * pFile,const int run_number, bool run_book);
void insertFig(FILE * pFile,TString imageFile);
std::vector<std::string> getListOfLines();
void createSections();

void latexWrapper(int run_number, bool run_book) {

  gROOT->Reset();
  gROOT->SetStyle("Plain");
  gStyle->SetCanvasBorderMode(0); // turn off canvas borders

  // Define the sections and get the list of lines (i.e. plot PDF names and section headings)
  createSections();
  std::vector<std::string> list_of_lines = getListOfLines();

   //begin latex file
   FILE * pFile;

   pFile = fopen (Form("Data_Quality_Run%d.tex",run_number),"w");

   //header
   latexHeader(pFile,run_number, run_book);   

   fprintf (pFile, "\\newpage \n \\clearpage\n\n");
   int n_figs = 0;

   for (std::vector<std::string>::iterator plotIter = list_of_lines.begin(); plotIter != list_of_lines.end(); ++plotIter) {
     if ( (*plotIter).find("section") != std::string::npos) {
       std::cout << "Adding section heading: " << *plotIter << std::endl;
       fprintf (pFile, (*plotIter).c_str());
     }
     else {
       std::cout << "Inserting figure " << *plotIter << std::endl;
       insertFig(pFile,*plotIter);
       fprintf (pFile, "\\vspace{1.5cm}"); // add a bit of space between figures
       ++n_figs;

       if (n_figs%2 == 0) {
	 fprintf(pFile,"\\clearpage \n\n");
       }
     }
   }


   fprintf (pFile, "\\newpage \n \\clearpage\n\n");


   //close the latex file
   fprintf (pFile, "\\end{document}\n");
   fclose (pFile);


}

void createSections() {

  // Set defaults for section_started, subsection_names and subsection_started
  for (int iSection = 0; iSection < n_sections; ++iSection) {

      section_headings[iSection].section_started = false;

      for (int iSubSection = 0; iSubSection < n_max_subsections; ++iSubSection) {

	(section_headings[iSection]).subsection_names[iSubSection] = "";
	(section_headings[iSection]).subsection_started[iSubSection] = false;
      }
  }

  // Now do specifics
  section_headings[0].section_name = "\\section{FADC-specific data quality issues}\n\n";
  (section_headings[0]).subsection_names[0] = "\\subsection{Packet loss}\n\n";
  (section_headings[0]).subsection_names[1] = "\\subsection{Buffer overflow}\n\n";

  section_headings[1].section_name = "\\section{Digitizer overflows}\n\n";

  section_headings[2].section_name = "\\section{TPulseIsland-specific data quality issues}\n\n";
  (section_headings[2]).subsection_names[0] = "\\subsection{Total number of TPulseIslands}\n\n";
  (section_headings[2]).subsection_names[1] = "\\subsection{Length of TPulseIslands}\n\n";
  (section_headings[2]).subsection_names[2] = "\\subsection{Rate of TPulseIslands}\n\n";
  (section_headings[2]).subsection_names[3] = "\\subsection{Timestamps of TPulseIslands}\n\n";
  (section_headings[2]).subsection_names[4] = "\\subsection{Amplitudes of TPulseIslands}\n\n";

  section_headings[3].section_name = "\\section{Pulse persistancy plots}\n\n";

  section_headings[4].section_name = "\\section{TDC-specific data quality issues}\n\n";
  (section_headings[4]).subsection_names[0] = "\\subsection{Rate of muSc hits}\n\n";
  (section_headings[4]).subsection_names[1] = "\\subsection{Unknown TDC parameters}\n\n";
  (section_headings[4]).subsection_names[2] = "\\subsection{Time difference between muSc hits in TDC and BU CAEN}\n\n";

  section_headings[5].section_name = "\\section{Time differences between channels and muSc hit}\n\n";

  section_headings[6].section_name = "\\section{Run time}\n\n";

  section_headings[7].section_name = "\\section{DAQ-specific data quality issues}\n\n";
  (section_headings[7]).subsection_names[0] = "\\subsection{DAQ Livetime}\n\n";
  (section_headings[7]).subsection_names[1] = "\\subsection{Pedestal}\n\n";
  (section_headings[7]).subsection_names[2] = "\\subsection{Noise}\n\n";
  (section_headings[7]).subsection_names[3] = "\\subsection{Thresholds}\n\n";

}

std::vector<std::string> getListOfLines() {

  // Print the list of plots to a file
  gROOT->ProcessLine(".! ls data_quality_figs/*.png > list_of_lines.txt");

  // Open this file for reading
  FILE * pListOfLinesFile;
  pListOfLinesFile = fopen("list_of_lines.txt", "r");

  // Get a vector of plot names ready
  std::vector<std::string> plot_names;
  const int max_char = 100;
  char plotname_cstr[max_char];

  // Read in the plot names
  if (pListOfLinesFile == NULL) perror ("Error opening list_of_lines.txt");
  else
    {
      while ( ! feof (pListOfLinesFile) )
	{
	  if ( fgets (plotname_cstr , max_char , pListOfLinesFile) == NULL ) break;
	  
	  // strip off the final \n
	  int length = strlen(plotname_cstr);
	  if (plotname_cstr[length - 1] == '\n')
	    plotname_cstr[length - 1] = '\0';

	  std::string plotname = plotname_cstr; // convert to std::string so I can use find()

	  // Work out which section and subsection this plot is for
	  int section = -1;
	  int subsection = -1;

	  if (plotname.find("FADC") != std::string::npos) {
	    section = 0;

	    // Check for subsection
	    if (plotname.find("PacketLoss") != std::string::npos) {
	      subsection = 0;
	    }
	    else if (plotname.find("BufferOverflow") != std::string::npos) {
	      subsection = 1;
	    }
	  }
	  else if (plotname.find("Digitizer") != std::string::npos) {
	    section = 1;
	  }
	  else if (plotname.find("Island") != std::string::npos) {
	    section = 2;

	    // Check for subsection
	    if (plotname.find("Counter") != std::string::npos) {
	      subsection = 0;
	    }
	    else if (plotname.find("Length") != std::string::npos) {
	      subsection = 1;
	    }
	    else if (plotname.find("Rate") != std::string::npos) {
	      subsection = 2;
	    }
	    else if (plotname.find("Timestamp") != std::string::npos) {
	      subsection = 3;
	    }
	    else if (plotname.find("Amplitude") != std::string::npos) {
	      subsection = 4;
	    }
	  }
	  else if (plotname.find("TDCCheck") != std::string::npos) {
	    section = 4;

	    // Check for subsection
	    if (plotname.find("muSc_rate") != std::string::npos) {
	      subsection = 0;
	    }
	    else if (plotname.find("Unknown") != std::string::npos) {
	      subsection = 1;
	    }
	    else if (plotname.find("TDiff") != std::string::npos) {
	      subsection = 2;
	    }
	  }
	  else if (plotname.find("muScTDiff") != std::string::npos) {
	    section = 5;
	  }
	  else if (plotname.find("RunTime") != std::string::npos) {
	    section = 6;
	  }
	  else if (plotname.find("DAQ") != std::string::npos) {
	    section = 7;
	    // Check for subsection
	    if (plotname.find("Livetime") != std::string::npos) {
	      subsection = 0;
	    }
	    else if (plotname.find("Pedestal") != std::string::npos) {
	      subsection = 1;
	    }
	    else if (plotname.find("Noise") != std::string::npos) {
	      subsection = 2;
	    }
	    else if (plotname.find("Thresholds") != std::string::npos) {
	      subsection = 3;
	    }
	  }
	  else if (plotname.find("PulseShapes") != std::string::npos) { // want the Pedestal and Noise plots to be put into a separate section even though they are created in this module
	    section = 3;
	  }


	  // If the section was found...
	  if (section >= 0) {
	    // ...add the section heading if it hasn't been started yet
	    if (section_headings[section].section_started == false) {
	      plot_names.push_back(section_headings[section].section_name);
	      section_headings[section].section_started = true;
	    }
	  }

	  // If the subsection was found...
	  if (subsection >= 0 && section >= 0) {
	    // ...add the subsection heading if it hasn't been started yet
	    if (section_headings[section].subsection_started[subsection] == false &&
		section_headings[section].subsection_names[subsection] != "" ) {

	      plot_names.push_back(section_headings[section].subsection_names[subsection]);
	      section_headings[section].subsection_started[subsection] = true;
	    }
	  }
	  
	  // Finally put the plot name in
	  plot_names.push_back(plotname);
	}
      fclose (pListOfLinesFile);
    }

  return plot_names;
}

void insertFig(FILE * pFile,TString imageFile) {

  fprintf(pFile,"\\begin{figure}[h] \n");
  fprintf(pFile,"\\begin{center} \n");
  fprintf(pFile,"\\includegraphics[height=0.33\\textheight, angle=0]{%s} \n",imageFile.Data());
  fprintf(pFile,"\\end{center} \n");
  fprintf(pFile,"\\vspace{-5mm} \n");
  fprintf(pFile,"\\caption{} \n");
  fprintf(pFile,"\\end{figure} \n");
}

void latexHeader(FILE * pFile,const int run_number, bool run_book){

  char *target; int first_run=0; int last_run=0;
   // Hard-code some info about the datsets and run numbers (probably a better way to do this)
   if (run_number>=2091 && run_number <= 2172) { target = "SiR"; first_run=2091; last_run=2172; }
   else if (run_number>=2808 && run_number <= 3012) { target = "Al100"; first_run=2808; last_run=3012; }
   else if (run_number>=3101 && run_number <= 3456) { target = "Al50(a)"; first_run=3101; last_run=3456; }
   else if (run_number>=3474 && run_number <= 3540) { target = "Si16P"; first_run=3474; last_run=3540; }
   else if (run_number>=3563 && run_number <= 3721) { target = "Al50(b)"; first_run=3563; last_run=3721; }
   else if (run_number>=3763 && run_number <= 3770) { target = "SiR2(3\\%)"; first_run=3763; last_run=3770; }
   else if (run_number>=3771 && run_number <= 3779) { target = "SiR2(1\\%)"; first_run=3771; last_run=3779; }

   fprintf (pFile, "\\documentclass[12pt,twoside,titlepage]{article}\n");
   fprintf (pFile, "\\usepackage{geometry}\n");
   fprintf (pFile, "\\geometry{verbose,tmargin=10mm,bmargin=20mm,lmargin=10mm,rmargin=10mm}\n");
   fprintf (pFile, "\\usepackage{graphics}\n");
   fprintf (pFile, "\\usepackage{rotating}\n");
   fprintf (pFile, "\\usepackage{textcomp}\n");
   fprintf (pFile, "\\usepackage{subfigure}\n");
   fprintf (pFile, "\\usepackage{color}\n");
   fprintf (pFile, "\\usepackage[pdftex,bookmarks,colorlinks]{hyperref}\n\n\n");
   fprintf (pFile, "\\begin{document}\n");

   if (run_book)
     fprintf (pFile, "\\title {Data Quality Summary for AlCap Run \\#%d (%s Target)}\n",run_number,target);
   else
     fprintf (pFile, "\\title {Data Quality Summary for AlCap Dataset %s (Runs \\#%d - %d)}\n",target,first_run,last_run);

   fprintf (pFile, "\\date{Produced \\today}\n\n");
   fprintf (pFile, "\\author{Macro by A. Edmonds and J. Grange}\n\n");
   fprintf (pFile, "\\maketitle\n");
   fprintf (pFile, "\\newpage\n\n");
   fprintf (pFile, "\\tableofcontents\n\n");

  

}
