
//Framework
#include "tut.h"
#include "tut_reporter.h"

//C++/STL
#include <iostream>

//Local
#include "tut_externals.h"

namespace tut
{
  test_runner_singleton runner;
};

int main(int argc, char* argv[])
{
  tut::reporter reporter;
  tut::runner.get().set_callback(&reporter);
  tut::runner.get().run_tests();

  return !reporter.all_ok();
}  
