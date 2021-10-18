//#include "scripts/ExportPulse/DrawTPIAndTemplateTAPs_fromPulseViewer.C"
#include "DrawTPIAndTemplateTAPs_fromPulseViewer.C"
#include "DrawTPIsAndTAPs_fromPulseViewer.C"

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

  //  int run_number = 9040;
  int run_number = 10404;
  //  int run_number = 9041;

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
  //  filename << "/home/edmonds/data/out/local/out" << std::setw(5) << std::setfill('0') << run_number << "_template-fits_export-pulse_3.root";
  //  filename << "/home/edmonds/data/out/local/out" << std::setw(5) << std::setfill('0') << run_number << "_template-fits_export-pulse_4.root";
  //  filename << "/home/edmonds/data/out/local/out" << std::setw(5) << std::setfill('0') << run_number << "_template-fits_export-pulse_5.root";
  //  filename << "/home/edmonds/data/out/local/out" << std::setw(5) << std::setfill('0') << run_number << "_template-fits_export-pulse_single-pulse.root"; // for paper (templates)
  //  filename << "/home/edmonds/data/out/local/out" << std::setw(5) << std::setfill('0') << run_number << "_template-fits_export-pulse_refine-factor-2.root";
  filename << "/home/edmonds/data/out/local/out" << std::setw(5) << std::setfill('0') << run_number << "_paper_export-pulse.root";
  TFile* file = new TFile(filename.str().c_str(), "READ");
  if (file->IsZombie()) {
    std::cout << "Problem opening file " << filename << std::endl;
  }

  std::vector<PulseToDraw> pulses_to_draw;
  FillPulsesToDraw(pulses_to_draw);
  std::stringstream histname, histtitle;

  for (std::vector<PulseToDraw>::const_iterator i_pulse = pulses_to_draw.begin(); i_pulse != pulses_to_draw.end(); ++i_pulse) {
    histname.str("");
    histname << "Pulse_" << i_pulse->detname << "_" << i_pulse->event_number << "_" << i_pulse->pulse_number;

    histtitle.str("");
    histtitle << "SiL3 Dataset, Detector: " << i_pulse->detname << ", Event: " << i_pulse->event_number << ", Pulse: " << i_pulse->pulse_number;
    TCanvas c1("c1", "c1", 1200, 800);
    //    if (DrawTPIAndTemplateTAPs_fromPulseViewer_file(file, histname.str(), c1, histtitle.str())) {
    if (DrawTPIsAndTAPs_fromPulseViewer_file(file, histname.str(), c1, histtitle.str())) {
      
      std::string pdfname = "figs/" + histname.str() + ".pdf";
      //      std::string savename = "picture-book/" + pdfname;
      std::string savename = "picture-book-paper/" + pdfname;
      c1.SetFillColor(kWhite);
      c1.SaveAs(savename.c_str());
      
      out_file << "\\begin{frame}" << std::endl;
      out_file << "\\begin{center}" << std::endl;
      out_file << "\\includegraphics[width=1.0\\textwidth]{" << pdfname << "}" << std::endl;
      out_file << std::endl << "Event " << i_pulse->event_number << " Pulse " << i_pulse->pulse_number << std::endl;
      out_file << "\\end{center}" << std::endl;
      out_file << "\\end{frame}" << std::endl << std::endl;
    }
  }
  out_file << "\\end{document}" << std::endl;
  out_file.close();
}

