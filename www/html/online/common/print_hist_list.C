
#include <fstream>

#include "gethist.C"

void print_hist_list(const int run_nr = 0)
{
  
  TObjArray *names = get_hist_list(run_nr);
  if ( !names ) return;

  ofstream *os = new ofstream("../tmp/hist_list.dat");

  TIter next (names);
  TObject *obj;
  while ( (obj = next()) )
    *os << obj->GetName() << " ";
  
  delete os;

}
