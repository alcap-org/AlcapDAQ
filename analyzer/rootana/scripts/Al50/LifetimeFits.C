void LifetimeFits() {

  TFile* file = new TFile("~/data/out/v33/total.root");
  TH1F* SiL_Time = (TH1F*) file->Get("TME_EvdE/hSiL_Time");

  SiL_Time->Rebin(20);

  double initial_time = 0;
  double final_time = 6000;

  TF1* expo = new TF1("exponential", "[0]*TMath::Exp([1]*x) + [2]*TMath::Exp([3]*x)", initial_time, final_time);    
  expo->SetParameter(0, 1000);
  expo->SetParameter(1, -0.001);
  expo->SetParameter(2, 1000);
  expo->SetParameter(3, -0.01);
  SiL_Time->Fit("exponential", "QR");
  
  double al_lifetime = -1.0/expo->GetParameter(1);
  double al_error = expo->GetParError(1) / (expo->GetParameter(1)*expo->GetParameter(1));
  double pb_lifetime = -1.0/expo->GetParameter(3);
  double pb_error = expo->GetParError(3) / (expo->GetParameter(3)*expo->GetParameter(3));

  std::cout << "[" << initial_time << ", " << final_time << "]: Al = " << al_lifetime << " +- " << al_error << " ns" << std::endl;
  std::cout << "[" << initial_time << ", " << final_time << "]: Pb = " << pb_lifetime << " +- " << pb_error << " ns" << std::endl;
  std::cout << "[" << initial_time << ", " << final_time << "]: chi^2/ndf = " << expo->GetChisquare() / expo->GetNDF() << std::endl;

  TF1* Al_decay = new  TF1("Al_decay", "[0]*TMath::Exp([1]*x)", initial_time, final_time);    
  Al_decay->SetParameter(0, expo->GetParameter(0));
  Al_decay->SetParameter(1, expo->GetParameter(1));
  Al_decay->SetLineColor(kBlue);
  TF1* Pb_decay = new  TF1("Pb_decay", "[0]*TMath::Exp([1]*x)", initial_time, final_time);    
  Pb_decay->SetParameter(0, expo->GetParameter(2));
  Pb_decay->SetParameter(1, expo->GetParameter(3));
  Pb_decay->SetLineColor(kBlack);

  Al_decay->Draw("SAME");
  Pb_decay->Draw("SAME");
  c1->SaveAs("~/data/out/v33/plots/SiL_DoubleExpFit.pdf");
  c1->SaveAs("~/data/out/v33/plots/SiL_DoubleExpFit.png");

  initial_time = 600;
  TF1* new_expo = new TF1("new_exponential", "[0]*TMath::Exp([1]*x)", initial_time, final_time);    
  new_expo->SetParameter(0, 1000);
  new_expo->SetParameter(1, -0.001);
  hSiL_Time->Fit("new_exponential", "QR");

  al_lifetime = -1.0/new_expo->GetParameter(1);
  al_error = new_expo->GetParError(1) / (new_expo->GetParameter(1)*new_expo->GetParameter(1));

  std::cout << "[" << initial_time << ", " << final_time << "]: Al = " << al_lifetime << " +- " << al_error << " ns" << std::endl;
  std::cout << "[" << initial_time << ", " << final_time << "]: chi^2/ndf = " << new_expo->GetChisquare() / new_expo->GetNDF() << std::endl;
}