void FillPulsesToDraw(std::vector<PulseToDraw>& pulses) {

  //  pulses.push_back(PulseToDraw("SiL3_S", 0, 195)); // active target (for paper)
  pulses.push_back(PulseToDraw("SiR2_S", 1, 10)); // standard (for paper)
  // Interesting ones for SiL3 dataset (run 9040)
  // for (int i_event = 0; i_event < 20; ++i_event) {
  //   for (int i_pulse = 0; i_pulse < 1100; ++i_pulse) {
  //     pulses.push_back(PulseToDraw("SiL3_S", i_event, i_pulse));
  //     //    pulses.push_back(PulseToDraw("muSc", 0, i_pulse));
  //   }
  // }
  

  // Interesting one for Si16b datset (run 10404);
  /*  for (int i_pulse = 0; i_pulse <= 139; ++i_pulse) {
    pulses.push_back(PulseToDraw("SiT_1_S", 0, i_pulse));
  }
  for (int i_pulse = 0; i_pulse <= 78; ++i_pulse) {
    pulses.push_back(PulseToDraw("SiT_2_S", 0, i_pulse));
  }
  for (int i_pulse = 0; i_pulse <= 143; ++i_pulse) {
    pulses.push_back(PulseToDraw("SiT_3_S", 0, i_pulse));
  }
  for (int i_pulse = 0; i_pulse <= 235; ++i_pulse) {
    pulses.push_back(PulseToDraw("SiT_4_S", 0, i_pulse));
  }

  for (int i_pulse = 0; i_pulse <= 6; ++i_pulse) {
    pulses.push_back(PulseToDraw("SiL1_1_S", 0, i_pulse));
  }
  for (int i_pulse = 0; i_pulse <= 6; ++i_pulse) {
    pulses.push_back(PulseToDraw("SiL1_2_S", 0, i_pulse));
  }
  for (int i_pulse = 0; i_pulse <= 10; ++i_pulse) {
    pulses.push_back(PulseToDraw("SiL1_3_S", 0, i_pulse));
  }
  for (int i_pulse = 0; i_pulse <= 10; ++i_pulse) {
    pulses.push_back(PulseToDraw("SiL1_4_S", 0, i_pulse));
  }
  for (int i_pulse = 0; i_pulse <= 9; ++i_pulse) {
    pulses.push_back(PulseToDraw("SiL1_5_S", 0, i_pulse));
  }
  for (int i_pulse = 0; i_pulse <= 9; ++i_pulse) {
    pulses.push_back(PulseToDraw("SiL1_6_S", 0, i_pulse));
  }
  for (int i_pulse = 0; i_pulse <= 16; ++i_pulse) {
    pulses.push_back(PulseToDraw("SiL1_7_S", 0, i_pulse));
  }
  for (int i_pulse = 0; i_pulse <= 12; ++i_pulse) {
    pulses.push_back(PulseToDraw("SiL1_8_S", 0, i_pulse));
  }
  for (int i_pulse = 0; i_pulse <= 13; ++i_pulse) {
    pulses.push_back(PulseToDraw("SiL1_9_S", 0, i_pulse));
  }
  for (int i_pulse = 0; i_pulse <= 9; ++i_pulse) {
    pulses.push_back(PulseToDraw("SiL1_10_S", 0, i_pulse));
  }
  for (int i_pulse = 0; i_pulse <= 12; ++i_pulse) {
    pulses.push_back(PulseToDraw("SiL1_11_S", 0, i_pulse));
  }
  for (int i_pulse = 0; i_pulse <= 14; ++i_pulse) {
    pulses.push_back(PulseToDraw("SiL1_12_S", 0, i_pulse));
  }
  for (int i_pulse = 0; i_pulse <= 14; ++i_pulse) {
    pulses.push_back(PulseToDraw("SiL1_13_S", 0, i_pulse));
  }
  for (int i_pulse = 0; i_pulse <= 13; ++i_pulse) {
    pulses.push_back(PulseToDraw("SiL1_14_S", 0, i_pulse));
  }
  for (int i_pulse = 0; i_pulse <= 11; ++i_pulse) {
    pulses.push_back(PulseToDraw("SiL1_15_S", 0, i_pulse));
  }
  */

  /*  pulses.push_back(PulseToDraw("SiL3_S", 0, 64));
pulses.push_back(PulseToDraw("SiL3_S", 0, 199));
pulses.push_back(PulseToDraw("SiL3_S", 0, 335));
pulses.push_back(PulseToDraw("SiL3_S", 0, 433));
pulses.push_back(PulseToDraw("SiL3_S", 0, 504));
pulses.push_back(PulseToDraw("SiL3_S", 0, 598));
pulses.push_back(PulseToDraw("SiL3_S", 0, 682));
pulses.push_back(PulseToDraw("SiL3_S", 0, 714));
pulses.push_back(PulseToDraw("SiL3_S", 0, 795));
pulses.push_back(PulseToDraw("SiL3_S", 0, 795));
pulses.push_back(PulseToDraw("SiL3_S", 0, 843));
pulses.push_back(PulseToDraw("SiL3_S", 0, 882));

 pulses.push_back(PulseToDraw("SiL3_S", 1, 97));
pulses.push_back(PulseToDraw("SiL3_S", 1, 311));
pulses.push_back(PulseToDraw("SiL3_S", 1, 326));
pulses.push_back(PulseToDraw("SiL3_S", 1, 330));
pulses.push_back(PulseToDraw("SiL3_S", 1, 421));
pulses.push_back(PulseToDraw("SiL3_S", 1, 537));
pulses.push_back(PulseToDraw("SiL3_S", 2, 111));
pulses.push_back(PulseToDraw("SiL3_S", 2, 410));
pulses.push_back(PulseToDraw("SiL3_S", 2, 461));
pulses.push_back(PulseToDraw("SiL3_S", 2, 559));
pulses.push_back(PulseToDraw("SiL3_S", 3, 375));
pulses.push_back(PulseToDraw("SiL3_S", 3, 393));
pulses.push_back(PulseToDraw("SiL3_S", 4, 41));
pulses.push_back(PulseToDraw("SiL3_S", 4, 173));
pulses.push_back(PulseToDraw("SiL3_S", 4, 558));
pulses.push_back(PulseToDraw("SiL3_S", 4, 623));
pulses.push_back(PulseToDraw("SiL3_S", 4, 750));
pulses.push_back(PulseToDraw("SiL3_S", 4, 938));
  */

    /*  pulses.push_back(PulseToDraw("SiL3_S", 0, 61));
	pulses.push_back(PulseToDraw("SiL3_S", 0, 208));
	pulses.push_back(PulseToDraw("SiL3_S", 0, 241));
	pulses.push_back(PulseToDraw("SiL3_S", 0, 282));
	pulses.push_back(PulseToDraw("SiL3_S", 0, 325));
	pulses.push_back(PulseToDraw("SiL3_S", 0, 335));
	pulses.push_back(PulseToDraw("SiL3_S", 0, 345));
	pulses.push_back(PulseToDraw("SiL3_S", 0, 449));
	pulses.push_back(PulseToDraw("SiL3_S", 0, 462));
	pulses.push_back(PulseToDraw("SiL3_S", 0, 493));
	pulses.push_back(PulseToDraw("SiL3_S", 0, 649));
	pulses.push_back(PulseToDraw("SiL3_S", 0, 969));
    */
  /*
  pulses.push_back(PulseToDraw("SiL3_S", 0, 71));
  pulses.push_back(PulseToDraw("SiL3_S", 0, 72));
  pulses.push_back(PulseToDraw("SiL3_S", 0, 124));
  pulses.push_back(PulseToDraw("SiL3_S", 0, 125));
  pulses.push_back(PulseToDraw("SiL3_S", 0, 175));
  pulses.push_back(PulseToDraw("SiL3_S", 0, 176));
  pulses.push_back(PulseToDraw("SiL3_S", 0, 335));
  pulses.push_back(PulseToDraw("SiL3_S", 0, 336));
  pulses.push_back(PulseToDraw("SiL3_S", 0, 462));
  pulses.push_back(PulseToDraw("SiL3_S", 0, 463));
  pulses.push_back(PulseToDraw("SiL3_S", 0, 626));
  pulses.push_back(PulseToDraw("SiL3_S", 0, 624));
  pulses.push_back(PulseToDraw("SiL3_S", 0, 649));
  pulses.push_back(PulseToDraw("SiL3_S", 0, 650));
  pulses.push_back(PulseToDraw("SiL3_S", 0, 832));
  pulses.push_back(PulseToDraw("SiL3_S", 0, 833));
  pulses.push_back(PulseToDraw("SiL3_S", 0, 840));
  pulses.push_back(PulseToDraw("SiL3_S", 0, 841));
  pulses.push_back(PulseToDraw("SiL3_S", 0, 907));
  pulses.push_back(PulseToDraw("SiL3_S", 0, 908));
  pulses.push_back(PulseToDraw("SiL3_S", 0, 915));
  pulses.push_back(PulseToDraw("SiL3_S", 0, 916));
  pulses.push_back(PulseToDraw("SiL3_S", 0, 969));
  pulses.push_back(PulseToDraw("SiL3_S", 0, 970));
  */
}
