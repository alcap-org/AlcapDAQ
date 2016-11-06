/**
 * Refer to the name array for choosing the histograms to track over runs. By default the
 * root files after timeoffset_calib.cfg is places in the folder files.
 * A simple Gaussian fit is used with the mean parameter set to be the max bin value.
 */
#include <vector>
#include <string>
const char* name[] = {
  "ghi/hSiT-1-F_GeHiGain#FirstComplete#{constant_fraction=0.70}{no_time_shift=true}TDiff",	//0
  "glo/hSiT-1-F_GeLoGain#FirstComplete#{constant_fraction=0.70}{no_time_shift=true}TDiff",	//1
  "l3f/hSiT-1-F_SiL3-F#FirstComplete#{constant_fraction=0.70}{no_time_shift=true}TDiff",	//2
  "l3s/hSiT-1-F_SiL3-S#FirstComplete#{constant_fraction=0.70}{no_time_shift=true}TDiff",	//3
  "r3s/hSiT-1-F_SiR3-S#FirstComplete#{constant_fraction=0.70}{no_time_shift=true}TDiff",	//4
  "r2f/hSiT-1-F_SiR2-F#FirstComplete#{constant_fraction=0.70}{no_time_shift=true}TDiff",	//5
  "r2s/hSiT-1-F_SiR2-S#FirstComplete#{constant_fraction=0.70}{no_time_shift=true}TDiff",	//6
  "l1Fs/hSiT-1-F_SiL1-16-S#FirstComplete#{constant_fraction=0.70}{no_time_shift=true}TDiff",	//7
  "l1bf/hSiT-1-F_SiL1-B-F#FirstComplete#{constant_fraction=0.70}{no_time_shift=true}TDiff",	//8
  "r14s/hSiT-1-F_SiR1-4-S#FirstComplete#{constant_fraction=0.70}{no_time_shift=true}TDiff",	//9
  "r14f/hSiT-1-F_SiR1-4-F#FirstComplete#{constant_fraction=0.70}{no_time_shift=true}TDiff",	//10
  "t2f/hSiT-1-F_SiT-2-F#FirstComplete#{constant_fraction=0.70}{no_time_shift= true}TDiff",	//11
  "t3f/hSiT-1-F_SiT-3-F#FirstComplete#{constant_fraction=0.70}{no_time_shift= true}TDiff",	//12
  "t4f/hSiT-1-F_SiT-4-F#FirstComplete#{constant_fraction=0.70}{no_time_shift= true}TDiff",	//13
};
vector<Float_t> run;
vector<Float_t> mean;
vector<Float_t> sigma;
vector<Float_t> chisquare;

void GetTDiff(int channel = 10, const char *dirname="files/") {
  TSystemDirectory dir(dirname, dirname);
  TList *files = dir.GetListOfFiles();
  if(files) {
    TSystemFile *file;
    TString fname;
    TIter next(files);
    while ((file=(TSystemFile*)next() ) ) {
      fname = file->GetName();
      if(!file->IsDirectory() && fname.EndsWith(".root") ) {
        GetHistogram(fname, "PlotTAP_TDiff", name[channel]);
      }
    }
    DrawGraph(name[channel]);
  }
}
void GetHistogram(const char * filename, const char * dirname, const char * histogram, int window_size = 1000) {

  std::string filepath("files/");
  filepath.append(filename);
  TFile* file = new TFile(filepath.c_str(), "READ");
  TDirectoryFile * tdiff_dir = (TDirectoryFile*) file->Get(dirname);
  if (!tdiff_dir) {
    cout << "Error: " << dirname << " folder doesn't exist in " << filename << endl;
    return 0;
  }
  std::string strFilename(filename);
  int number;
  int binMax;
  double maxPoint;
  number = atoi(strFilename.substr(3, 5).c_str() );

  TH1F * hist = tdiff_dir->Get(histogram);
  binMax = hist->GetMaximumBin();
  maxPoint = hist->GetXaxis()->GetBinCenter(binMax);
  
  TF1 * fitter = new TF1("fitter", "gaus", maxPoint - window_size/2, maxPoint + window_size/2);
  fitter->SetParameter(1, maxPoint);
  hist->Fit(fitter, "RQ+");
  run.push_back(number);
  mean.push_back(fitter->GetParameter(1) );
  sigma.push_back(fitter->GetParameter(2) );
  chisquare.push_back(fitter->GetChisquare() );
}
void DrawGraph(const char *name, bool debug = false) {
  if(debug) {
    cout << run.size() << endl;
    for(int i=0; i<run.size(); i++) {
      cout << run.at(i) << " " << mean.at(i) << " " << sigma.at(i) << " " << chisquare.at(i) << endl;
    }
  }
  TGraphErrors *gr = new TGraphErrors(run.size(), &run[0], &mean[0], 0, &sigma[0]);
  gr->SetTitle(name);
  gr->GetXaxis()->SetTitle("Run");
  gr->GetYaxis()->SetTitle("Mean (ns)");
  gr->Draw();
}
