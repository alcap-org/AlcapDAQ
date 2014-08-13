#include "PictureBook.h"

#include <iostream>

PictureBook::PictureBook(std::string filename) {

  fLatexFile.open(filename.c_str(), std::ofstream::out);
  fLatexFile << "Picture Book created\n";
}

PictureBook::~PictureBook() {
  fLatexFile.close();
}
