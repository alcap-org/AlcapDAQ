#ifndef IntegrateRates_C
#define IntegrateRates_C

void IntegrateRates_wStatAndSystSpectra(TGraphAsymmErrors* stat_spectrum, TGraphAsymmErrors* syst_spectrum,
					double min_energy, double max_energy,
					double& rate, double& total_stat_error,
					double& total_high_syst_error, double& total_low_syst_error) {
  bool start_integral = false;

  rate = 0; total_stat_error = 0; total_high_syst_error = 0; total_low_syst_error = 0;
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
      double next_energy = 0; double next_y = 0;
      syst_spectrum->GetPoint(i_element+1, next_energy, next_y);
      double next_x = next_energy;
      double this_x = this_energy;
      double width_x = next_x - this_x;
      if (width_x == 0) {
	continue;
      }
      double this_stat_err = stat_spectrum->GetErrorYlow(i_element);
      double this_low_syst_err = syst_spectrum->GetErrorYlow(i_element);
      double this_high_syst_err = syst_spectrum->GetErrorYhigh(i_element);
      
      double this_rate = this_y * width_x;
      if (this_rate > 0) {
	double this_low_syst_error = (this_low_syst_err/this_y)*this_rate;
	double this_high_syst_error = (this_high_syst_err/this_y)*this_rate;
	double this_stat_error = (this_stat_err/this_y)*this_rate;
	rate += this_rate;
	total_low_syst_error += (this_low_syst_error * this_low_syst_error);
	total_high_syst_error += (this_high_syst_error * this_high_syst_error);
	total_stat_error += (this_stat_error * this_stat_error);
      
	// std::cout << std::scientific;
	//	std::cout << "E = " << this_x << ", y = " << this_y << std::endl;
	//	std::cout << "dE = " << width_x << std::endl;
	// std::cout << "\t this_rate = " << this_rate << std::endl;
	// std::cout << "\t this_stat_err = " << this_stat_err << ", this_stat_error = " << this_stat_error << std::endl;
	// std::cout << "\t this_low_syst_err = " << this_low_syst_err << ", this_low_syst_error = " << this_low_syst_error << std::endl;
	// std::cout << "\t this_high_syst_err = " << this_high_syst_err << ", this_high_syst_error = " << this_high_syst_error << std::endl;
	// // (stat.) + " << this_high_syst_err << " - " << this_low_syst_err 
	// //		<< ", rate = " << this_rate
	// //		<< " +/- " << this_stat_error << " (stat.) + " << this_high_syst_error << " - " << this_low_syst_error
	// //		<< ", total errors so far = " << std::sqrt(total_stat_error) << " (stat.) + "
	// //		<< std::sqrt(total_high_syst_error) << " - " << std::sqrt(total_low_syst_error) << std::endl;
      }
    }
  }
  total_low_syst_error = std::sqrt(total_low_syst_error);
  total_high_syst_error = std::sqrt(total_high_syst_error);
  total_stat_error = std::sqrt(total_stat_error);
  //  std::cout << "Total Rate = " << rate << " +/- " << total_stat_error << " (stat.) + " << total_high_syst_error << " - " << total_low_syst_error << " (syst.)" << std::endl;
}

void IntegrateRates_wStatAndStatSystSpectra(TGraphAsymmErrors* stat_spectrum,
					    TGraphAsymmErrors* statsyst_spectrum,
					    double min_energy, double max_energy,
					    double& rate, double& total_stat_error,
					    double& total_high_syst_error, double& total_low_syst_error) {

  TGraphAsymmErrors* syst_spectrum = (TGraphAsymmErrors*) statsyst_spectrum->Clone("syst_spectrum");
    
  for (int i_point = 0; i_point < syst_spectrum->GetN(); ++i_point) {
    double statsyst_hi = *(statsyst_spectrum->GetEYhigh() + i_point);
    double stat_hi = *(stat_spectrum->GetEYhigh() + i_point);
    double syst_hi = std::sqrt(statsyst_hi*statsyst_hi - stat_hi*stat_hi);
    *(syst_spectrum->GetEYhigh() + i_point) = syst_hi;

    double statsyst_lo = *(statsyst_spectrum->GetEYlow() + i_point);
    double stat_lo = *(stat_spectrum->GetEYlow() + i_point);
    double syst_lo = std::sqrt(statsyst_lo*statsyst_lo - stat_lo*stat_lo);
    *(syst_spectrum->GetEYlow() + i_point) = syst_lo;
  }
  IntegrateRates_wStatAndSystSpectra(stat_spectrum, syst_spectrum, min_energy, max_energy, rate, total_stat_error, total_high_syst_error, total_low_syst_error);
}

#endif
