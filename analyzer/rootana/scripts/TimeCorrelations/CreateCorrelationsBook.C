#include <algorithm>

void CreateCorrelationsBook() {
  std::fstream tex_file("main.tex", fstream::out);

  tex_file << "\\documentclass[smaller]{beamer}" << std::endl;
  tex_file << "% This is the file main.tex" << std::endl;
  tex_file << "%\\usetheme{Berkeley}" << std::endl;
  tex_file << "\\setbeamertemplate{footline}[frame number]" << std::endl;
  tex_file << "\\usepackage{wasysym}" << std::endl;
  tex_file << "\\usepackage{multirow}" << std::endl;
  tex_file << "\\title{Run 10404 (Si16b) TDiff Plots (with Rough Time Cut)}" << std::endl;

  tex_file << "\\begin{document}" << std::endl;
  tex_file << "\\begin{frame}" << std::endl;
  tex_file << "\\titlepage" << std::endl;
  tex_file << "\\end{frame}" << std::endl;
  tex_file << std::endl;
  tex_file << "\\section{Correlations}" << std::endl;
  //  tex_file << "\\begin{frame}" << std::endl;
  //  tex_file << "  \\frametitle{Overview}" << std::endl;
  //  tex_file << "  Here are some correlations" << std::endl;
  //  tex_file << "\\end{frame}" << std::endl << std::endl;

  std::string filename = "out10404_correlations-w-rough-time-cut.root";

  TFile* file = new TFile(filename.c_str(), "READ");

  TCanvas* c1 = new TCanvas("c1", "c1");

  TDirectory* tdiff_dir = file->GetDirectory("PlotTAP_TDiff");
  TList* tdiff_folders = tdiff_dir->GetListOfKeys();
  int n_max_folders = tdiff_folders->GetSize();

  TIter next_folder(tdiff_folders);
  TDirectory* folder;
  while ((folder = (TDirectory*)next_folder() ) ) {

    std::string foldername = "PlotTAP_TDiff/";
    foldername += folder->GetName();
    TDirectory* actual_folder = file->GetDirectory(foldername.c_str());
    TList* tdiff_objs = actual_folder->GetListOfKeys();
    //    std::cout << "AE: " << tdiff_objs << std::endl;
    TIter next_obj(tdiff_objs);
    TObject* obj;
    while ((obj = (TObject*)next_obj() ) ) {
      std::string objname = obj->GetName();
      // only want the 2D histogram and the 1D histogram

      if (! (objname.find("AmpB") != std::string::npos || objname.find("TimeA") != std::string::npos
	     || objname.find("TimeB") != std::string::npos || objname.find("export") != std::string::npos)) {
	
	std::string histname = foldername + "/" + objname;
	if (objname.find("AmpA") != std::string::npos) {
	  TH2F* hist2d = (TH2F*) file->Get(histname.c_str());
	  c1->SetLogz(1);
	  hist2d->Draw("COLZ");
	}
	else {
	  TH1F* hist1d = (TH1F*) file->Get(histname.c_str());
	  c1->SetLogz(0);
	  hist1d->Draw();
	}

	std::string canvasname = "figs/"+objname+".pdf";

	size_t start_pos = 0;
	std::string to = "-";
	std::string from = "#";
	while((start_pos = canvasname.find(from, start_pos)) != std::string::npos) {
	  canvasname.replace(start_pos, from.length(), to);
	  start_pos += to.length(); // Handles case where 'to' is a substring of 'from'
	}
	start_pos = 0;
	from = "{";
	while((start_pos = canvasname.find(from, start_pos)) != std::string::npos) {
	  canvasname.replace(start_pos, from.length(), to);
	  start_pos += to.length(); // Handles case where 'to' is a substring of 'from'
	}
	start_pos = 0;
	from = "}";
	while((start_pos = canvasname.find(from, start_pos)) != std::string::npos) {
	  canvasname.replace(start_pos, from.length(), to);
	  start_pos += to.length(); // Handles case where 'to' is a substring of 'from'
	}
	start_pos = 0;
	from = "_";
	while((start_pos = canvasname.find(from, start_pos)) != std::string::npos) {
	  canvasname.replace(start_pos, from.length(), to);
	  start_pos += to.length(); // Handles case where 'to' is a substring of 'from'
	}
	start_pos = 0;
	from = ".2";
	to = "-2";
	while((start_pos = canvasname.find(from, start_pos)) != std::string::npos) {
	  canvasname.replace(start_pos, from.length(), to);
	  start_pos += to.length(); // Handles case where 'to' is a substring of 'from'
	}

	c1->SaveAs(canvasname.c_str());

	tex_file << "\\begin{frame}" << std::endl;
	tex_file << "  \\begin{center}" << std::endl;

	tex_file << "    \\includegraphics[width=1.0\\textwidth]{" << canvasname << "}" << std::endl;
	tex_file << "  \\end{center}" << std::endl;
	tex_file << "\\end{frame}" << std::endl << std::endl;
      }
    }
  }
  tex_file << "\\end{document}";
}
