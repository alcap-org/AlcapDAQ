void rsp(int runnr, int addr, int fadc)
{
  gInterpreter->AddIncludePath("/home/nam/tmp/mucapdaq/midas/include");
  gROOT->LoadMacro("hdb.cxx+");
  //gROOT->LoadMacro("/home/nam/mucap_daq/midas/linux/lib/libmidas_shared.so");
  gROOT->LoadMacro("/home/nam/tmp/mucapdaq/midas/linux/lib/libmidas.so");
  gROOT->LoadMacro("/home/nam/tmp/mucapdaq/compress/libmucap_compress.so");
  gROOT->LoadMacro("show_pulses.cxx+");
  char name[256];
  sprintf(name, "/home/nam/tmp/mucapdaq/dir/run%05d.mid", runnr);
  process_file(name, addr, fadc);
}
