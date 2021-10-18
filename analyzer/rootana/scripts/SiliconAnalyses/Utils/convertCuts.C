void convertCuts() {

  //  std::string filename = "~/data/results/Si16b/mark-al50-cuts.root";
  //  std::string filename = "~/data/results/Si16b/mark-al100-cuts.root";
  //  std::string filename = "~/data/results/Si16b/mark-al50-sir3-cuts.root";
  //  std::string filename = "~/data/results/Si16b/mark-si16b-cuts.root";
  std::string filename = "~/data/results/Si16b/si16b-cuts.root";
  TFile* file = new TFile(filename.c_str(), "READ");

  //  std::string outfilename = "~/data/results/Si16b/mark-al50-cuts-keV.root";
  //  std::string outfilename = "~/data/results/Si16b/mark-al100-cuts-keV.root";
  //  std::string outfilename = "~/data/results/Si16b/mark-al50-sir3-cuts-keV.root";
  //  std::string outfilename = "~/data/results/Si16b/mark-si16b-cuts-keV.root";
  std::string outfilename = "~/data/results/Si16b/si16b-cuts-cutoff.root";
  TFile* outfile = new TFile(outfilename.c_str(), "RECREATE");

  TIter next(file->GetListOfKeys());
  TKey *key;
  double pdt_cutoff = 17.4;
  double alpha_cutoff = 19.5;
  double alpha_cutoff_min = 9.0;

  while ((key = (TKey*)next())) {
    TClass *cl = gROOT->GetClass(key->GetClassName());
    if (!cl->InheritsFrom("TCutG")) { continue; }
    TCutG *cut = (TCutG*)key->ReadObj();

    std::string new_name = cut->GetName();
    //    new_name += "_keV";
    new_name += "_cutoff";
    std::cout << new_name << std::endl;
    double prev_x = -1;
    double y_high_to_set_proton = -1;
    double y_low_to_set_proton = -1;
    double y_high_to_set_deuteron = -1;
    double y_low_to_set_deuteron = -1;
    double y_high_to_set_triton = -1;
    double y_low_to_set_triton = -1;
    double y_high_to_set_alpha = -1;
    double y_low_to_set_alpha = -1;
    bool on_bottom_line = true; // we start going anticlock-wise from the left
    // Go through all the points and find the y-values needed for the cutoff
    for (int i_point = 0; i_point < cut->GetN(); ++i_point) {
      double x, y;
      cut->GetPoint(i_point, x, y);
      if (new_name.find("proton") != std::string::npos) {
	if (x > pdt_cutoff) { // MeV
	  if (on_bottom_line && y_low_to_set_proton<0) {
	    y_low_to_set_proton = y;
	  }
	}
      }
      if (new_name.find("deuteron") != std::string::npos) {
	std::cout << "x, y = " << x << ", " << y << " (prev_x = " << prev_x << ")" << std::endl;
	if (x > pdt_cutoff) { // MeV
	  if (on_bottom_line && y_low_to_set_deuteron<0) {
	    y_low_to_set_deuteron = y;
	    std::cout << "Set Y Low = " << y_low_to_set_deuteron << std::endl;
	  }
	}
      }
      if (new_name.find("triton") != std::string::npos) {
	if (x > pdt_cutoff) { // MeV
	  if (on_bottom_line && y_low_to_set_triton<0) {
	    y_low_to_set_triton = y;
	  }
	}
      }
      if (new_name.find("alpha") != std::string::npos) {
	if (x > alpha_cutoff) { // MeV
	  if (on_bottom_line && y_low_to_set_alpha<0) {
	    y_low_to_set_alpha = y;
	  }
	}
      }
      
      if (x < prev_x) {
	if (new_name.find("proton") != std::string::npos && x>pdt_cutoff) {
	  on_bottom_line = false;
	  if (y_high_to_set_proton<0) {
	    y_high_to_set_proton = y;
	  }
	}
	if (new_name.find("deuteron") != std::string::npos && x>pdt_cutoff) {
	  on_bottom_line = false;
	  if (y_high_to_set_deuteron<0) {
	    y_high_to_set_deuteron = y;
	    std::cout << "Set Y High (deuteron) = " << y_high_to_set_deuteron << std::endl;
	  }
	}
	if (new_name.find("triton") != std::string::npos && x>pdt_cutoff) {
	  on_bottom_line = false;
	  if (y_high_to_set_triton<0) {
	    y_high_to_set_triton = y;
	  }
	}
	if (new_name.find("alpha") != std::string::npos && x>alpha_cutoff) {
	  on_bottom_line = false;
	  if (y_high_to_set_alpha<0) {
	    y_high_to_set_alpha = y;
	  }
	}
      }
      prev_x = x;
    }
    //    std::cout << "Set Y High (proton) = " << y_high_to_set_proton << ", Set Y Low = " << y_low_to_set_proton << std::endl;
    //    std::cout << "Set Y High (triton) = " << y_high_to_set_triton << ", Set Y Low = " << y_low_to_set_triton << std::endl;
    TCutG* new_cut = new TCutG(new_name.c_str(), cut->GetN());
    on_bottom_line = true;
    prev_x = -1;
    for (int i_point = 0; i_point < cut->GetN(); ++i_point) {
      double x, y;
      cut->GetPoint(i_point, x, y);
      
      if (new_name.find("proton") != std::string::npos) {
	if (x > pdt_cutoff) { // MeV
	  x = pdt_cutoff;
	  if (on_bottom_line) {
	    y = y_low_to_set_proton;
	  }
	  else {
	    y = y_high_to_set_proton;
	  }
	}
      }
      if (new_name.find("deuteron") != std::string::npos) {
	if (x > pdt_cutoff) { // MeV
	  x = pdt_cutoff;
	  if (on_bottom_line) {
	    y = y_low_to_set_deuteron;
	  }
	  else {
	    y = y_high_to_set_deuteron;
	  }
	}
      }
      if (new_name.find("triton") != std::string::npos) {
	if (x > pdt_cutoff) { // MeV
	  x = pdt_cutoff;
	  if (on_bottom_line) {
	    y = y_low_to_set_triton;
	  }
	  else {
	    y = y_high_to_set_triton;
	  }
	}
      }
      if (new_name.find("alpha") != std::string::npos) {
	if (x > alpha_cutoff) { // MeV
	  x = alpha_cutoff;
	  if (on_bottom_line) {
	    y = y_low_to_set_alpha;
	  }
	  else {
	    y = y_high_to_set_alpha;
	  }
	}
	if (x < alpha_cutoff_min) {
	  x = alpha_cutoff_min;
	}
      }

      // if (new_name.find("alpha") != std::string::npos) {
      // 	if (x > alpha_cutoff) {
      // 	  x = alpha_cutoff;
      // 	}
      // 	if (x < alpha_cutoff_min) {
      // 	  x = alpha_cutoff_min;
      // 	}
      // }
      if (x < prev_x) {
	if (new_name.find("proton") != std::string::npos && x>pdt_cutoff) {
	  on_bottom_line = false;
	}
	if (new_name.find("deuteron") != std::string::npos && x>pdt_cutoff) {
	  on_bottom_line = false;
	}
	if (new_name.find("triton") != std::string::npos && x>pdt_cutoff) {
	  on_bottom_line = false;
	}
	if (new_name.find("alpha") != std::string::npos && x>alpha_cutoff) {
	  on_bottom_line = false;
	}
      }
      prev_x = x;
      
      //      new_cut->SetPoint(i_point, x*1000, y*1000);
      new_cut->SetPoint(i_point, x, y);
    }
    new_cut->Write();
  }
  
  outfile->Write();
  outfile->Close();
}
