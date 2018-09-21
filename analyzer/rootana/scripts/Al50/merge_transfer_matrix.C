{
  const int N           = 36;
  const char* IFNAMEFMT = "data/Al100/tm%d.2layer.root";
  const char* OFNAME    = "data/Al100/tm.2layer.root";
  TFileMerger ofile;
  for (int i = 1; i <= N; ++i) {
    char ifname[256];
    sprintf(ifname, IFNAMEFMT, i);
    ofile.AddFile(ifname);
  }
  ofile.OutputFile(OFNAME);
  ofile.Merge();
}
