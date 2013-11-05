
void raw()
{

  /*****************************************************************/
  // Prepare the canvas
  gStyle->SetOptStat("ne"); 
  TCanvas *canvas = (TCanvas *) gROOT->GetListOfCanvases()->At(0);
  canvas->Divide(2,3);
  /*****************************************************************/
  

  char name[256];

  // ==============================================================
  //      X-Y coincidence
  // ==============================================================

  TVirtualPad *pad = canvas->cd(1);
  sprintf(name,"emc_hist_raw/h2_XY");
  TH2D *h2_XY = (TH2D*)getObject( name );
  if ( !h2_XY )
    {
      printf("***ERROR! Cannot retrieve histogram [%s]\n",name);
    }
  else
    {
      h2_XY->Draw("ColZ");
      pad->SetLogz();
    }
  
  // ==============================================================
  //      X-distribution, single hits
  // ==============================================================

  TVirtualPad *pad = canvas->cd(3);
  sprintf(name,"emc_hist_raw/h1_X");
  TH1D *h1_X = (TH1D*)getObject( name );
  if ( !h1_X )
    {
      printf("***ERROR! Cannot retrieve histogram [%s]\n",name);
    }
  else
    {
      h1_X->Draw();
      h1_X->SetLineWidth(2);
    }

  
  // ==============================================================
  //      X-distribution, coincidences
  // ==============================================================

  if ( h2_XY )
    {
      TH1D *h1_XY_px = h2_XY->ProjectionX();      
      h1_XY_px->Draw("Same");
      h1_XY_px->SetLineColor(kRed);
      h1_XY_px->SetLineWidth(2);
    }
  
  // ==============================================================
  //      Y-distribution, single hits
  // ==============================================================

  TVirtualPad *pad = canvas->cd(2);
  sprintf(name,"emc_hist_raw/h1_Y");
  TH1D *h1_Y = (TH1D*)getObject( name );
  if ( !h1_Y )
    {
      printf("***ERROR! Cannot retrieve histogram [%s]\n",name);
    }
  else
    {
      h1_Y->Draw();
      h1_Y->SetLineWidth(2);
    }

  
  // ==============================================================
  //      Y-distribution, coincidences
  // ==============================================================

  if ( h2_XY )
    {
      TH1D *h1_XY_py = h2_XY->ProjectionY();
      h1_XY_py->Draw("Same");
      h1_XY_py->SetLineColor(kRed);
      h1_XY_py->SetLineWidth(2);
    }


   // ==============================================================
  //      Bank size vs. spill nr
  // ==============================================================

  TVirtualPad *pad = canvas->cd(4);
  sprintf(name,"emc/gr_bk_size");
  TGraphErrors *gr = (TGraphErrors*) getObject( name );
  if ( !gr )
    {
      printf("***ERROR! Cannot retrieve histogram [%s]\n",name);
    }
  else
    {
      gr->Draw("AL");
      gr->SetLineColor(kBlue);
      gr->GetHistogram()->SetXTitle("spill number");
      gr->GetHistogram()->SetYTitle("bank size (DWORDs)");
      gr->GetHistogram()->GetYaxis()->SetTitleOffset(1.4);
    }

  
  // ==============================================================
  //      Time-distribution of single X-hits
  // ==============================================================
  
  TVirtualPad *pad = canvas->cd(5);
  sprintf(name,"emc_hist_raw/h1_X_t");
  TH1D *h1_X_t = (TH1D*)getObject( name );
  if ( !h1_X_t ){
      printf("***ERROR! Cannot retrieve histogram [%s]\n",name);
  }
  else{
    h1_X_t->Draw();
  }


  // ==============================================================
  //      Time-distribution of single Y-hits
  // ==============================================================
  
  TVirtualPad *pad = canvas->cd(6);
  sprintf(name,"emc_hist_raw/h1_Y_t");
  TH1D *h1_Y_t = (TH1D*)getObject( name );
  if ( !h1_Y_t ){
    printf("***ERROR! Cannot retrieve histogram [%s]\n",name);
  }
  else{
    h1_Y_t->Draw();
  }

}
