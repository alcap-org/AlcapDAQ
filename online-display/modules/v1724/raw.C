
void raw()
{

  /*****************************************************************/
  // Prepare the canvas
  gStyle->SetOptStat("ne"); 
  TCanvas *canvas = (TCanvas *) gROOT->GetListOfCanvases()->At(0);
  canvas->Divide(4,2);
  /*****************************************************************/

  char name[1024];

  // ==============================================================
  //      2D pulses
  // ==============================================================

  for (int i=0; i<8; i++)
    {
      TVirtualPad *pad = canvas->cd(1+i);
      sprintf(name,"h2_v1724_pulses_chan_%i",i);
      TH2D *h2 = (TH2D*)getObject( name );
      if ( !h2 )
	{
	  printf("***ERROR! Cannot retrieve histogram [%s]\n",name);
	}
      else
	{
	  h2->Draw("ColZ");
	  pad->SetLogz();
	}
    }

}
