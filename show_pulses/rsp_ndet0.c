void rsp_ndet0(int runnr, int addr, int fadc)
{
  gInterpreter->AddIncludePath("/home/l_data/daq/midas/include");
  gROOT->LoadMacro("/home/l_data/daq/midas/linux64/lib/libmidas_shared.so");
  gROOT->LoadMacro("hdb.cxx+");
  gROOT->LoadMacro("/home/l_data/online-analysis/compress/libmucap_compress.so");
  gROOT->LoadMacro("show_pulses_ndet_0.cxx+g");
  char name[256];
  sprintf(name, "/data/midas-mucap/run%05d.mid", runnr);
  process_file(name, addr, fadc);
}
