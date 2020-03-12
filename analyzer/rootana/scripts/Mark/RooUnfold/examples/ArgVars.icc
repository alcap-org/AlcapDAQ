//=====================================================================-*-C++-*-
// File and Version Information:
//      $Id$
//
// Description:
//      Parse argument list for parameter settings
//
// Author: Tim Adye <T.J.Adye@rl.ac.uk>
//
//==============================================================================

#ifndef ARGVARS_ICC
#define ARGVARS_ICC

#include "ArgVars.h"

#if !defined(__CINT__) || defined(__MAKECINT__)
#include <iostream>
#include <cstring>
#include <cstdlib>
#include "TObject.h"
#include "TList.h"
#include "TString.h"
#endif

ArgVars::~ArgVars()
{
}

ArgVars& ArgVars::Add(const ArgVars& args)
{
  TIter next(&args.lst);
  const TObject* arg;
  while ((arg= next())) lst.Add (arg->Clone());
  return *this;
}

ArgVars& ArgVars::Add (ArgVar* arg)
{
  const void* pvar;
  if      (arg->svar) pvar= arg->svar;
  else if (arg->ivar) pvar= arg->ivar;
  else                pvar= arg->fvar;
  TIter next(&lst);
  const ArgVar* argx;
  while ((argx= dynamic_cast<const ArgVar*>(next()))) {
    if (strcmp (argx->GetName(), arg->GetName()) == 0) {
      std::cerr << "ArgVars::Add("<<arg->GetName()<<") duplicate name in argument list" << std::endl;
      break;
    }
    if (pvar == argx->ivar || pvar == argx->fvar || pvar == argx->svar) {
      std::cerr << "ArgVars::Add("<<arg->GetName()<<") refers to the same variable as '" << argx->GetName() << "'" << std::endl;
      break;
    }
  }
  lst.Add (arg);
  return *this;
}

void ArgVars::Print (std::ostream& o, const char* sep) const
{
  Int_t kilroy= 0;
  TIter next(&lst);
  const ArgVar* arg;
  while ((arg= dynamic_cast<const ArgVar*>(next()))) {
    if (kilroy++) o << sep;
    o << arg->GetName() << '=';
    if      (arg->svar) o << *(arg->svar);
    else if (arg->ivar) o << *(arg->ivar);
    else                o << *(arg->fvar);
  }
}

void ArgVars::Usage (const char* prog) const
{
  std::cout << "Usage: " << prog << " [PARAMETER=VALUE [PARAMETER=VALUE ...]]" << std::endl << std::endl
            << "PARAMETER  DEFAULT DESCRIPTION" << std::endl;
  ArgHelp (std::cout);
}

void ArgVars::ArgHelp (std::ostream& o) const
{
  TIter next(&lst);
  const ArgVar* arg;
  while ((arg= dynamic_cast<const ArgVar*>(next()))) {
    std::ios_base::fmtflags f= o.flags();
    o.width(10); o.setf (std::ios_base::left, std::ios_base::adjustfield);
    o << arg->GetName() << ' ';
    o.flags (f); o.width(7);
    if (!arg->setdef)   o << " ";
    else if (arg->defhelp) o << arg->defhelp;
    else if (arg->svar) o << *(arg->svar);
    else if (arg->ivar) o << *(arg->ivar);
    else                o << *(arg->fvar);
    const char* help= arg->GetTitle();
    if (help && *help)  o << ' ' << help;
    o << std::endl;
  }
}

bool ArgVars::CmpOpt (const char* p, const char* opt, const char* s)
{
  if (s) return (strncmp (p, opt, s-p) == 0);
  else   return (strcmp  (p, opt)      == 0);
}

ArgVar* ArgVars::Find (const char* name) const
{
  return dynamic_cast<ArgVar*>(lst.FindObject(name));
}

ArgVars& ArgVars::SetDefault (const char* name, Double_t def)
{
  ArgVar* arg= Find (name);
  if (arg && arg->fvar)
    arg->fdef= def;
  else
    std::cerr << "ArgVars::SetDefault("<<name<<','<<def<<") Double_t argument does not exist" << std::endl;
  return *this;
}

ArgVars& ArgVars::SetDefault (const char* name, Int_t def)
{
  ArgVar* arg= Find (name);
  if (arg && arg->ivar)
    arg->idef= def;
  else
    std::cerr << "ArgVars::SetDefault("<<name<<','<<def<<") Int_t argument does not exist" << std::endl;
  return *this;
}

ArgVars& ArgVars::SetDefault (const char* name, const TString& def)
{
  ArgVar* arg= Find (name);
  if (arg && arg->svar)
    arg->sdef= def;
  else
    std::cerr << "ArgVars::SetDefault("<<name<<','<<def<<") TString argument does not exist" << std::endl;
  return *this;
}

void ArgVars::SetDefaults() const
{
  TIter next(&lst);
  const ArgVar* arg;
  while ((arg= dynamic_cast<const ArgVar*>(next()))) {
    if (!arg->setdef) continue;
    if      (arg->svar) arg->svar->Replace (0, arg->svar->Length(), arg->sdef);  // use Replace to work round CINT bug #75875
    else if (arg->ivar) *(arg->ivar)= arg->idef;
    else                *(arg->fvar)= arg->fdef;
  }
}

Int_t ArgVars::SetArgs (int argc, const char* const* argv, bool split) const
{
  SetDefaults();
  const char* prog= strrchr (argv[0], '/');
  if (prog) prog++; else prog= argv[0];
  int  err=  0;
  bool help= false;
  for (int i= 1; i<argc; i++) {
    const char* p= argv[i];
    while (p) {
      if (split) {
        while (*p==' ') p++;
        if (*p=='\0') break;
      }
      bool found= false;
      const char* s= split ? strchr (p, ' ') : 0;
      if (CmpOpt (p, "-h",     s) ||
          CmpOpt (p, "--help", s)) {
        help= true;
        if (s) {
          p= s+1;
          continue;
        }
        break;
      }
      ArgVar* arg;
      const char* q= 0, *d= strchr (p, '=');
      if (d) {
        size_t nv= d-p;
        if (split) {
          while (nv>0 && p[nv-1]==' ') nv--;
        }
        d++;
        char* argname= new char[nv+1];
        strncpy (argname, p, nv);
        argname[nv]= '\0';
        arg= Find (argname);
        delete [] argname; argname= 0;
        if (arg) {
          const char* nxt= 0;
          if      (arg->svar) {
            nxt= strchr (d, ' ');
            if (!nxt) nxt= d+strlen(d);
            arg->svar->Replace (0, arg->svar->Length(), d, nxt-d);
          }
          else if (arg->ivar) *(arg->ivar)= strtol (d, (char**)&nxt, 10);
          else                *(arg->fvar)= strtod (d, (char**)&nxt);
          if (split && nxt && *nxt==' ') q= nxt+1;
          found= (nxt && nxt>d && (*nxt=='\0' || (split && *nxt==' ')));
        }
      }
      if (!found) {
        std::cerr << prog << ": unrecognised parameter: " << p << std::endl;
        err++;
      }
      p= q;
    }
  };
  if (help) {
    Usage (prog);
    return 1;
  }
  if (err) return 2;
  return 0;
}

#endif
