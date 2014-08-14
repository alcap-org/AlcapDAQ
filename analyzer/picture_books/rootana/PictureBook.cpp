#include "PictureBook.h"

#include <iostream>

PictureBook::PictureBook(std::string filename) {

  fLatexFile.open(filename.c_str(), std::ofstream::out);
  WritePreamble();
}

PictureBook::~PictureBook() {
  WriteEnd();
  fLatexFile.close();
}

void PictureBook::WritePreamble() {

   fLatexFile << "\\documentclass[12pt,twoside,titlepage]{article}\n";
   fLatexFile << "\\usepackage{geometry}\n";
   fLatexFile << "\\geometry{verbose,tmargin=10mm,bmargin=20mm,lmargin=10mm,rmargin=10mm}\n";
   fLatexFile << "\\usepackage{graphics}\n";
   fLatexFile << "\\usepackage{rotating}\n";
   fLatexFile << "\\usepackage{textcomp}\n";
   fLatexFile << "\\usepackage{subfigure}\n";
   fLatexFile << "\\usepackage{color}\n";
   fLatexFile << "\\usepackage[pdftex,bookmarks,colorlinks]{hyperref}\n\n\n";
   fLatexFile << "\\begin{document}\n";
   fLatexFile << "\\title{Picture Book!}\n";

   fLatexFile << "\\date{Produced \\today}\n\n";
   fLatexFile << "\\author{}\n\n";
   fLatexFile << "\\maketitle\n";
   fLatexFile << "\\newpage\n\n";
   fLatexFile << "\\tableofcontents\n\n";
   fLatexFile << "\\newpage\n\n";
}

void PictureBook::WriteEnd() {

   fLatexFile << "\\end{document}\n";
}

void PictureBook::StartNewSection(std::string section_name) {
  fLatexFile << "\\section{" << section_name << "}\n";
}

void PictureBook::InsertFigure(std::string image_file) {

  fLatexFile << "\\begin{figure}[h] \n";
  fLatexFile << "\\begin{center} \n";
  fLatexFile << "\\includegraphics[height=0.33\\textheight, angle=0]{" << image_file << "}\n" << std::endl;
  fLatexFile << "\\end{center} \n";
  fLatexFile << "\\vspace{-5mm} \n";
  fLatexFile << "\\caption{} \n";
  fLatexFile << "\\end{figure} \n";
}
