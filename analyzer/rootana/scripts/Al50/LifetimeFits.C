void LifetimeFits() {

  std::string savelocation = "~/data/out/v33/plots";
  const int n_arms = 2;

  TFile* file = new TFile("~/data/out/v33/total.root");
  TH1F* SiL_Time = (TH1F*) file->Get("TME_EvdE/hSiL_Time");
  TH1F* SiR_Time = (TH1F*) file->Get("TME_EvdE/hSiR_Time");

  TH1F* time_hists[n_arms] = {SiL_Time, SiR_Time};
  std::string arm_names[n_arms] = {"SiL", "SiR"};
  double final_times[n_arms] = {6000,4000};

  for (int i_arm = 0; i_arm < n_arms; ++i_arm) {
    time_hists[i_arm]->Rebin(20);

    double initial_time = 0;
    //    double final_time = 6000;

    TF1* expo = new TF1("exponential", "[0]*TMath::Exp([1]*x) + [2]*TMath::Exp([3]*x)", initial_time, final_times[i_arm]);    
    expo->SetParameter(0, 1000);
    expo->SetParameter(1, -0.001);
    expo->SetParameter(2, 1000);
    expo->SetParameter(3, -0.01);
    time_hists[i_arm]->Fit("exponential", "QR");
    gStyle->SetOptFit(1111);
  
    double al_lifetime = -1.0/expo->GetParameter(1);
    double al_error = expo->GetParError(1) / (expo->GetParameter(1)*expo->GetParameter(1));
    double pb_lifetime = -1.0/expo->GetParameter(3);
    double pb_error = expo->GetParError(3) / (expo->GetParameter(3)*expo->GetParameter(3));

    std::cout << "[" << initial_time << ", " << final_times[i_arm] << "]: Al = " << al_lifetime << " +- " << al_error << " ns" << std::endl;
    std::cout << "[" << initial_time << ", " << final_times[i_arm] << "]: Pb = " << pb_lifetime << " +- " << pb_error << " ns" << std::endl;
    std::cout << "[" << initial_time << ", " << final_times[i_arm] << "]: chi^2/ndf = " << expo->GetChisquare() / expo->GetNDF() << std::endl;

    TF1* Al_decay = new  TF1("Al_decay", "[0]*TMath::Exp([1]*x)", initial_time, final_times[i_arm]);    
    Al_decay->SetParameter(0, expo->GetParameter(0));
    Al_decay->SetParameter(1, expo->GetParameter(1));
    Al_decay->SetLineColor(kBlue);
    TF1* Pb_decay = new  TF1("Pb_decay", "[0]*TMath::Exp([1]*x)", initial_time, final_times[i_arm]);    
    Pb_decay->SetParameter(0, expo->GetParameter(2));
    Pb_decay->SetParameter(1, expo->GetParameter(3));
    Pb_decay->SetLineColor(kBlack);

    Al_decay->Draw("SAME");
    Pb_decay->Draw("SAME");
    std::string plotname = savelocation+"/"+arm_names[i_arm]+"_DoubleExpFit";
    std::string pdfname = plotname+".pdf";
    std::string pngname = plotname+".png";
    c1->SaveAs(pdfname.c_str());
    c1->SaveAs(pngname.c_str());

    int lead_decay_drop = Pb_decay->GetX(1); // the time at which the exponential goes below 1
    initial_time = lead_decay_drop - lead_decay_drop%100; // round to the nearest hundred
    TF1* new_expo = new TF1("new_exponential", "[0]*TMath::Exp([1]*x)", initial_time, final_times[i_arm]);    
    new_expo->SetParameter(0, 1000);
    new_expo->SetParameter(1, -0.001);
    time_hists[i_arm]->Fit("new_exponential", "QR");

    al_lifetime = -1.0/new_expo->GetParameter(1);
    al_error = new_expo->GetParError(1) / (new_expo->GetParameter(1)*new_expo->GetParameter(1));

    std::cout << "[" << initial_time << ", " << final_times[i_arm] << "]: Al = " << al_lifetime << " +- " << al_error << " ns" << std::endl;
    std::cout << "[" << initial_time << ", " << final_times[i_arm] << "]: chi^2/ndf = " << new_expo->GetChisquare() / new_expo->GetNDF() << std::endl;
    std::string plotname = savelocation+"/"+arm_names[i_arm]+"_SingleExpFit";
    std::string pdfname = plotname+".pdf";
    std::string pngname = plotname+".png";
    c1->SaveAs(pdfname.c_str());
    c1->SaveAs(pngname.c_str());
  }
}
