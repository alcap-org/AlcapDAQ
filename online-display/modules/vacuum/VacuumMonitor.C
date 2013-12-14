void VacuumMonitor()
{
	/*****************************************************************/
	// Prepare the canvas
	gStyle->SetOptStat("ne");
	TCanvas *AlCapCanvas = (TCanvas *) gROOT->GetListOfCanvases()->At(0);
	AlCapCanvas->Clear();
	//  AlCapCanvas->Divide(4,2);
	TPad *pad1 = new TPad("Pressure","Pressure",0,0.5,1,1);
	pad1->Draw();
	TPad *pad2 = new TPad("Status","Status",0,0,1,0.5);
	pad2->SetFillStyle(4000); //will be transparent
	pad2->Draw();

	// gROOT->ProcessLine(".L modules/common/get_histogram.C"); // get_histogram() defined here
	/*****************************************************************/

	TH1* hist_pressure = (TH1 *)getObject("Vacuum_Pressure");
	TH1* hist_status = (TH1 *)getObject("Vacuum_Status");
	if (hist_pressure) {
		pad1->cd();
		pad1->SetLogy(1);
		pad1->SetGridx(1);
		pad1->SetGridy(1);
		hist_pressure->SetMarkerStyle(20);
		hist_pressure->SetMarkerSize(0.3);
		hist_pressure->GetYaxis()->SetRangeUser(1e-6,1e3);
		hist_pressure->Draw("P");
		pad2->cd();
		pad2->SetGridx(1);
		pad2->SetGridy(1);
		hist_status->SetLineColor(632);
		hist_status->Draw("L");
	}
}
