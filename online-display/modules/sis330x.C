template<typename T>
std::string toString(const T& value)
{
  std::ostringstream oss;
  oss << value;
  return oss.str();
}

void sis330x(std::string digitizer, std::string hist_type)
{
  /*****************************************************************/
  // Prepare the canvas
  gStyle->SetOptStat("ne");
  TCanvas *AlCapCanvas = (TCanvas *) gROOT->GetListOfCanvases()->At(0);
  AlCapCanvas->Clear();
  AlCapCanvas->Divide(8,5);
  /*****************************************************************/
  const int n_channels = 8;
  const int n_boards = 5;
  int canvas_number = 1;
//  std::string hist_type[]={"adcSum","Heights", "Times"};
  std::string ch_name;

  for (int i = 1; i <= n_boards; i++) {
    for (int j = 1; j <= n_channels; j++) {
      ch_name = digitizer + "_B" + toString(i) + "C" + toString(j);
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
}
