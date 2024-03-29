#include "TFile.h"
#include "TSystem.h"
#include "TMacro.h"
      
void importdir(const char *dirname) {
  //Example of script showing how to create a ROOT file with subdirectories.
  //The script scans a given directory tree and recreates the
  //same structure in the ROOT file.
  //All source files of type .h,cxx,c,dat,py are imported as TMacro objects
  //see also other tutorial readCode.C
      
   char *slash = (char*)strrchr(dirname,'/');
   char *locdir;
   if (slash) locdir = slash+1;
   else       locdir = (char*)dirname;
   printf("processing dir %s\n",dirname);
   TDirectory *savdir = gDirectory;
   TDirectory *adir = savdir->mkdir(locdir);
   adir->cd();
   void *dirp = gSystem->OpenDirectory(dirname);
   if (!dirp) return;
   char *direntry;
   Long_t id, size,flags,modtime;
   //loop on all entries of this directory
   while ((direntry=(char*)gSystem->GetDirEntry(dirp))) {
      TString afile = Form("%s/%s",dirname,direntry);
      gSystem->GetPathInfo(afile,&id,&size,&flags,&modtime);
      if (strstr(direntry,"G__")) continue;
      if (strstr(direntry,".c")    ||
          strstr(direntry,".h")    ||
          strstr(direntry,".m")    ||
          strstr(direntry,".dat")  ||
          strstr(direntry,".py")   ||
          strstr(direntry,".C")) {
         TMacro *m = new TMacro(afile);
         m->Write(direntry);
         delete m;
      } else {
         if (flags != 3)                     continue; //must be a directory
         if (direntry[0] == '.')             continue; //forget the "." amd ".." special cases
         if (!strcmp(direntry,"CVS"))        continue; //forget some special directories
         if (!strcmp(direntry,"htmldoc"))    continue;
         if (strstr(dirname,"root/include")) continue;
         //we have found a valid sub-directory. Process it
         importdir(afile);
     }
  }
  savdir->cd();
}
void importCode() {
   TFile *f = new TFile("code.root","recreate");
   importdir("../../root"); //change the directory as you like
   delete f;
}
