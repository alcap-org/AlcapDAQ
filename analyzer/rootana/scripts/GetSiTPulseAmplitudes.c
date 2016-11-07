/**
 * Refer to the name array for choosing the histograms to track over runs. By default the
 * root files after SiTPulseAmplitudes.cfg are placed in the folder SiTPulseAmplitudes.
 */
#include <vector>
#include <string>
const char* name[] = {
  "hSiT-1-S#MaxBinAPGenerator#any_Amplitude",
  "hSiT-2-S#MaxBinAPGenerator#any_Amplitude",
  "hSiT-3-S#MaxBinAPGenerator#any_Amplitude",
  "hSiT-4-S#MaxBinAPGenerator#any_Amplitude",
};
TH2F * trend;

void GetSiTPulseAmplitudes(int start_run = 9655, int end_run = 9682, int channel = 0, const char *dirname="SiTPulseAmplitudes/") {
  trend = new TH2F(name[channel], "h", end_run - start_run, start_run, end_run, 4096, 0, 4096);
  trend->Reset();
  trend->SetDirectory(0);
  TSystemDirectory dir(dirname, dirname);
  TList *files = dir.GetListOfFiles();
  if(files) {
    TSystemFile *file;
    TString fname;
    TIter next(files);
    while ((file=(TSystemFile*)next() ) ) {
      fname = file->GetName();
      if(!file->IsDirectory() && fname.EndsWith(".root") ) {
        GetHistogram(fname, "PlotTAP_Amplitude", channel);
      }
    }
    trend->Draw("colz");
  }
}
void GetHistogram(const char * filename, const char * dirname, int channel) {

  std::string filepath("SiTPulseAmplitudes/");
  filepath.append(filename);
  TFile* file = new TFile(filepath.c_str(), "READ");
  TDirectoryFile * dir = (TDirectoryFile*) file->Get(dirname);
  if (!dir) {
    cout << "Error: " << dirname << " folder doesn't exist in " << filename << endl;
    return 0;
  }
  std::string strFilename(filename);
  int number;
  int binMax;
  double maxPoint;
  number = atoi(strFilename.substr(3, 5).c_str() );
  TH1F * hist = dir->Get(name[channel]);
  for(int i=0; i < hist->GetSize(); i++) {
    trend->Fill(number, i, hist->GetBinContent(i) );
  }
}
