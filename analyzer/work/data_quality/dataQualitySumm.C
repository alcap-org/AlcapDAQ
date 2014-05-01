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

 > root [0] .L dataQualitySumm.C++
 > root [1] dataQualitySumm(n_run)

*/

void latexHeader(FILE * pFile,const int n_run);
void insertFig(FILE * pFile,TH1F *fig_name,const int n_run);
void insertFig(FILE * pFile,TH2F *fig_name,const int n_run);
void insertFig(FILE * pFile,TString imageFile);
std::vector<std::string> getListOfPlots();

void dataQualitySumm(const char* data_dir, const int n_run) {

  gROOT->Reset();
  gROOT->SetStyle("Plain");
  gStyle->SetCanvasBorderMode(0); // turn off canvas borders

  TFile* file;

  std::stringstream filename;
  filename << data_dir << "/hist/hist0" << n_run << ".root";
  file = new TFile(filename.str().c_str(), "READ");

  std::vector<std::string> list_of_plots = getListOfPlots();

  TH1F *hDQ_FADCPacketLoss_Fraction;
  file->GetObject("DataQuality_LowLevel/hDQ_FADCPacketLoss_Fraction",hDQ_FADCPacketLoss_Fraction);
  hDQ_FADCPacketLoss_Fraction->GetYaxis()->SetTitleOffset(1.3);
   //begin latex file
   FILE * pFile;

   pFile = fopen (Form("Data_Quality_Run%d.tex",n_run),"w");

   //header
   latexHeader(pFile,n_run);   

   fprintf (pFile, "\\newpage \n \\clearpage\n\n");
   fprintf (pFile, "\\section{FADC-specific data quality issues}\n\n");
   fprintf (pFile, "\\subsection{Packet loss}\n\n");

   for (std::vector<std::string>::iterator plotIter = list_of_plots.begin(); plotIter != list_of_plots.end(); ++plotIter) {
     std::cout << *plotIter << std::endl;
     insertFig(pFile,*plotIter);
   }

   fprintf (pFile, "\\subsection{Buffer overflow}\n\n");

   fprintf (pFile, "\\newpage \n \\clearpage\n\n");
   fprintf (pFile, "\\section{Digitizer overflows}\n\n");


   //close the latex file
   fprintf (pFile, "\\end{document}\n");
   fclose (pFile);


}

std::vector<std::string> getListOfPlots() {
  gROOT->ProcessLine(".! ls data_quality_figs/*.pdf > list_of_plots.txt");

  FILE * pListOfPlotsFile;
  pListOfPlotsFile = fopen("list_of_plots.txt", "r");

  std::vector<std::string> plot_names;
  const int max_char = 100;
  char plotname[max_char];
  if (pListOfPlotsFile == NULL) perror ("Error opening list_of_plots.txt");
  else
    {
      while ( ! feof (pListOfPlotsFile) )
	{
	  if ( fgets (plotname , max_char , pListOfPlotsFile) == NULL ) break;
	  
	  // strip off the final \n
	  int length = strlen(plotname);
	  if (plotname[length - 1] == '\n') 
	    plotname[length - 1] = '\0';

	  plot_names.push_back(plotname);
	}
      fclose (pListOfPlotsFile);
    }

  return plot_names;
}

void insertFig(FILE * pFile,TH1F *hist,const int n_run){

  TString imageFile = "data_quality_figs/";
  imageFile += hist->GetName();
  imageFile += Form("_run%d",n_run);
  imageFile += ".pdf";


  //make the image file
  TCanvas *c1 = new TCanvas();
  hist->Draw();
  c1->Print(imageFile.Data());

  fprintf(pFile,"\\begin{figure}[h] \n");
  fprintf(pFile,"\\begin{center} \n");
  fprintf(pFile,"\\includegraphics[height=12cm, angle=0]{%s} \n",imageFile.Data());
  fprintf(pFile,"\\end{center} \n");
  fprintf(pFile,"\\vspace{-5mm} \n");
  fprintf(pFile,"\\caption{} \n");
  fprintf(pFile,"\\end{figure} \n");
  fprintf(pFile,"\\clearpage \n");
  

}

void insertFig(FILE * pFile,TH2F *hist,const int n_run){

}

void insertFig(FILE * pFile,TString imageFile) {

  fprintf(pFile,"\\begin{figure}[h] \n");
  fprintf(pFile,"\\begin{center} \n");
  fprintf(pFile,"\\includegraphics[height=12cm, angle=0]{%s} \n",imageFile.Data());
  fprintf(pFile,"\\end{center} \n");
  fprintf(pFile,"\\vspace{-5mm} \n");
  fprintf(pFile,"\\caption{} \n");
  fprintf(pFile,"\\end{figure} \n");
  fprintf(pFile,"\\clearpage \n");
}

void latexHeader(FILE * pFile,const int n_run){

   char *target;
   if (n_run==2) target = "SiR";

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
   fprintf (pFile, "\\title {Data Quality Summary for AlCap Run \\#%d (%s Target)}\n",n_run,target);
   fprintf (pFile, "\\date{Produced \\today}\n\n");
   fprintf (pFile, "\\author{Macro by J. Grange, Argonne National Laboratory}\n\n");
   fprintf (pFile, "\\maketitle\n");
   fprintf (pFile, "\\newpage\n\n");
   fprintf (pFile, "\\tableofcontents\n\n");

  

}
