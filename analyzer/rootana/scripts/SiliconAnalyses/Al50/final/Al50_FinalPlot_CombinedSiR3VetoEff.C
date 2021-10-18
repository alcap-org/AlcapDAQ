struct Point {
  Point(std::string name, double minE, double maxE, double eff, double effErr) : name(name), minE(minE), maxE(maxE), eff(eff), effErr(effErr) {}

  std::string name;
  double minE;
  double maxE;
  double eff;
  double effErr;
};
void Al50_FinalPlot_CombinedSiR3VetoEff() {

  std::vector<Point> all_points;
  all_points.push_back(Point("john", 6, 7, 0.75, 0.03));
  all_points.push_back(Point("john", 7, 8, 0.77, 0.02));
  all_points.push_back(Point("john", 8, 9, 0.80, 0.02));
  all_points.push_back(Point("john", 9, 10, 0.78, 0.02));
  all_points.push_back(Point("john", 10, 11, 0.70, 0.02));
  all_points.push_back(Point("mark", 8, 9, 0.81, 0.02));
  all_points.push_back(Point("mark", 9, 10, 0.77, 0.02));
  all_points.push_back(Point("mark_mc1", 10, 11, 0.865, 0.006));
  all_points.push_back(Point("mark_mc2", 10, 11, 0.81, 0.01));
  all_points.push_back(Point("andy", 5, 6, 0.69, 0.05));
  all_points.push_back(Point("andy", 6, 7, 0.67, 0.03));
  all_points.push_back(Point("andy", 7, 8, 0.69, 0.04));
  all_points.push_back(Point("andy", 8, 9, 0.78, 0.03));
  all_points.push_back(Point("andy", 9, 10, 0.73, 0.03));

  TGraphErrors* gr_all = new TGraphErrors();
  TGraphErrors* gr_nomc = new TGraphErrors();
  TGraphErrors* gr_john = new TGraphErrors();
  TGraphErrors* gr_mark = new TGraphErrors();
  TGraphErrors* gr_andy = new TGraphErrors();
  for (const auto& i_point : all_points) {
    gr_all->SetPoint(gr_all->GetN(), i_point.minE, i_point.eff);
    *(gr_all->GetEY()+gr_all->GetN()-1) = (i_point.effErr);

    if (i_point.name.find("mc") == std::string::npos) {
      gr_nomc->SetPoint(gr_nomc->GetN(), i_point.minE, i_point.eff);
      *(gr_nomc->GetEY()+gr_nomc->GetN()-1) = (i_point.effErr);

      if (i_point.name.find("mark") != std::string::npos) {
	gr_mark->SetPoint(gr_mark->GetN(), i_point.minE, i_point.eff);
	*(gr_mark->GetEY()+gr_mark->GetN()-1) = (i_point.effErr);
      }

      if (i_point.name.find("john") != std::string::npos) {
	gr_john->SetPoint(gr_john->GetN(), i_point.minE, i_point.eff);
	*(gr_john->GetEY()+gr_john->GetN()-1) = (i_point.effErr);
      }

      if (i_point.name.find("andy") != std::string::npos) {
	gr_andy->SetPoint(gr_andy->GetN(), i_point.minE, i_point.eff);
	*(gr_andy->GetEY()+gr_andy->GetN()-1) = (i_point.effErr);
      }
    }
  }
  //  gr_all->Draw("A*E");
  gr_john->SetMarkerStyle(kOpenCircle);
  gr_john->SetMarkerColor(kRed);
  gr_john->SetLineColor(kRed);
  gr_john->Draw("APE");
  gr_john->SetTitle("All Measured SiR3 Veto Efficiencies");
  gr_john->GetXaxis()->SetTitle("Energy Bin [MeV]");
  gr_john->GetYaxis()->SetTitle("SiR3 Veto Efficiency");
  gr_john->GetYaxis()->SetRangeUser(0.55, 0.85);

  gr_mark->SetMarkerStyle(kOpenCircle);
  gr_mark->SetMarkerColor(kBlue);
  gr_mark->SetLineColor(kBlue);
  gr_mark->Draw("PE SAME");

  gr_andy->SetMarkerStyle(kOpenCircle);
  gr_andy->SetMarkerColor(kSpring);
  gr_andy->SetLineColor(kSpring);
  gr_andy->Draw("PE SAME");

  gr_nomc->SetMarkerColor(kWhite);
  gr_nomc->SetLineColor(kWhite);

  TLegend* leg = new TLegend(0.30,0.2,0.80,0.35);
  leg->SetBorderSize(0);
  leg->SetTextSize(0.035);
  leg->SetFillColor(kWhite);
  std::stringstream leglabel;
  leglabel.str("");
  leglabel << std::fixed << std::setprecision(2) << "John, Al50, Mean = " << gr_john->GetMean(2) << ", Std Dev = " << gr_john->GetRMS(2);
  leg->AddEntry(gr_john, leglabel.str().c_str(), "pl");

  leglabel.str("");
  leglabel << "Mark, Al50, Mean = " << gr_mark->GetMean(2) << ", Std Dev = " << gr_mark->GetRMS(2);
  leg->AddEntry(gr_mark, leglabel.str().c_str(), "pl");

  leglabel.str("");
  leglabel << "Andy, Si16b, Mean = " << gr_andy->GetMean(2) << ", Std Dev = " << gr_andy->GetRMS(2);
  leg->AddEntry(gr_andy, leglabel.str().c_str(), "pl");

  leglabel.str("");
  leglabel << "All, Mean = " << gr_nomc->GetMean(2) << ", Std Dev = " << gr_nomc->GetRMS(2);
  leg->AddEntry(gr_nomc, leglabel.str().c_str(), "pl");

  leg->Draw();
}
