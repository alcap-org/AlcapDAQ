void rsp(int runnr, int addr, int fadc)
{
  gInterpreter->AddIncludePath("../midas/include");
  gROOT->LoadMacro("hdb.cxx+");
  //gROOT->LoadMacro("/home/nam/mucap_daq/midas/linux/lib/libmidas_shared.so");
  gROOT->LoadMacro("../midas/linux/lib/libmidas.so");
  gROOT->LoadMacro("../compress/libmucap_compress.so");
  gROOT->LoadMacro("show_pulses.cxx+");
  char name[256];
  sprintf(name, "../dir/run%05d.mid", runnr);
  process_file(name, addr, fadc);
}
