void SiCalibrationPlots() {

  const int n_channels = 10;
  const int n_peaks = 2;
  std::string names[n_channels] = {"SiL2-S", "SiR2-S", "SiR1-1-S", "SiR1-2-S", "SiR1-3-S", "SiR1-4-S", 
				   "SiL1-1-S", "SiL1-2-S", "SiL1-3-S", "SiL1-4-S"};
  double peaks[n_channels][n_peaks] = { {5422, 466}, {5422, 630}, {1000, 5485}, {1000, 5485}, {1000, 5485}, {1000, 5485},
				  {1000, 5485}, {1000, 5485}, {1000, 5485}, {1000, 5485} };
  double ADCs[n_channels][n_peaks] = { {703.6, 73.4}, {703.9, 102.1}, {423.4, 2168.8}, {428.4, 2114.5}, {407.9, 2183.7}, {429.5, 2174.8},
				       {420.2, 2113.2}, {408.3, 2148.7}, {420.7, 2088.9}, {420.3, 2158.8} };

  TGraph* graphs[n_channels];
  TMultiGraph* multi_graph = new TMultiGraph();

  for (int i_channel = 0; i_channel < n_channels; ++i_channel) {
    std::cout << names[i_channel] << ": ";
    graphs[i_channel] = new TGraph(n_peaks, peaks[i_channel], ADCs[i_channel]);
    for (int i_peak = 0; i_peak < n_peaks; ++i_peak) {
      std::cout << peaks[i_channel][i_peak] << " ";
    }
    std::cout << std::endl;
    multi_graph->Add(graphs[i_channel]);
  }
  multi_graph->Draw("AL*");
}
