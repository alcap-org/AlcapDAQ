#ifndef IntegrateRates_C
#define IntegrateRates_C

void IntegrateRates_wStatAndSystSpectra(TGraphAsymmErrors* stat_spectrum, TGraphAsymmErrors* syst_spectrum,
					double min_energy, double max_energy,
					double& rate, double& total_stat_error,
					double& total_high_syst_error, double& total_low_syst_error) {
  bool start_integral = false;

  for (int i_element = 0; i_element < syst_spectrum->GetN(); ++i_element) {
    double this_energy = 0; double this_y = 0;
    syst_spectrum->GetPoint(i_element, this_energy, this_y);
    if (this_energy > min_energy) {
      start_integral = true;
    }
    if (this_energy > max_energy) {
      break;
    }
    
    if (start_integral) {
      double prev_energy = 0; double prev_y = 0;
      syst_spectrum->GetPoint(i_element-1, prev_energy, prev_y);
      double previous_x = prev_energy;
      double this_x = this_energy;
      double width_x = this_x - previous_x;
      if (width_x == 0) {
	continue;
      }
      double this_stat_err = stat_spectrum->GetErrorYlow(i_element);
      double this_low_syst_err = syst_spectrum->GetErrorYlow(i_element);
      double this_high_syst_err = syst_spectrum->GetErrorYhigh(i_element);
      
      double this_rate = this_y * width_x;
      double this_low_syst_error = (this_low_syst_err/this_y)*this_rate;
      double this_high_syst_error = (this_high_syst_err/this_y)*this_rate;
      double this_stat_error = (this_stat_err/this_y)*this_rate;
      rate += this_rate;
      total_low_syst_error += (this_low_syst_error * this_low_syst_error);
      total_high_syst_error += (this_high_syst_error * this_high_syst_error);
      total_stat_error += (this_stat_error * this_stat_error);

      // std::cout << std::scientific;
      // std::cout << "AE: E = " << this_x //<< ", w = " << width_x << ", y = " << this_y
      // 	    << " +/- " << this_stat_err << " (stat.) + " << this_high_syst_err << " - " << this_low_syst_err 
      // 	    << ", rate = " << this_rate
      // 	    << " +/- " << this_stat_error << " (stat.) + " << this_high_syst_error << " - " << this_low_syst_error
      // 	    << ", total errors so far = " << std::sqrt(total_stat_error) << " (stat.) + "
      // 	    << std::sqrt(total_high_syst_error) << " - " << std::sqrt(total_low_syst_error) << std::endl;
    }
  }
  total_low_syst_error = std::sqrt(total_low_syst_error);
  total_high_syst_error = std::sqrt(total_high_syst_error);
  total_stat_error = std::sqrt(total_stat_error);
  //  std::cout << "Total Rate = " << rate << " +/- " << total_stat_error << " (stat.) + " << total_high_syst_error << " - " << total_low_syst_error << " (syst.)" << std::endl;
}
#endif
