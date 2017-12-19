#include "scripts/ExportPulse/DrawTPIAndTemplateTAPs_fromPulseViewer.C"

#include <fstream>
#include <sstream>
#include <iomanip>

struct PulseToDraw {
  PulseToDraw(std::string d, int e, int p) : detname(d), event_number(e), pulse_number(p) {}

  std::string detname;
  int event_number;
  int pulse_number;
};

void FillPulsesToDraw(std::vector<PulseToDraw>& pulses);

void PulsePictureBook() {

  int run_number = 9740;

  std::ofstream out_file;
  out_file.open("picture-book/picture-book.tex", std::ofstream::out);

  out_file << "\\documentclass[smaller]{beamer}" << std::endl;
  out_file << "\\setbeamertemplate{footline}[frame number]" << std::endl;
  out_file << "\\usepackage{wasysym}" << std::endl;
  out_file << "\\usepackage{multirow}" << std::endl;
  out_file << "\\title{Picture Book for Run " << run_number << "}" << std::endl;
  out_file << "\\date{\\today}" << std::endl;
  out_file << "\\setbeamertemplate{sidebar right}{}" << std::endl;
  out_file << "\\begin{document}" << std::endl;
  out_file << "\\begin{frame}" << std::endl;
  out_file << "\\titlepage" << std::endl;
  out_file << "\\end{frame}" << std::endl;
  out_file << "\\section{Picture Book}" << std::endl;

  std::stringstream filename;
  filename << "out" << std::setw(5) << std::setfill('0') << run_number << "_template-fits.root";

  std::vector<PulseToDraw> pulses_to_draw;
  FillPulsesToDraw(pulses_to_draw);
  std::stringstream histname;

  for (std::vector<PulseToDraw>::const_iterator i_pulse = pulses_to_draw.begin(); i_pulse != pulses_to_draw.end(); ++i_pulse) {
    histname.str("");
    histname << "Pulse_" << i_pulse->detname << "_" << i_pulse->event_number << "_" << i_pulse->pulse_number;
    TCanvas c1("c1", "c1", 1200, 800);
    DrawTPIAndTemplateTAPs_fromPulseViewer(filename.str(), histname.str(), c1);
    
    std::string pdfname = "figs/" + histname.str() + ".pdf";
    std::string savename = "picture-book/" + pdfname;
    c1.SaveAs(savename.c_str());
    
    out_file << "\\begin{frame}" << std::endl;
    out_file << "\\begin{center}" << std::endl;
    out_file << "\\includegraphics[width=1.0\\textwidth]{" << pdfname << "}" << std::endl;
    out_file << std::endl << "Event " << i_pulse->event_number << " Pulse " << i_pulse->pulse_number << std::endl;
    out_file << "\\end{center}" << std::endl;
    out_file << "\\end{frame}" << std::endl << std::endl;
  }
  out_file << "\\end{document}" << std::endl;
  out_file.close();
}

