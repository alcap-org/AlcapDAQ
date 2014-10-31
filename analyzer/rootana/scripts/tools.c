// Equivilent to h1 += h2, but we have
// varying bin widths/bin edges
void AddEnergyHistograms(TH1* h1, const TH1* h2) {
  const TAxis *x1 = h1->GetXaxis(), *x2 = h2->GetXaxis();
  for (unsigned int bin_1 = 0; bin_1 <= h1->GetNbinsX(); ++bin_1) {
    Double_t low_edge_1 = x1->GetBinLowEdge(bin_1), high_edge_1 = x1->GetBinUpEdge(bin_1), bin_center_1 = x1->GetBinCenter(bin_1);
    Int_t low_bin_2 = h2->FindFixBin(low_edge_1), high_bin_2 = h2->FindFixBin(high_edge_1);
    if (high_bin_2 - low_bin_2 > 2) {
      printf("--------------------------------------------------------------------------------\n");
      printf("ADDING ENERGY HISTOGRAMS BINS NOT 1 APART: low=%d, high=%d\n", low_bin_2, high_bin_2);
      printf("--------------------------------------------------------------------------------\n");
    }
    if (low_bin_2 == high_bin_2 && low_bin_2 != 0 && low_bin_2 != h2->GetNbinsX()) {
      // If the first histogram's bin is completely
      // contained in the second histogram's bin,
      // only take some of it.
      Double_t overlap = x1->GetBinWidth(bin_1) / x2->GetBinWidth(low_bin_2);
      h1->Fill(bin_center_1, overlap * h2->GetBinContent(low_bin_2));
      continue;
    }
    if (high_bin_2 - low_bin_2 == 2) {
      // If the second histogram's bin is completely
      // contained in the first histogram's bin,
      // add that middle bin then continue.
      h1->Fill(bin_center_1, h2->GetBinContent( (high_bin_2+low_bin_2)/2 ));
    }
    if (low_bin_2 > 0 && low_bin_2 <= h2->GetNbinsX()) {
      Double_t high_edge_2 = x2->GetBinUpEdge(low_bin_2);
      Double_t overlap = (high_edge_2 - low_edge_1)/x2->GetBinWidth(low_bin_2);
      h1->Fill(bin_center_1, overlap * h2->GetBinContent(low_bin_2));
    }
    if (high_bin_2 > 0 && high_bin_2 <= h2->GetNbinsX()) {
      Double_t low_edge_2 = x2->GetBinLowEdge(high_bin_2);
      Double_t overlap = (high_edge_1 - low_edge_2)/x2->GetBinWidth(high_bin_2);
      h1->Fill(bin_center_1, overlap * h2->GetBinContent(high_bin_2));
    }
  }
}

/* FINISH THIS LATER */
unsigned int RearrangeMaskedArrays(const unsigned int n0, bool m[], Double_t x[], Double_t y[], Double_t ex[] = NULL, Double_t ey[] = NULL) {
  unsigned int n = n0 - 1;
  for (unsigned int i = 0; i < n; ++i) {
    if (!m[i]) {
      while (!m[n] && n-1 > i) --n;
      const Double_t tmp_x = x[n]; x[n] = x[i]; x[i] = tmp_x;
      const Double_t tmp_y = y[n]; y[n] = y[i]; y[i] = tmp_y;
      const bool     tmp_m = m[n]; m[n] = m[i]; m[i] = tmp_m;
      if (ex)
	const Double_t tmp_ex = ex[n]; ex[n] = ex[i]; ex[i] = tmp_ex;
      if (ey)
	const Double_t tmp_ey = ey[n]; ey[n] = ey[i]; ey[i] = tmp_ey;
    }
  }
  n = 0;
  for (unsigned int i = 0; i < n0; ++i) if (m[i]) ++n;
  return ++n;
}
