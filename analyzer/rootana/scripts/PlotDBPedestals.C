void PlotDBPedestals() {

  const int max_n_runs = 2000;
  double run_numbers[max_n_runs];
  double pedestals[max_n_runs];
  double noises[max_n_runs];
  for (int i_run = 0; i_run < max_n_runs; ++i_run) {
    run_numbers[i_run] = pedestals[i_run] = noises[i_run] = 0;
  }

  const int n_dbs = 3;
  std::string db_names[n_dbs] = {"/gpfs/home/edmonds_a/calibration.db", "/gpfs/home/quirk_j/AlcapDAQ_2/analyzer/rootana/calibration.db", "/gpfs/home/wong_m/AlcapDAQ/analyzer/rootana/calibration.db"};
  int i_run_counter = 0;

  for (int i_db = 0; i_db < n_dbs; ++i_db) {
    char dbname[256];
    sprintf(dbname, "sqlite://%s", db_names[i_db].c_str());
    TSQLiteServer* db = new TSQLiteServer(dbname);

    char cmd[128];
    sprintf(cmd, "SELECT run, pedestal, noise FROM PedestalAndNoise WHERE channel==\"GeHiGain\"");
    TSQLiteResult* res = (TSQLiteResult*)db->Query(cmd);
    TSQLiteRow* row = NULL;

    while (row = (TSQLiteRow*)res->Next()) {
      double run_number = atof(row->GetField(0));
      double pedestal = atof(row->GetField(1));
      double noise = atof(row->GetField(2));
      std::cout << i_run_counter << ", " << run_number << ", " << pedestal << ", " << noise << std::endl;
      run_numbers[i_run_counter] = run_number;
      pedestals[i_run_counter] = pedestal;
      noises[i_run_counter] = noise;
      
      ++i_run_counter;
    }
    
    delete row;
    delete res;
  }

  TGraphErrors* gr = new TGraphErrors(max_n_runs, run_numbers, pedestals, NULL, noises);
  gr->Draw("A*");
  gr->SetTitle("Calculated Pedestal vs Run Number for GeHiGain");

  gr->GetXaxis()->SetRangeUser(9000, 10500);
  gr->GetXaxis()->SetTitle("Run Number");
  //  double y_min = 770; double y_max = 810;
  double y_min = 900; double y_max = 1050;
  gr->GetYaxis()->SetRangeUser(y_min, y_max);
  gr->GetYaxis()->SetTitle("Pedestal");

  // Si16b
  double first_Si16b_run = 10362;
  double last_Si16b_run = 10450;
  TBox* Si16b_box = new TBox(first_Si16b_run, y_min, last_Si16b_run, y_max);
  Si16b_box->SetLineColor(kRed);
  Si16b_box->SetFillStyle(0);
  Si16b_box->Draw();

  double Si16b_text_x = first_Si16b_run + (last_Si16b_run - first_Si16b_run) / 2;
  double Si16b_text_y = y_min + (3*(y_max - y_min)) / 4;
  std::cout << Si16b_text_x << ", " << Si16b_text_y << std::endl;
  TLatex* Si16b_text = new TLatex(Si16b_text_x, Si16b_text_y, "Si16b");
  Si16b_text->SetTextAlign(22);
  Si16b_text->SetTextColor(kRed);
  Si16b_text->Draw();

  // Al50
  double first_Al50_run = 9889;
  double last_Al50_run = 10129;
  TBox* Al50_box = new TBox(first_Al50_run, y_min, last_Al50_run, y_max);
  Al50_box->SetLineColor(kBlue);
  Al50_box->SetFillStyle(0);
  Al50_box->Draw();

  double Al50_text_x = first_Al50_run + (last_Al50_run - first_Al50_run) / 2;
  double Al50_text_y = y_min + (3*(y_max - y_min)) / 4;
  std::cout << Al50_text_x << ", " << Al50_text_y << std::endl;
  TLatex* Al50_text = new TLatex(Al50_text_x, Al50_text_y, "Al50");
  Al50_text->SetTextAlign(22);
  Al50_text->SetTextColor(kBlue);
  Al50_text->Draw();

  // Al100
  double first_Al100_run = 9655;
  double last_Al100_run = 9682;
  TBox* Al100_box = new TBox(first_Al100_run, y_min, last_Al100_run, y_max);
  Al100_box->SetLineColor(kBlack);
  Al100_box->SetFillStyle(0);
  Al100_box->Draw();

  double Al100_delta = 300;
  double Al100_text_x = first_Al100_run + (last_Al100_run - first_Al100_run) / 2 - Al100_delta;
  double Al100_text_y = y_min + (3*(y_max - y_min)) / 4;
  std::cout << Al100_text_x << ", " << Al100_text_y << std::endl;
  TLatex* Al100_text = new TLatex(Al100_text_x, Al100_text_y, "Al100 (Tranche 8)");
  Al100_text->SetTextAlign(22);
  Al100_text->Draw();

  //  TLine* text_line = new TLine(


  //  TLine* last_Si16b_line = new TLine(last_Si16b_run, y_min, last_Si16b_run, y_max);
  //  last_Si16b_line->SetLineColor(kRed);
  //  last_Si16b_line->Draw();
}
