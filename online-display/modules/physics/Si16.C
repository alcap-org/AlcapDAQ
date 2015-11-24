void Si16()
{
  /*****************************************************************/
  // Prepare the canvas
  gStyle->SetOptStat("ne");
  TCanvas *AlCapCanvas = (TCanvas *) gROOT->GetListOfCanvases()->At(0);
  AlCapCanvas->Clear();
  AlCapCanvas->Divide(4,2);

  gStyle->SetOptFit(1);
  
  //  gROOT->ProcessLine(".L modules/common/get_histogram.C"); // get_histogram() defined here
  /*****************************************************************/

  TH1D *h1_Y = new TH1D("h1_Y","muons",15,0.5,15.5);
  h1_Y->SetXTitle("Strip Nr");
  h1_Y->SetYTitle("counts");
  h1_Y->SetLineWidth(3);

  int bin = 0;
  for (unsigned int iboard=4; iboard<6; iboard++)
    {
      for (unsigned int ich=1; ich<9; ich++)
	{
	  bin++;
	  if ( bin > 15 ) break;
	  TH1 *h1_S = get_histogram( Form("SIS3300_B%iC%i", iboard, ich), "Heights" );
	  if ( !h1_S ) continue;
	  
	  if (iboard == 1 && ich == 4) { h1_S->Draw(); } else { h1_S->Draw("Same"); }
	  //Double_t Y = h1_S->Integral(170,500);
	  //Double_t Y = h1_S->Integral(165,500);
	  // 11-18-2015 
	  //Double_t Y = h1_S->Integral(150,800);
	  Double_t Y = h1_S->Integral(200,800);
	  h1_Y->SetBinContent(bin,Y);
	}
    }

  h1_Y->Draw();
  gPad->SetGridx();

  Double_t Nmu = h1_Y->Integral();
  
  TPaveText *pt = new TPaveText(0.805,0.837,0.997,0.997,"NDC");
  pt->AddText( Form("N_{#mu} = %.2e",Nmu) );    
  
  
  TF1 *f_fit = new TF1("f_fit","gaus(0)",0.0,16.0);
  f_fit->SetParameter(0,10000);
  f_fit->SetParameter(1,8);
  f_fit->SetParameter(2,12);
  h1_Y->Fit(f_fit,"RN");
  h1_Y->Fit(f_fit,"RE");
  f_fit->Draw("Same");
  f_fit->SetLineColor( kRed );

  pt->Draw();

  
}
