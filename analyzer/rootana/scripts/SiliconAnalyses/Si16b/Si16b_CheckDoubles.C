void Si16b_CheckDouble() {

  TFile* file = new TFile("~/data/results/Si16b_passive/subtrees.root", "READ");
  TTree* tree = (TTree*) file->Get("siBlockTree_SiL");

  int run_id;
  int block_id;
  int tme_id;
  double thin_energy;
  double thin_time;
  int thin_tpi_id;
  double thick_energy;
  double thick_time;
  int thick_tpi_id = -1;
  tree->SetBranchAddress("run_id", &run_id);
  tree->SetBranchAddress("block_id", &block_id);
  tree->SetBranchAddress("tme_id", &tme_id);
  tree->SetBranchAddress("thin_energy", &thin_energy);
  tree->SetBranchAddress("thin_time", &thin_time);
  tree->SetBranchAddress("thin_tpi_id", &thin_tpi_id);
  tree->SetBranchAddress("thick_energy", &thick_energy);
  tree->SetBranchAddress("thick_time", &thick_time);
  tree->SetBranchAddress("thick_tpi_id", &thick_tpi_id);

  int current_entry = 0;
  int last_run_id;
  int last_block_id;
  int last_tme_id;
  int last_thick_tpi_id;
  while (thick_tpi_id < 0) {
    tree->GetEntry(current_entry);
    last_run_id = run_id;
    last_block_id = block_id;
    last_tme_id = tme_id;
    last_thick_tpi_id = thick_tpi_id;
    ++current_entry;
  }
  for (int i_entry = current_entry; i_entry < tree->GetEntries(); ++i_entry) {
    tree->GetEntry(i_entry);

    if (thick_tpi_id < 0) {
      continue;
    }

    if (run_id == last_run_id && block_id == last_block_id && tme_id == last_tme_id && thick_tpi_id == last_thick_tpi_id) {
      std::cout << "Repeated Thick TPI: Run #" << run_id << ", Block #" << block_id << ", TME #" << tme_id << ", TPI #" << thick_tpi_id << ": E = " << thick_energy << ", t = " << thick_time << std::endl;
    }

    last_run_id = run_id;
    last_block_id = block_id;
    last_tme_id = tme_id;
    last_thick_tpi_id = thick_tpi_id;
  }
}
