void muPC()
{
  /*****************************************************************/
  // Prepare the canvas

  //gROOT->LoadMacro("palette.cxx");
  //SetRainbowPalette();
  // *** VT 10-Nov-2005
  //gStyle->SetOptStat("ner");
  gStyle->SetOptStat(1111);
  TCanvas *MuSunCanvas = (TCanvas *) gROOT->GetListOfCanvases()->At(0);
  MuSunCanvas->Clear();
  MuSunCanvas->Divide(2,2);  
 
  MuSunCanvas->cd(1);

  //TH2 *hmupc1 = (TH2*) getObject("hmupc1XY");
  TH2 *hmupc1 = (TH2*) getObject("muPC1_coinc_XY_2D");
  hmupc1->SetTitle("MuPC XY coincidence");
  hmupc1->GetXaxis()->Set(24, 0.5, 24.5);
  hmupc1->GetYaxis()->Set(24, 0.5, 24.5);
  hmupc1->Draw("COL");
  TLine *crosshair_horiz = new TLine();
  TLine *crosshair_vert = new TLine();
  crosshair_horiz->SetLineColor(kWhite);
  crosshair_vert->SetLineColor(kWhite);
  double vertPos = 14.5;
  double horzPos = 14.5; 
  crosshair_horiz->DrawLine(0,vertPos,25,vertPos);
  crosshair_vert->DrawLine(horzPos,0,horzPos,25);


  MuSunCanvas->cd(3);  
  gStyle->SetOptFit(0111);
  TH1 *hmupc1x = hmupc1->ProjectionX();
  hmupc1x->SetTitle("MuPC1 X");
  hmupc1x->Draw("");  
  hmupc1x->Fit("gaus","","",2.,23.);

  MuSunCanvas->cd(2);  
  TH1 *hmupc1y = hmupc1->ProjectionY();
  hmupc1y->SetTitle("MuPC1 Y");
  hmupc1y->Draw("");
  hmupc1y->Fit("gaus","","",1.,24.);

  MuSunCanvas->cd(4);  
  TH2 *hmupcCoinc = (TH2*) getObject("muPC1_coinc_XY_2D_with_muSC");
  hmupcCoinc->GetXaxis()->Set(24, 0.5, 24.5);
  hmupcCoinc->GetYaxis()->Set(24, 0.5, 24.5);
  hmupcCoinc->SetTitle("muPC1 coinc muSC");
  hmupcCoinc->Draw("COL");
  TLine *crosshair_horiz = new TLine();
  TLine *crosshair_vert = new TLine();
  crosshair_horiz->SetLineColor(kWhite);
  crosshair_vert->SetLineColor(kWhite);
  crosshair_horiz->DrawLine(0,vertPos,25,vertPos);
  crosshair_vert->DrawLine(horzPos,0,horzPos,25);

  /*
  MuSunCanvas->cd(4);
  TH2 *h4  = (TH2*) getObject("hmupcXY_BestEntrance");
  h4->SetTitle("muPC1 coinc Best Entrance");
  h4->Draw("COL");
  crosshair_horiz->DrawLine(0,vertPos,25,vertPos);
  crosshair_vert->DrawLine(horzPos,0,horzPos,25);

  MuSunCanvas->cd(5);
  TH2 *hmupcXY_BestEntranceWithStopTrack  = (TH2*) getObject("hmupcXY_BestEntranceWithStopTrack");
  hmupcXY_BestEntranceWithStopTrack->Divide(h4);
  hmupcXY_BestEntranceWithStopTrack->SetTitle("muPC1 Best Entrance Stop efficiency");
  hmupcXY_BestEntranceWithStopTrack->Draw("COLZ");
  crosshair_horiz->DrawLine(0,vertPos,25,vertPos);
  crosshair_vert->DrawLine(horzPos,0,horzPos,25);
  */

/*  MuSunCanvas->cd(4);  
  TH3 *fMupcXYT_muschi = (TH3*) getHist("hMupcXYT_muschi");
  fMupcXYT_muschi->GetZaxis()->SetRangeUser(150,250);
  fMupcXYT_muschi->Project3D("yx")->Draw("col");
  TLine *crosshair_horiz = new TLine();
  TLine *crosshair_vert = new TLine();
  crosshair_horiz->SetLineColor(kWhite);
  crosshair_vert->SetLineColor(kWhite);
  crosshair_horiz->DrawLine(-50,0,50,0);
  crosshair_vert->DrawLine(0,-50,0,50);

  MuSunCanvas->cd(5);  
  TH3 *fMupcXYT_musclo = (TH3*) getHist("hMupcXYT_musclo");
  fMupcXYT_musclo->GetZaxis()->SetRangeUser(200,250);
  fMupcXYT_musclo->Project3D("xy")->Draw("col");
  TLine *crosshair_horiz = new TLine();
  TLine *crosshair_vert = new TLine();
  crosshair_horiz->SetLineColor(kWhite);
  crosshair_vert->SetLineColor(kWhite);
  crosshair_horiz->DrawLine(-50,0,50,0);
  crosshair_vert->DrawLine(0,-50,0,50);*/
}

