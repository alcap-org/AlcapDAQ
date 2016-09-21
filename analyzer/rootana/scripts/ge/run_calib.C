void run_calib(const char* fname, int run, bool gehi) {
  gROOT->ProcessLine(".L Source.C+");
  gROOT->ProcessLine(".L Run.C+");
  gROOT->ProcessLine(".L Peak.C+");
  gROOT->ProcessLine(".L Peaks.C+");
  gROOT->ProcessLine(".L R15b_ge_calib.C+");
  R15b_ge_calib(fname, run, gehi);
}
