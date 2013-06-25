void rsp_res_ana(int runnr, int addr, int fadc)
{
  gInterpreter->AddIncludePath("/home/nam/fadc_software/midas/include");
  gROOT->LoadMacro("hdb.cxx+");
  //gROOT->LoadMacro("/home/nam/fadc_software/midas/linux/lib/libmidas_shared.so");
  gROOT->LoadMacro("/home/nam/fadc_software/midas/linux/lib/libmidas.so");
  gROOT->LoadMacro("/home/nam/fadc_software/compress/libmucap_compress.so");
  gROOT->LoadMacro("res_analysis.cxx+");
  char name[256];
  sprintf(name, "/home/nam/fadc_software/dir/run%05d.mid", runnr);
  process_file(name, addr, fadc);
}
