#ifndef DEMANGLE_H_
#define DEMANGLE_H_
// Author: Stefan Roiser 2004

// Copyright CERN, CH-1211 Geneva 23, 2004-2006, All rights reserved.
//
// Permission to use, copy, modify, and distribute this software for any
// purpose is hereby granted without fee, provided that this copyright and
// permissions notice appear in all copies and derivatives.
//
// This software is provided "as is" without express or implied warranty.

#include <typeinfo>
#include <string>

namespace Except {
  namespace Tools {
    /// Demangle will call the internal demangling routines and
    /// return the demangled name of a typeid.
    std::string Demangle( const std::type_info & ti );
  } 
}
#endif
