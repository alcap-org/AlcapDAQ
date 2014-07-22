// Author: Stefan Roiser 2004

// Copyright CERN, CH-1211 Geneva 23, 2004-2006, All rights reserved.
//
// Permission to use, copy, modify, and distribute this software for any
// purpose is hereby granted without fee, provided that this copyright and
// permissions notice appear in all copies and derivatives.
//
// This software is provided "as is" without express or implied warranty.

#include <cstring>
#include <cstdlib>

#include "demangle.h"

#if defined(__GNUC__)
#include <cxxabi.h>
#endif

namespace {
  // Return true if char is alpha or digit.
  bool isalphanum(int i) {
    return isalpha(i) || isdigit(i);
  }
  
  // Normalize a type name.
  std::string NormalizeName( const char * nam ) {
    std::string norm_name;
    char prev = 0;
    for (size_t i = 0; nam[i] != 0; i++) {
      char curr = nam[i];
      if (curr == ' ') {
        char next = 0;
        while (nam[i] != 0 && (next = nam[i + 1]) == ' ') {
          ++i;
        }
        if (!isalphanum(prev) || !isalpha(next)) {
          continue; // continue on non-word boundaries
        }
      } else if ((curr == '>' && prev == '>') 
                 || (curr == '(' && prev != ')')) {
        norm_name += ' ';
      }
      norm_name += (prev = curr);
    }
    
    return norm_name;
  }
  
  
  std::string NormalizeName( const std::string & nam ) {
    return NormalizeName(nam.c_str());
  }
}

// Demangle a type_info object.
std::string Except::Tools::Demangle( const std::type_info & ti ) { 
#if defined(_WIN32)
  static std::vector<std::string> keywords;
  if ( 0 == keywords.size() ) {
    keywords.push_back("class ");
    keywords.push_back("struct ");
    keywords.push_back("enum ");
    keywords.push_back("union ");
    keywords.push_back("__cdecl");
  }
  std::string r = ti.name();
  for ( size_t i = 0; i < keywords.size(); i ++ ) {
    while (r.find(keywords[i]) != std::string::npos) 
      r = r.replace(r.find(keywords[i]), keywords[i].size(), "");
    while (r.find(" *") != std::string::npos) 
      r = r.replace(r.find(" *"), 2, "*");
    while (r.find(" &") != std::string::npos) 
      r = r.replace(r.find(" &"), 2, "&");
  }
  return r;

#elif defined(__GNUC__)
  
  int status = 0;
  bool remove_additional_pointer = false;
  std::string  mangled = ti.name();
  
  // if the At Name is string return the final string Name 
  // abi::Demangle would return "std::string" instead
  if ( mangled == "Ss" ) return "std::basic_string<char>";
  
#if __GNUC__ <= 3 && __GNUC_MINOR__ <= 3
  // Function types are not decoded at all. We are an extra 'P' to convert it to a pointer
  // and remove it at the end.
  if ( mangled[0] == 'F' ) {
    mangled.insert(0,"P");
    remove_additional_pointer = true;
    }
#elif __GNUC__ >= 4
  // From gcc 4.0 on the fundamental types are not demangled anymore by the dynamic demangler
  if (mangled.length() == 1) {
    switch ( mangled[0] ) {
    case 'a': return "signed char";        break;
    case 'b': return "bool";               break;
    case 'c': return "char";               break;
    case 'd': return "double";             break;
    case 'e': return "long double";        break;
    case 'f': return "float";              break;
    case 'g': return "__float128";         break;
    case 'h': return "unsigned char";      break;
    case 'i': return "int";                break;
    case 'j': return "unsigned int";       break;
      //case 'k': return "";                   break;
    case 'l': return "long";               break;
    case 'm': return "unsigned long";      break;
    case 'n': return "__int128";           break;
    case 'o': return "unsigned __int128";  break;
      //case 'p': return "";                   break;
      //case 'q': return "";                   break;
      //case 'r': return "";                   break;
    case 's': return "short";              break;
    case 't': return "unsigned short";     break;
      //case 'u': return "";                   break;
    case 'v': return "void";               break;
    case 'w': return "wchar_t";            break;
    case 'x': return "long long";          break;
    case 'y': return "unsigned long long"; break;
    case 'z': return "...";                break;
    default:                               break;
    }
  }
#endif
  char * c_demangled = abi::__cxa_demangle( mangled.c_str(), 0, 0, & status );
  std::string demangled = c_demangled;
  std::free( c_demangled );
  if ( remove_additional_pointer ) {
    demangled = demangled.replace(demangled.find("(*)"), 3, "");
  }
  while ( demangled.find(", ") != std::string::npos ) {
    demangled = demangled.replace(demangled.find(", "), 2, ",");
  }
  return demangled;
  
#elif defined(__IBMCPP__)
  
  return NormalizeName(ti.name());
  
#endif
  return "";
}

