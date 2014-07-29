void add_seq(std::vector<Int_t>& list, Int_t low, Int_t high) {
  for (Int_t i = low; i <= high; ++i)
    list.push_back(i);
}

std::vector<Int_t> get_run_list() {
  std::vector<Int_t> runs;
  add_seq(runs, 2806, 2813);
  add_seq(runs, 2826, 2873);
  add_seq(runs, 2889, 2894);
  add_seq(runs, 2897, 2905);
  add_seq(runs, 2934, 2944);
  add_seq(runs, 2963, 2992);
  add_seq(runs, 2994, 2995);
  add_seq(runs, 2997, 2999);
  add_seq(runs, 3012, 3012);
  return runs;
}

std::vector<std::string> get_file_names() {
  std::vector<Int_t> runs = get_run_list();
  std::vector<std::string> fnames;
  for (unsigned int i = 0; i < runs.size(); ++i) {
    char str[16];
    sprintf(str, "out%05d.root", fnames.at(i));
    fnames.push_back(std::string(str));
  }
  return fnames;
}


{
  TSpectrum *s = new TSpectrum();
  
}
