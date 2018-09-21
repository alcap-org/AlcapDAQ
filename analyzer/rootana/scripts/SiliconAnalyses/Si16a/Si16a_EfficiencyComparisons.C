#include "TH1.h"
#include "TLine.h"
#include "TBox.h"
#include "TLatex.h"
#include "TCanvas.h"

#include <sstream>

struct EffPoint {
  EffPoint() {};
  EffPoint(std::string label, double eff, double eff_err) : label(label), eff(eff), eff_err(eff_err) {};
  
  std::string label;
  double eff;
  double eff_err;
};

struct RunBlock {
  RunBlock(std::string label, double start_element, double end_element, std::vector<EffPoint> eff_points) : label(label), start_element(start_element), end_element(end_element), eff_points(eff_points) {}

  std::string label;
  double start_element;
  double end_element;
  std::vector<EffPoint> eff_points;
};

void Si16a_EfficiencyComparisons() {

  const int n_ref_efficiencies = 2;
  std::string ref_labels[n_ref_efficiencies] = {" run 9302", " run 10139"};
  //  double ref_eff_val[n_ref_efficiencies] = {7.4e-4, 6.7e-4};
  //  double ref_eff_err[n_ref_efficiencies] = {0.1e-4, 0.3e-4};
  double ref_eff_val[n_ref_efficiencies] = {6.9e-4, 6.7e-4};
  double ref_eff_err[n_ref_efficiencies] = {0.1e-4, 0.1e-4};
  Int_t colours[n_ref_efficiencies] = {kRed, kBlue};
  
  //  std::string xray_count_tags[n_counts] = {"Full XRay Spectrum", "XRay Spectrum w/ SiL3 Hit in TME"};
  //  std::string target_count_tags[n_counts] = {"-200 < t < 200 ns && 3000 < E < 600 keV", "-200 < t < 200 ns && 3900 < E < 4000 keV"};
  std::vector<EffPoint> eff_points;
  std::vector<RunBlock> run_blocks;
  double start_element = eff_points.size();
  //  eff_points.push_back(EffPoint("SiL3 (All)", 7.73e-4, 0.11e-4));
  eff_points.push_back(EffPoint("2000 -- 2100 keV", 7.49e-4, 3.70e-4));
  eff_points.push_back(EffPoint("2100 -- 2200 keV", 4.38e-4, 1.84e-4));
  eff_points.push_back(EffPoint("2200 -- 2300 keV", 3.97e-4, 1.59e-4));
  eff_points.push_back(EffPoint("2600 -- 2700 keV", 7.48e-4, 2.89e-4));
  eff_points.push_back(EffPoint("2700 -- 2800 keV", 6.40e-4, 1.92e-4));
  eff_points.push_back(EffPoint("2800 -- 2900 keV", 6.23e-4, 1.05e-4));
  eff_points.push_back(EffPoint("2900 -- 3000 keV", 5.14e-4, 1.52e-4));
  eff_points.push_back(EffPoint("3000 -- 3100 keV", 4.55e-4, 1.29e-4));
  eff_points.push_back(EffPoint("3100 -- 3200 keV", 6.64e-4, 1.32e-4));
  eff_points.push_back(EffPoint("3200 -- 3300 keV", 4.80e-4, 0.77e-4));
  eff_points.push_back(EffPoint("3300 -- 3400 keV", 5.98e-4, 0.65e-4));
  eff_points.push_back(EffPoint("3400 -- 3500 keV", 5.91e-4, 0.50e-4));
  eff_points.push_back(EffPoint("3500 -- 3600 keV", 5.11e-4, 0.32e-4));
  eff_points.push_back(EffPoint("3600 -- 3700 keV", 5.63e-4, 0.26e-4));
  eff_points.push_back(EffPoint("3700 -- 3800 keV", 6.42e-4, 0.22e-4));
  eff_points.push_back(EffPoint("3800 -- 3900 keV", 5.67e-4, 0.18e-4));
  eff_points.push_back(EffPoint("3900 -- 4000 keV", 5.93e-4, 0.18e-4));
  eff_points.push_back(EffPoint("4000 -- 4100 keV", 5.80e-4, 0.20e-4));
  eff_points.push_back(EffPoint("4100 -- 4200 keV", 6.03e-4, 0.23e-4));
  eff_points.push_back(EffPoint("4200 -- 4300 keV", 5.55e-4, 0.29e-4));
  eff_points.push_back(EffPoint("4300 -- 4400 keV", 5.22e-4, 0.34e-4));
  eff_points.push_back(EffPoint("4400 -- 4500 keV", 5.80e-4, 0.46e-4));
  eff_points.push_back(EffPoint("4500 -- 4600 keV", 4.22e-4, 0.45e-4));
  eff_points.push_back(EffPoint("4600 -- 4700 keV", 4.48e-4, 0.55e-4));
  eff_points.push_back(EffPoint("4700 -- 4800 keV", 6.17e-4, 0.75e-4));
  eff_points.push_back(EffPoint("4800 -- 4900 keV", 6.27e-4, 0.84e-4));
  eff_points.push_back(EffPoint("4900 -- 5000 keV", 5.55e-4, 0.80e-4));
  eff_points.push_back(EffPoint("5000 -- 5100 keV", 5.27e-4, 0.97e-4));
  eff_points.push_back(EffPoint("5100 -- 5200 keV", 4.92e-4, 0.95e-4));
  eff_points.push_back(EffPoint("5200 -- 5300 keV", 7.78e-4, 0.77e-4));
  eff_points.push_back(EffPoint("5300 -- 5400 keV", 8.70e-4, 0.99e-4));
  eff_points.push_back(EffPoint("5400 -- 5500 keV", 6.63e-4, 1.34e-4));
  //eff_points.push_back(EffPoint("5500 -- 5600 keV", 5.66e-4, 0.06e-4));
  eff_points.push_back(EffPoint("5600 -- 5700 keV", 5.10e-4, 1.21e-4));
  eff_points.push_back(EffPoint("5700 -- 5800 keV", 5.98e-4, 1.40e-4));
  eff_points.push_back(EffPoint("5800 -- 5900 keV", 5.19e-4, 1.13e-4));
  eff_points.push_back(EffPoint("5900 -- 6000 keV", 5.64e-4, 1.44e-4));
  eff_points.push_back(EffPoint("3000 -- 6000 keV", 5.88e-4, 0.09e-4));
  double end_element = eff_points.size();
  run_blocks.push_back(RunBlock("SiL3 Dataset", start_element, end_element, eff_points));

  eff_points.clear();
  start_element = eff_points.size();
  eff_points.push_back(EffPoint("SiL1 (0 -- 400 keV))", 3.93e-4, 2.12e-4));
  eff_points.push_back(EffPoint("SiL1 (400 -- 800 keV))", 6.29e-4, 1.73e-4));
  end_element = eff_points.size();
  run_blocks.push_back(RunBlock("Run 9737, SF = 0.98", start_element, end_element, eff_points));

  eff_points.clear();
  start_element = eff_points.size();
  eff_points.push_back(EffPoint("SiL1 (0 -- 400 keV))", 9.59e-4, 2.96e-4));
  eff_points.push_back(EffPoint("SiL1 (400 -- 800 keV))", 9.80e-4, 1.41e-4));
  eff_points.push_back(EffPoint("SiL1 (800 -- 1200 keV))", 8.66e-4, 2.32e-4));
  end_element = eff_points.size();
  run_blocks.push_back(RunBlock("Run 9736, SF = 0.99", start_element, end_element, eff_points));
  
  eff_points.clear();
  start_element = eff_points.size();
  eff_points.push_back(EffPoint("SiL1 (0 -- 400 keV))", 5.95e-4, 2.10e-4));
  eff_points.push_back(EffPoint("SiL3 (0 -- 400 keV))", 5.52e-4, 3.12e-4));
  eff_points.push_back(EffPoint("SiL1 (400 -- 800 keV))", 8.11e-4, 0.93e-4));
  eff_points.push_back(EffPoint("SiL3 (400 -- 800 keV))", 10.32e-4, 0.14e-4));
  eff_points.push_back(EffPoint("SiL1 (800 -- 1200 keV))", 5.47e-4, 0.89e-4));
  eff_points.push_back(EffPoint("SiL1 (1200 -- 1600 keV))", 6.55e-4, 3.17e-4));
  end_element = eff_points.size();
  run_blocks.push_back(RunBlock("Run 9739, SF = 1.01", start_element, end_element, eff_points));
  
  eff_points.clear();
  start_element = eff_points.size();
  eff_points.push_back(EffPoint("SiL1 (0 -- 400 keV))", 9.57e-4, 3.70e-4));
  eff_points.push_back(EffPoint("SiL3 (0 -- 400 keV))", 6.36e-4, 2.70e-4));
  eff_points.push_back(EffPoint("SiL1 (400 -- 800 keV))", 5.36e-4, 0.81e-4));
  eff_points.push_back(EffPoint("SiL3 (400 -- 800 keV))", 5.00e-4, 1.77e-4));
  eff_points.push_back(EffPoint("SiL1 (800 -- 1200 keV))", 7.45e-4, 0.93e-4));
  eff_points.push_back(EffPoint("SiL1 (1200 -- 1600 keV))", 6.58e-4, 3.08e-4));
  end_element = eff_points.size();
  run_blocks.push_back(RunBlock("Run 9740, SF = 1.02", start_element, end_element, eff_points));
  
  eff_points.clear();
  start_element = eff_points.size();
  eff_points.push_back(EffPoint("SiL3 (0 -- 400 keV))", 8.15e-4, 2.52e-4));
  eff_points.push_back(EffPoint("SiL1 (400 -- 800 keV))", 5.74e-4, 0.87e-4));
  eff_points.push_back(EffPoint("SiL3 (400 -- 800 keV))", 4.95e-4, 1.41e-4));
  eff_points.push_back(EffPoint("SiL1 (800 -- 1200 keV))", 5.94e-4, 0.75e-4));
  eff_points.push_back(EffPoint("SiL3 (800 -- 1200 keV))", 8.05e-4, 2.24e-4));
  eff_points.push_back(EffPoint("SiL1 (1200 -- 1600 keV))", 7.86e-4, 3.22e-4));
  end_element = eff_points.size();
  run_blocks.push_back(RunBlock("Run 9741, SF = 1.03", start_element, end_element, eff_points));
  
  eff_points.clear();
  start_element = eff_points.size();
  eff_points.push_back(EffPoint("SiL3 (0 -- 400 keV))", 4.23e-4, 1.70e-4));
  eff_points.push_back(EffPoint("SiL1 (400 -- 800 keV))", 8.18e-4, 0.99e-4));
  eff_points.push_back(EffPoint("SiL3 (400 -- 800 keV))", 7.13e-4, 1.37e-4));
  eff_points.push_back(EffPoint("SiL1 (800 -- 1200 keV))", 7.29e-4, 0.80e-4));
  eff_points.push_back(EffPoint("SiL3 (800 -- 1200 keV))", 7.08e-4, 1.58e-4));
  end_element = eff_points.size();
  run_blocks.push_back(RunBlock("Run 9735, SF = 1.035", start_element, end_element, eff_points));
  
  eff_points.clear();
  start_element = eff_points.size();
  eff_points.push_back(EffPoint("SiL1 (400 -- 800 keV))", 4.81e-4, 0.77e-4));
  eff_points.push_back(EffPoint("SiL3 (400 -- 800 keV))", 5.17e-4, 1.04e-4));
  eff_points.push_back(EffPoint("SiL1 (800 -- 1200 keV))", 5.51e-4, 0.68e-4));
  eff_points.push_back(EffPoint("SiL3 (800 -- 1200 keV))", 5.49e-4, 1.20e-4));
  eff_points.push_back(EffPoint("SiL1 (1200 -- 1600 keV))", 5.48e-4, 2.62e-4));
  eff_points.push_back(EffPoint("SiL3 (1200 -- 1600 keV))", 8.24e-4, 2.80e-4));
  eff_points.push_back(EffPoint("SiL3 (500 -- 1500 keV))", 6.13e-4, 0.70e-4));
  end_element = eff_points.size();
  run_blocks.push_back(RunBlock("Run 9742, SF = 1.04", start_element, end_element, eff_points));
  
  eff_points.clear();
  start_element = eff_points.size();
  eff_points.push_back(EffPoint("SiL3 (0 -- 400 keV))", 6.00e-4, 1.78e-4));
  eff_points.push_back(EffPoint("SiL1 (400 -- 800 keV))", 5.01e-4, 0.71e-4));
  eff_points.push_back(EffPoint("SiL3 (400 -- 800 keV))", 6.17e-4, 1.02e-4));
  eff_points.push_back(EffPoint("SiL1 (800 -- 1200 keV))", 5.76e-4, 0.76e-4));
  eff_points.push_back(EffPoint("SiL3 (800 -- 1200 keV))", 6.40e-4, 0.99e-4));
  eff_points.push_back(EffPoint("SiL1 (1200 -- 1600 keV))", 7.84e-4, 2.86e-4));
  eff_points.push_back(EffPoint("SiL3 (1200 -- 1600 keV))", 6.62e-4, 1.62e-4));
  eff_points.push_back(EffPoint("SiL3 (500 -- 1500 keV))", 6.13e-4, 0.66e-4));
  end_element = eff_points.size();
  run_blocks.push_back(RunBlock("Run 9743, SF = 1.05", start_element, end_element, eff_points));
  
  //eff_points.push_back(EffPoint("Run 9736 (SF = 0.99)", 5.47e-4, 2.06e-4));
  //eff_points.push_back(EffPoint("Run 9739 (SF = 1.01)", 5.61e-4, 0.59e-4));
  //eff_points.push_back(EffPoint("Run 9740 (SF = 1.02)", 6.72e-4, 0.76e-4));
  //eff_points.push_back(EffPoint("Run 9741 (SF = 1.03)", 6.12e-4, 0.81e-4));
  //eff_points.push_back(EffPoint("Run 9735 (SF = 1.035)", 6.93e-4, 0.50e-4));
  //eff_points.push_back(EffPoint("Run 9742 (SF = 1.04)", 6.14e-4, 0.77e-4));
  //eff_points.push_back(EffPoint("Run 9743 (SF = 1.05)", 5.12e-4, 0.80e-4));
  //  double si16a_run_end_element = eff_points.size();

  double min_axis = 0;
  double max_axis = 12e-4;
  std::stringstream canvasname;
  for (std::vector<RunBlock>::const_iterator i_block = run_blocks.begin(); i_block != run_blocks.end(); ++i_block) {

    canvasname.str("");
    canvasname << "c_" << i_block - run_blocks.begin();
    TCanvas* c = new TCanvas(canvasname.str().c_str(), canvasname.str().c_str());
    
    int n_efficiencies = i_block->eff_points.size();
    
    TH1F* hEfficiencies = new TH1F("hEfficiencies", i_block->label.c_str(), n_efficiencies,0,n_efficiencies);

    for (std::vector<EffPoint>::const_iterator i_eff = i_block->eff_points.begin(); i_eff != i_block->eff_points.end(); ++i_eff) {
      int i_bin = i_eff - i_block->eff_points.begin() + 1;
      hEfficiencies->GetXaxis()->SetBinLabel(i_bin, i_eff->label.c_str());
      hEfficiencies->SetBinContent(i_bin, i_eff->eff);
      hEfficiencies->SetBinError(i_bin, i_eff->eff_err);
    }
    hEfficiencies->GetYaxis()->SetRangeUser(min_axis, max_axis);
    hEfficiencies->SetStats(false);
    hEfficiencies->SetLineColor(kBlack);
    hEfficiencies->Draw("HIST E");


    TLine* start_line = new TLine(i_block->start_element, min_axis, i_block->start_element, max_axis);
    start_line->Draw("LSAME");

    TLine* end_line = new TLine(i_block->end_element, min_axis, i_block->end_element, max_axis);
    end_line->Draw("LSAME");

    TLatex* latex = new TLatex();
    latex->SetTextAlign(22);
    latex->SetTextSize(0.035);
    double latex_x = i_block->start_element + (i_block->end_element - i_block->start_element)/2;
    double latex_y = 9.75e-4;
    //    latex->DrawLatex(latex_x, latex_y, i_block->label.c_str());

    for (int i_ref_eff = 0; i_ref_eff < n_ref_efficiencies; ++i_ref_eff) {
      Int_t i_colour = colours[i_ref_eff];
      
      TBox* ref_eff_box = new TBox(0, ref_eff_val[i_ref_eff]-ref_eff_err[i_ref_eff], n_efficiencies, ref_eff_val[i_ref_eff]+ref_eff_err[i_ref_eff]);
      ref_eff_box->SetFillStyle(3004);
      ref_eff_box->SetFillColor(i_colour);
      ref_eff_box->SetLineColor(i_colour);
      ref_eff_box->Draw("LSAME");
      
      TLine* ref_eff_line = new TLine(0, ref_eff_val[i_ref_eff], n_efficiencies, ref_eff_val[i_ref_eff]);
      ref_eff_line->SetLineColor(i_colour);
      ref_eff_line->SetLineWidth(2);
      ref_eff_line->Draw("LSAME");
      
      TLatex* latex = new TLatex();
      double latex_x = n_efficiencies;
      double latex_y = ref_eff_val[i_ref_eff];//+ref_eff_err[i_ref_eff];
      latex->SetTextAlign(12);
      latex->SetTextColor(i_colour);
      latex->DrawLatex(latex_x, latex_y, ref_labels[i_ref_eff].c_str());
    }

    std::string pdfname = "~/plots/2018-05-08/AlCapData_EffComparison_" + canvasname.str() + ".pdf";
    c->SaveAs(pdfname.c_str());
  }
}
