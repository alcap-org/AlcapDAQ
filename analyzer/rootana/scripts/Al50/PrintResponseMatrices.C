void PrintResponseMatrices() {

  TFile* file = new TFile("unfolded.root", "READ");

  const int n_arms = 2;
  std::string arm_names[n_arms] = {"SiL", "SiR"};

  for (int i_arm = 0; i_arm < n_arms; ++i_arm) {
    std::string arm_name = arm_names[i_arm];
    
    std::string response_name = arm_name + "_response;1";
    RooUnfoldResponse* response = (RooUnfoldResponse*) file->Get(response_name.c_str());

    TH2F* response_matrix = (TH2F*) response->Hresponse();
    response_matrix->SetStats(false);
    std::string histtitle = "Response Matrix for " + arm_name;
    response_matrix->SetTitle(histtitle.c_str());
    response_matrix->GetXaxis()->SetRangeUser(0, 20000);
    response_matrix->GetYaxis()->SetRangeUser(0, 20000);
    response_matrix->Draw("COLZ");

    TLine* line = new TLine(0,0, 20000,20000);
    line->SetLineWidth(2);
    line->SetLineColor(kRed);
    line->Draw();

    std::string plotname = "~/plots/ThesisPlots/"+arm_name+"-response-matrix";
    std::string pdfname = plotname+".pdf";
    c1->SaveAs(pdfname.c_str());
  }
}
