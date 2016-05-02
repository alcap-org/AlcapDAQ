template<typename T>
std::string toString(const T& value)
{
  std::ostringstream oss;
  oss << value;
  return oss.str();
}

void targett(std::string hist_type)
{
  /*****************************************************************/
  // Prepare the canvas
  gStyle->SetOptStat("ne");
  TCanvas *AlCapCanvas = (TCanvas *) gROOT->GetListOfCanvases()->At(0);
  AlCapCanvas->Clear();
  AlCapCanvas->Divide(4,1);
  /*****************************************************************/
  const int n_channels = 4;
  int canvas_number = 1;
//  std::string hist_type[]={"adcSum","Heights", "Times"};
  std::string digitizer = "SIS3300";
  std::string ch_name;

  //Channel SiT
  for (int j = 1; j <= n_channels; j++) {
    ch_name = digitizer + "_B2" + "C" + toString(j);
    TH1* hist = get_histogram(ch_name, hist_type);
    if (!hist) continue;
    AlCapCanvas->cd(canvas_number);
    if(hist_type.compare("Shapes") == 0) {
      hist->Draw("COLZ");
      gPad->SetLogz();
    } else {
      hist->Draw();
      gPad->SetLogy();
    }
    canvas_number++;
  }
}
