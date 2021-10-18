#include "BrentDeckerMinimizer.h"

// The Brent-Decker Method
//
// The Decker Method wants to solve f(x) = 0
//   1. Find a_0 and b_0 such that they have opposite signs,
//          where b_k is the current guess for the root of f, 
//          and a_k is the "contrapoint" (i.e. where f(a_k) and f(b_k) have opposite signs, and f(b_k) is closer to 0)
//   (For the first iteration b_-1 = a_0)
//   2. b_{k+1} = b_{k} - [(b_{k} - b_{k-1})/(f(b_{k}) - f(b_{k-1}))]*f(b_{k}), if f(b_{k}) != f(b_{k-1})    [the secant method]
//      or, b_{k+1} = (a_{k} + b_{k}) / 2    [the linear interpolation]
//   3. a_{k+1} = a_{k}, if f(a_{k}) and f(b_{k+1}) have opposite signs
//      otherwise, a_{k+1} = b_{k} because they will have opposite signs
//
// The Brent Method adds two small modifications
//   1. Before using the next iterate
//      a. if previous step was bisection and
//           i. abs(delta) < abs(b_k - b_{k-1}), propose next iterate = secant method (interpolation)
//              else next iterate = bisection
//           ii. if abs(s - b_k) < 0.5*abs(b_k - b_{k-1}), use secant method
//               else use bisection
//
//         where delta is a specific numerical tolerance and s is the proposed next iterate
//      b. if previous step was interpolation (secant method)
//           i. abs(delta) < abs(b_{k-1} - b_{k-2}), propose next iterate is secant method,
//              else next iterate = bisection
//           ii. is abs(s - b_k) < 0.5*abs(b_{k-1} - b_{k-2}), use secant method
//               else use bisection
//   2. Use inverse quadratic interpolation rather than linear interpolation
// For this situation we want to minimize the gradient of the chi2 function

//TH1F* BrentDeckerMinimizer::hInitialParAttempts;

