void Ti50_FinalPlot_XRayIntensity_fromLiterature() {
  const int n_lines = 7;
  double intensities[n_lines] = {1.000, 0.102, 0.031, 0.036, 0.041, 0.023, 0.099};
  double errors[n_lines] = {0.005, 0.003, 0.004, 0.003, 0.003, 0.004, 0.005};
  double sum = 0;
  double high_sum = 0; // high is to get highest allowed intensity for 2p1s
  double low_sum = 0;

  for (int i_line = 0; i_line < n_lines; ++i_line) {
    double intensity = intensities[i_line];
    double error = errors[i_line];
    sum += intensity;

    if (i_line==0) {
      high_sum += intensity + error;
      low_sum += intensity - error;
    }
    else {
      high_sum += intensity - error;
      low_sum += intensity + error;
    }
  }
  std::cout << "I = " << intensities[0] / sum << std::endl;
  std::cout << "high I = " << (intensities[0] + errors[0]) / high_sum << std::endl;
  std::cout << "loe I = " << (intensities[0] - errors[0]) / low_sum << std::endl;
}
