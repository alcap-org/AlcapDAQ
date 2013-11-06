void dummy()
{
  /*****************************************************************/
  // Prepare the canvas
	gStyle->SetOptStat("ne"); 
	TCanvas *canvas = (TCanvas *) gROOT->GetListOfCanvases()->At(0);
	canvas->Clear();
	canvas->Divide(2,2);
  /*****************************************************************/
	TH1 *h1 = (TH1 *)gDirectory->FindObject("hDummy1");
	if (h1)
	{
		TVirtualPad *pad = canvas->cd(1);
		h1->Draw();
	}
}