int BrentDeckerMinimizer::Minimize(int n_max_tries) {
  int i_try = 0;

  double f_a;
  int counter=0;
  // Find initial parameters such that f(a_0) and f(b_0) have opposite signs
  do {
    ++counter;
    
    for (int i_par = 0; i_par < fNPars; ++i_par) {
      a_k.at(i_par) = fParameters.at(i_par).fInitialGuess - (counter)*fParameters.at(i_par).fStep;
      b_k.at(i_par) = fParameters.at(i_par).fInitialGuess + (counter)*fParameters.at(i_par).fStep;
    }
    
    f_a = Gradient(a_k);
    f_b = Gradient(b_k);
    
  } while (f_a*f_b >= 0 && counter < 1000);

  //  hInitialParAttempts->Fill(counter);
  
  if (fPrintLevel > 0) {
    std::cout << "Initial Parameters: " << std::endl;
    std::cout << "a_0: ";
    PrintParameters(a_k);
    std::cout << "f(a_0) = " << f_a << std::endl;
    std::cout << "b_0: ";
    PrintParameters(b_k);
    std::cout << "f(b_0) = " << f_b << std::endl;
  }

  std::vector<double> s; // proposed next iterate
  s.resize(fNPars);
  std::vector<double> b_next;
  b_next.resize(fNPars);
  std::vector<double> b_pprev;
  b_pprev.resize(fNPars);
  b_prev = a_k;
  f_b_prev = f_a;
  std::vector<double> a_next;
  a_next.resize(fNPars);
  std::vector<double> a_prev;
  a_prev.resize(fNPars);

  double f_b_next;
  double f_a_next;
  bool success = false;
  bool use_bisection = false;
  bool previous_was_bisection = false;
  while (i_try < n_max_tries) {

    if (std::fabs(f_b) <= fLimit) {
      success = true;
      break;
    }

    // Want b_k to be the closest to zero so swap the vectors if needs be
    if (std::fabs(f_a) < std::fabs(f_b)) {
      if (fPrintLevel>0) {
	std::cout << "Swapping! " << std::endl;
	std::cout << "a_" << i_try << ": ";
	PrintParameters(a_k);
	std::cout << "b_" << i_try << ": ";
	PrintParameters(b_k);
      }
      
      a_k.swap(b_k);
    }


    // Find the next iterate
    //   2. b_{k+1} = b_{k} - [(b_{k} - b_{k-1})/(f(b_{k}) - f(b_{k-1}))]*f(b_{k}), if f(b_{k}) != f(b_{k-1})    [the secant method]
    //      or, b_{k+1} = (a_{k} + b_{k}) / 2    [the linear interpolation]
    //   3. a_{k+1} = a_{k}, if f(a_{k}) and f(b_{k+1}) have opposite signs
    //      otherwise, a_{k+1} = b_{k} because they will have opposite signs
    if (f_b != f_b_prev) { // secant method
      //      std::cout << "Secant!" << std::endl;
      SecantMethod(s);
    }
    else { // linear interpolation
      //      std::cout << "Linear!" << std::endl;
      BisectionMethod(s);
    }

    /*    if (previous_was_bisection) {
      // i. abs(delta) < abs(b_k - b_{k-1}), propose next iterate = secant method (interpolation)
      //    else next iterate = bisection
      // ii. if abs(s - b_k) < 0.5*abs(b_k - b_{k-1}), use secant method
      //     else use bisection
      for (int i_par = 0; i_par < fNPars; ++i_par) {
	double b_p = b_prev.at(i_par);
	double b = b_k.at(i_par);

	if (std::fabs(fLimit) < std::fabs(b - b_p)) {
	  SecantMethod(s, i_par);
	}
	else {
	  BisectionMethod(s, i_par);
	}

	double s_val = s.at(i_par);
	if (std::fabs(s_val - b) < 0.5*std::fabs(b - b_p)) {
	  previous_was_bisection = false;
	  SecantMethod(s, i_par);
	}
	else {
	  previous_was_bisection = true;
	  BisectionMethod(s, i_par);
	}
      }
    }
    else {
      // i. abs(delta) < abs(b_{k-1} - b_{k-2}), propose next iterate is secant method,
      //    else next iterate = bisection
      // ii. is abs(s - b_k) < 0.5*abs(b_{k-1} - b_{k-2}), use secant method
      //     else use bisection
      for (int i_par = 0; i_par < fNPars; ++i_par) {
	double b_pp = b_pprev.at(i_par);
	double b_p = b_prev.at(i_par);
	double b = b_k.at(i_par);

	if (std::fabs(fLimit) < std::fabs(b_p - b_pp)) {
	  SecantMethod(s, i_par);
	}
	else {
	  BisectionMethod(s, i_par);
	}

	double s_val = s.at(i_par);
	if (std::fabs(s_val - b) < 0.5*std::fabs(b_p - b_pp)) {
	  previous_was_bisection = false;
	  SecantMethod(s, i_par);
	}
	else {
	  previous_was_bisection = true;
	  BisectionMethod(s, i_par);
	}
      }
    }
    */

    for (int i_par = 0; i_par < fNPars; ++i_par) {
      b_next.at(i_par) = s.at(i_par);
    }

    f_b_next = Gradient(b_next);
    
    if(f_b_next*f_a<0) { // they have opposite signs (NB f_b is now the next iterate)
      a_next = a_k;// do nothing (a_next = a_k)
      f_a_next = f_a;
    }
    else {
      a_next = b_k;
      f_a_next = f_b;
    }

    // Move to the next iterate
    b_pprev = b_prev;
    b_prev = b_k;
    f_b_prev = f_b;
    b_k = b_next;
    f_b = f_b_next;
    
    a_prev = a_k;
    a_k = a_next;
    f_a = f_a_next;
    
    if (fPrintLevel > 0) {
      std::cout << "a_" << i_try + 1 << ": ";
      PrintParameters(a_k);
      std::cout << "f(a_" << i_try + 1 << ") = " << f_a << std::endl;
      std::cout << "b_" << i_try + 1 << ": ";
      PrintParameters(b_k);
      std::cout << "f(b_" << i_try + 1 << ") = " << f_b << std::endl;
    }


    ++i_try;
  }

  if (success && fPrintLevel > -1) {
    std::cout << "Final Parameters: " << std::endl;
    std::cout << "a_" << i_try + 1 << ": ";
    PrintParameters(a_k);
    std::cout << "f(a_" << i_try + 1 << ") = " << f_a << std::endl;
    std::cout << "b_" << i_try + 1 << ": ";
    PrintParameters(b_k);
    std::cout << "f(b_" << i_try + 1 << ") = " << f_b << std::endl;
    std::cout << "chi^2 = " << (*fFCN)(b_k) << std::endl;
  }

  return success;
}

void BrentDeckerMinimizer::PrintParameters(std::vector<double>& pars) {
  for (int i_par = 0; i_par < fNPars; ++i_par) {
    std::cout << fParameters.at(i_par).fName << ": " << pars.at(i_par) << ", ";
  }
}

double BrentDeckerMinimizer::Gradient(std::vector<double>& pars) {

  //  double best_gradient = 999999;
  
  for (int i_par = 0; i_par < fNPars; ++i_par) {
    lower.at(i_par) = pars.at(i_par) - fParameters.at(i_par).fStep;
    higher.at(i_par) = pars.at(i_par) + fParameters.at(i_par).fStep;
  }

  double f_1 = (*fFCN)(lower);
  double f_2 = (*fFCN)(higher);
  //  std::cout << "chi^2, f1 = " << f_1 << ", f_2 = " << f_2 << ", grad = " << f_1 - f_2 << std::endl;
  double this_gradient = (f_1 - f_2);

  return this_gradient;
}

