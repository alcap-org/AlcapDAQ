#include "StoppedMuonDiscrepancy.C"

void StoppedMuonDiscrepancy_MultipleRuns() {

  //  int n_runs = 100;
  //  int first_run = 10362;
  int n_runs = 10;
  int first_run = 9735;
  int last_run = first_run+n_runs;

  for (int i_run = first_run; i_run < last_run; ++i_run) {
    StoppedMuonDiscrepancy(i_run);
  }
}
