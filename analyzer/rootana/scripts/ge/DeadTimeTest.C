struct Run {
  double t_block;
  double t_dead;
  int n_block;
  int seen_photons;

  void seen(double time) {
    // calculate the block it would appear in
    int i_block = time / (t_block+t_dead);
    double t_in_block = time - i_block*(t_block+t_dead);
    //    std::cout << "Time = " << time << ", i_block = " << i_block << ", t_in_block = " << t_in_block << std::endl;
    if (t_in_block < t_block) {
      //      std::cout << "True" << std::endl;
      ++seen_photons;
    }
  }    
};

void DeadTimeTest() {

  double total_run_time = 6;
  
  Run A;
  A.t_block = 1;
  A.t_dead = 1;
  A.n_block = total_run_time / (A.t_block + A.t_dead);
  A.seen_photons = 0;

  Run B;
  B.t_block = 1;
  B.t_dead = 0.5;
  B.n_block = total_run_time / (B.t_block + B.t_dead);
  B.seen_photons = 0;

  std::cout << "Run A: t_block = " << A.t_block << " s, t_dead = " << A.t_dead << " s, n_block = " << A.n_block << std::endl;
  std::cout << "Run B: t_block = " << B.t_block << " s, t_dead = " << B.t_dead << " s, n_block = " << B.n_block << std::endl;

  int n_photons = 10000000;
  double activity = n_photons / total_run_time;
  std::cout << "Activity = " << n_photons << " / " << total_run_time << " = " << activity << " decays/s" << std::endl;
  TF1* arrival_fn = new TF1("arrival_fn", "1", 0, total_run_time);
  TH1F* hArrivalTime = new TH1F("hArrivalTime", "", 100,0,total_run_time);
  for (int i_photon = 0; i_photon < n_photons; ++i_photon) {
    if (i_photon % 10000 == 0) {
      std::cout << i_photon << " / " << n_photons << std::endl;
    }
    double time = arrival_fn->GetRandom();
    A.seen(time);
    B.seen(time);
    
    hArrivalTime->Fill(time);
  }
  hArrivalTime->Draw();

  std::cout << "Run A: N_photons = " << A.seen_photons << ", Eff = " << A.seen_photons << " / " << activity*A.n_block*A.t_block << " = "
	    << (float)A.seen_photons / (activity*A.n_block*A.t_block) << std::endl;
  std::cout << "Run B: N_photons = " << B.seen_photons << ", Eff = " << B.seen_photons << " / " << activity*B.n_block*B.t_block << " = "
	    << (float)B.seen_photons / (activity*B.n_block*B.t_block) << std::endl;

}
