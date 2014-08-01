#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <cstdio>

#include "AlcapExcept.h"
//#include "demangle.h"

#ifdef _GNU_SOURCE
#include <execinfo.h>
#endif

unsigned int Except::Base::gBacktraceSymbols = 5;

Except::Base::Base() {
  fTrace[0] = 0;
  fWhat[0] = 0;
#ifdef _GNU_SOURCE
  if (gBacktraceSymbols>0) {
    const unsigned int bufferSize=100;
    if (gBacktraceSymbols>bufferSize) gBacktraceSymbols = bufferSize;
    void *buffer[bufferSize];
    unsigned int frames = bufferSize;
    if (gBacktraceSymbols<frames) frames = gBacktraceSymbols;
    int nFrames = backtrace(buffer,frames);
    char **symbols = backtrace_symbols(buffer,nFrames);
    if (symbols != NULL) {
      unsigned int len = 0;
      std::strcat(fTrace,"Backtrace:\n");
      for (int i = 0; i<nFrames; ++i) {
        std::string sym(symbols[i]);
        std::string lib;
        std::string::size_type s = sym.find_last_of('/');
        if (s != std::string::npos) sym = sym.substr(s+1);
        s = sym.find_first_of('(');
        if (s != std::string::npos) lib = sym.substr(0,s);
        sym = sym.substr(s+1);
        std::string::size_type t = sym.find_first_of(')');
        if (s != std::string::npos
            && t != std::string::npos) sym = sym.substr(0,t);
        else sym="";
        if (lib.empty() && sym.empty()) continue;
        len += lib.length();
        len += sym.length() + 6;
        if (2*len>sizeof(fTrace)) break;
        std::strcat(fTrace," --> ");
        if (!lib.empty()) std::strcat(fTrace,lib.c_str());
        if (!sym.empty()) {
          std::strcat(fTrace,": ");
          std::strcat(fTrace,sym.c_str());
        }
        std::strcat(fTrace,"\n");
      }
      std::free(symbols);
    }
  }
#endif
  std::strcat(fWhat,"Except::Base");
}

void Except::Base::AppendWhat(const char* child) {
  unsigned int wLen = std::strlen(fWhat);
  unsigned int cLen = std::strlen(child);
  if (wLen+cLen < sizeof(fWhat)-5) {
    std::strcat(fWhat,"::");
    std::strcat(fWhat,child);
  }
}

void Except::Base::AppendWhat(const char* child, const char* message) {
  AppendWhat(child);
  unsigned int wLen = std::strlen(fWhat);
  unsigned int mLen = std::strlen(message);
  if (wLen+mLen < sizeof(fWhat)-9) {
    std::strcat(fWhat,"  \"");
    std::strcat(fWhat,message);
    std::strcat(fWhat,"\"");
  }
}

void Except::Base::AppendWhat(const char* child, const char* file, int line) {
  AppendWhat (child);
  unsigned int wLen = std::strlen(fWhat);
  unsigned int fLen = std::strlen(file);
  if (fLen+wLen < sizeof(fWhat)-16) {
    char buf[16];
    buf[0]=0;
    snprintf(buf,16,":%d)", line);
    std::strcat(fWhat," (");
    std::strcat(fWhat,file);
    std::strcat(fWhat,buf);
  }
}