void FillPulsesToDraw(std::vector<PulseToDraw>& pulses) {

  // Interesting ones
  /*  pulses.push_back(PulseToDraw("SiL1_9_S", 0, 2));
  pulses.push_back(PulseToDraw("SiL1_9_S", 0, 5));
  pulses.push_back(PulseToDraw("SiL1_9_S", 1, 6));
  pulses.push_back(PulseToDraw("SiL1_9_S", 1, 7));
  pulses.push_back(PulseToDraw("SiL1_9_S", 1, 9));
  pulses.push_back(PulseToDraw("SiL1_9_S", 1, 10));
  pulses.push_back(PulseToDraw("SiL1_9_S", 2, 1));
  pulses.push_back(PulseToDraw("SiL1_9_S", 2, 7));
  pulses.push_back(PulseToDraw("SiL1_9_S", 4, 5));
  pulses.push_back(PulseToDraw("SiL1_9_S", 5, 5));
  pulses.push_back(PulseToDraw("SiL1_9_S", 5, 6));
  pulses.push_back(PulseToDraw("SiL1_9_S", 5, 8));
  pulses.push_back(PulseToDraw("SiL1_9_S", 6, 2));
  pulses.push_back(PulseToDraw("SiL1_9_S", 6, 6));
  pulses.push_back(PulseToDraw("SiL1_9_S", 6, 8));
  pulses.push_back(PulseToDraw("SiL1_9_S", 7, 5));
  pulses.push_back(PulseToDraw("SiL1_9_S", 7, 8));
  pulses.push_back(PulseToDraw("SiL1_9_S", 7, 15));
  //  pulses.push_back(PulseToDraw("SiL1_9_S", 8, 7));
  */
  // All pulses
  pulses.push_back(PulseToDraw("SiL1_9_S", 0, 0)); 
  pulses.push_back(PulseToDraw("SiL1_9_S", 0, 1));
  pulses.push_back(PulseToDraw("SiL1_9_S", 0, 2));  
  pulses.push_back(PulseToDraw("SiL1_9_S", 0, 3));
  pulses.push_back(PulseToDraw("SiL1_9_S", 0, 4));
  pulses.push_back(PulseToDraw("SiL1_9_S", 0, 5));
  pulses.push_back(PulseToDraw("SiL1_9_S", 0, 6));
  pulses.push_back(PulseToDraw("SiL1_9_S", 0, 7));
  pulses.push_back(PulseToDraw("SiL1_9_S", 0, 8));
  pulses.push_back(PulseToDraw("SiL1_9_S", 0, 9));
  pulses.push_back(PulseToDraw("SiL1_9_S", 0, 10));
  
  pulses.push_back(PulseToDraw("SiL1_9_S", 1, 0)); 
  pulses.push_back(PulseToDraw("SiL1_9_S", 1, 1)); 
  pulses.push_back(PulseToDraw("SiL1_9_S", 1, 2)); 
  pulses.push_back(PulseToDraw("SiL1_9_S", 1, 3)); 
  pulses.push_back(PulseToDraw("SiL1_9_S", 1, 4)); 
  pulses.push_back(PulseToDraw("SiL1_9_S", 1, 5)); 
  pulses.push_back(PulseToDraw("SiL1_9_S", 1, 6)); 
  pulses.push_back(PulseToDraw("SiL1_9_S", 1, 7)); 
  pulses.push_back(PulseToDraw("SiL1_9_S", 1, 8)); 
  pulses.push_back(PulseToDraw("SiL1_9_S", 1, 9));
  pulses.push_back(PulseToDraw("SiL1_9_S", 1, 10 ));
  pulses.push_back(PulseToDraw("SiL1_9_S", 1, 11));
  pulses.push_back(PulseToDraw("SiL1_9_S", 1, 12));
  
  pulses.push_back(PulseToDraw("SiL1_9_S", 2, 0)); 
  pulses.push_back(PulseToDraw("SiL1_9_S", 2, 1));
  pulses.push_back(PulseToDraw("SiL1_9_S", 2, 2));
  pulses.push_back(PulseToDraw("SiL1_9_S", 2, 3));
  pulses.push_back(PulseToDraw("SiL1_9_S", 2, 4));
  pulses.push_back(PulseToDraw("SiL1_9_S", 2, 5));
  pulses.push_back(PulseToDraw("SiL1_9_S", 2, 6));
  pulses.push_back(PulseToDraw("SiL1_9_S", 2, 7));
  pulses.push_back(PulseToDraw("SiL1_9_S", 2, 8));
  pulses.push_back(PulseToDraw("SiL1_9_S", 2, 9));
  pulses.push_back(PulseToDraw("SiL1_9_S", 2, 10));
  pulses.push_back(PulseToDraw("SiL1_9_S", 2, 11));
  pulses.push_back(PulseToDraw("SiL1_9_S", 2, 12));
  pulses.push_back(PulseToDraw("SiL1_9_S", 2, 13));
  pulses.push_back(PulseToDraw("SiL1_9_S", 2, 14));

  pulses.push_back(PulseToDraw("SiL1_9_S", 3, 0)); 
  pulses.push_back(PulseToDraw("SiL1_9_S", 3, 1)); 
  pulses.push_back(PulseToDraw("SiL1_9_S", 3, 2)); 
  pulses.push_back(PulseToDraw("SiL1_9_S", 3, 3)); 
  pulses.push_back(PulseToDraw("SiL1_9_S", 3, 4)); 
  pulses.push_back(PulseToDraw("SiL1_9_S", 3, 5)); 
  pulses.push_back(PulseToDraw("SiL1_9_S", 3, 6)); 
  pulses.push_back(PulseToDraw("SiL1_9_S", 3, 7)); 
  pulses.push_back(PulseToDraw("SiL1_9_S", 3, 8)); 
  pulses.push_back(PulseToDraw("SiL1_9_S", 3, 9));
  pulses.push_back(PulseToDraw("SiL1_9_S", 3, 10));
  pulses.push_back(PulseToDraw("SiL1_9_S", 3, 11));
  pulses.push_back(PulseToDraw("SiL1_9_S", 3, 12));
  
  pulses.push_back(PulseToDraw("SiL1_9_S", 4, 0)); 
  pulses.push_back(PulseToDraw("SiL1_9_S", 4, 1)); 
  pulses.push_back(PulseToDraw("SiL1_9_S", 4, 2)); 
  pulses.push_back(PulseToDraw("SiL1_9_S", 4, 3)); 
  pulses.push_back(PulseToDraw("SiL1_9_S", 4, 4)); 
  pulses.push_back(PulseToDraw("SiL1_9_S", 4, 5)); 
  pulses.push_back(PulseToDraw("SiL1_9_S", 4, 6)); 
  pulses.push_back(PulseToDraw("SiL1_9_S", 4, 7));
  
  pulses.push_back(PulseToDraw("SiL1_9_S", 5, 0)); 
  pulses.push_back(PulseToDraw("SiL1_9_S", 5, 1)); 
  pulses.push_back(PulseToDraw("SiL1_9_S", 5, 2)); 
  pulses.push_back(PulseToDraw("SiL1_9_S", 5, 3)); 
  pulses.push_back(PulseToDraw("SiL1_9_S", 5, 4)); 
  pulses.push_back(PulseToDraw("SiL1_9_S", 5, 5)); 
  pulses.push_back(PulseToDraw("SiL1_9_S", 5, 6)); 
  pulses.push_back(PulseToDraw("SiL1_9_S", 5, 7)); 
  pulses.push_back(PulseToDraw("SiL1_9_S", 5, 8)); 
  pulses.push_back(PulseToDraw("SiL1_9_S", 5, 9));
  pulses.push_back(PulseToDraw("SiL1_9_S", 5, 10));
  pulses.push_back(PulseToDraw("SiL1_9_S", 5, 11));
  pulses.push_back(PulseToDraw("SiL1_9_S", 5, 12));
  pulses.push_back(PulseToDraw("SiL1_9_S", 5, 13));
  pulses.push_back(PulseToDraw("SiL1_9_S", 5, 14));
  pulses.push_back(PulseToDraw("SiL1_9_S", 5, 15));
  
  /*
  pulses.push_back(PulseToDraw("SiL1_9_S", 6, 0));
  pulses.push_back(PulseToDraw("SiL1_9_S", 6, 1)); 
  pulses.push_back(PulseToDraw("SiL1_9_S", 6, 2)); 
  pulses.push_back(PulseToDraw("SiL1_9_S", 6, 3)); 
  pulses.push_back(PulseToDraw("SiL1_9_S", 6, 4)); 
  pulses.push_back(PulseToDraw("SiL1_9_S", 6, 5)); 
  pulses.push_back(PulseToDraw("SiL1_9_S", 6, 6)); 
  pulses.push_back(PulseToDraw("SiL1_9_S", 6, 7)); 
  pulses.push_back(PulseToDraw("SiL1_9_S", 6, 8)); 
  pulses.push_back(PulseToDraw("SiL1_9_S", 6, 9));
  pulses.push_back(PulseToDraw("SiL1_9_S", 6, 10));
  
  pulses.push_back(PulseToDraw("SiL1_9_S", 7, 0)); 
  pulses.push_back(PulseToDraw("SiL1_9_S", 7, 1)); 
  pulses.push_back(PulseToDraw("SiL1_9_S", 7, 2)); 
  pulses.push_back(PulseToDraw("SiL1_9_S", 7, 3)); 
  pulses.push_back(PulseToDraw("SiL1_9_S", 7, 4)); 
  pulses.push_back(PulseToDraw("SiL1_9_S", 7, 5)); 
  pulses.push_back(PulseToDraw("SiL1_9_S", 7, 6)); 
  pulses.push_back(PulseToDraw("SiL1_9_S", 7, 7)); 
  pulses.push_back(PulseToDraw("SiL1_9_S", 7, 8)); 
  pulses.push_back(PulseToDraw("SiL1_9_S", 7, 9));
  pulses.push_back(PulseToDraw("SiL1_9_S", 7, 10));
  pulses.push_back(PulseToDraw("SiL1_9_S", 7, 11));
  pulses.push_back(PulseToDraw("SiL1_9_S", 7, 12));
  pulses.push_back(PulseToDraw("SiL1_9_S", 7, 13));
  pulses.push_back(PulseToDraw("SiL1_9_S", 7, 14));
  pulses.push_back(PulseToDraw("SiL1_9_S", 7, 15));
  pulses.push_back(PulseToDraw("SiL1_9_S", 7, 16));
  pulses.push_back(PulseToDraw("SiL1_9_S", 7, 17));
  */
}
